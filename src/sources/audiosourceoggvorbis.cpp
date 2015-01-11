#include "sources/audiosourceoggvorbis.h"

#include <vorbis/codec.h>

namespace Mixxx
{

AudioSourceOggVorbis::AudioSourceOggVorbis() {
    memset(&m_vf, 0, sizeof(m_vf));
}

AudioSourceOggVorbis::~AudioSourceOggVorbis() {
    close();
}

AudioSourcePointer AudioSourceOggVorbis::create(QString fileName) {
    QSharedPointer<AudioSourceOggVorbis> pAudioSource(new AudioSourceOggVorbis);
    if (OK == pAudioSource->open(fileName)) {
        // success
        return pAudioSource;
    } else {
        // failure
        return AudioSourcePointer();
    }
}

Result AudioSourceOggVorbis::open(QString fileName) {
    const QByteArray qbaFilename(fileName.toLocal8Bit());
    if (0 != ov_fopen(qbaFilename.constData(), &m_vf)) {
        qWarning() << "Failed to open OggVorbis file:" << fileName;
        return ERR;
    }

    if (!ov_seekable(&m_vf)) {
        qWarning() << "OggVorbis file is not seekable:" << fileName;
        return ERR;
    }

    // lookup the ogg's channels and samplerate
    const vorbis_info* vi = ov_info(&m_vf, -1);
    if (!vi) {
        qWarning() << "Failed to read OggVorbis file:" << fileName;
        return ERR;
    }
    setChannelCount(vi->channels);
    setFrameRate(vi->rate);

    ogg_int64_t frameCount = ov_pcm_total(&m_vf, -1);
    if (0 <= frameCount) {
        setFrameCount(frameCount);
    } else {
        qWarning() << "Failed to read OggVorbis file:" << fileName;
        return ERR;
    }

    return OK;
}

void AudioSourceOggVorbis::close() {
    const int clearResult = ov_clear(&m_vf);
    if (0 != clearResult) {
        qWarning() << "Failed to close OggVorbis file" << clearResult;
    }
    reset();
}

AudioSource::diff_type AudioSourceOggVorbis::seekSampleFrame(
        diff_type frameIndex) {
    const int seekResult = ov_pcm_seek(&m_vf, frameIndex);
    if (0 != seekResult) {
        qWarning() << "Failed to seek OggVorbis file:" << seekResult;
    }
    return ov_pcm_tell(&m_vf);
}

AudioSource::size_type AudioSourceOggVorbis::readSampleFrames(
        size_type numberOfFrames, sample_type* sampleBuffer) {
    return readSampleFrames(numberOfFrames, sampleBuffer, frames2samples(numberOfFrames), false);
}

AudioSource::size_type AudioSourceOggVorbis::readSampleFramesStereo(
        size_type numberOfFrames,
        sample_type* sampleBuffer,
        size_type sampleBufferSize) {
    return readSampleFrames(numberOfFrames, sampleBuffer, sampleBufferSize, true);
}

AudioSource::size_type AudioSourceOggVorbis::readSampleFrames(
        size_type numberOfFrames,
        sample_type* sampleBuffer, size_type sampleBufferSize,
        bool readStereoSamples) {
    sample_type* nextSample = sampleBuffer;
    const size_type numberOfFramesTotal = math_min(numberOfFrames, samples2frames(sampleBufferSize));
    size_type numberOfFramesRead = 0;
    while (numberOfFramesTotal > numberOfFramesRead) {
        float** pcmChannels;
        int currentSection;
        const long readResult = ov_read_float(&m_vf, &pcmChannels,
                numberOfFramesTotal - numberOfFramesRead, &currentSection);
        if (0 == readResult) {
            // EOF
            break; // done
        }
        if (0 < readResult) {
            if (isChannelCountMono()) {
                if (readStereoSamples) {
                    for (long i = 0; i < readResult; ++i) {
                        *nextSample++ = pcmChannels[0][i];
                        *nextSample++ = pcmChannels[0][i];
                    }
                } else {
                    for (long i = 0; i < readResult; ++i) {
                        *nextSample++ = pcmChannels[0][i];
                    }
                }
            } else if (isChannelCountStereo() || readStereoSamples) {
                for (long i = 0; i < readResult; ++i) {
                    *nextSample++ = pcmChannels[0][i];
                    *nextSample++ = pcmChannels[1][i];
                }
            } else {
                for (long i = 0; i < readResult; ++i) {
                    for (size_type j = 0; j < getChannelCount(); ++j) {
                        *nextSample++ = pcmChannels[j][i];
                    }
                }
            }
            numberOfFramesRead += readResult;
        } else {
            qWarning() << "Failed to read from OggVorbis file:" << readResult;
            break; // abort
        }
    }
    return numberOfFramesRead;
}

} // namespace Mixxx