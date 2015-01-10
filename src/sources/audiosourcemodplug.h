#ifndef AUDIOSOURCEMODPLUG_H
#define AUDIOSOURCEMODPLUG_H

#include "sources/audiosource.h"
#include "util/defs.h"

namespace ModPlug {
#include <libmodplug/modplug.h>
}

#include <vector>

namespace Mixxx
{

// Class for reading tracker files using libmodplug.
// The whole file is decoded at once and saved
// in RAM to allow seeking and smooth operation in Mixxx.
class AudioSourceModPlug: public AudioSource {
public:
    static const size_type kChannelCount = 2; // always stereo
    static const size_type kFrameRate = 44100; // always 44.1kHz

    // apply settings for decoding
    static void configure(unsigned int bufferSizeLimit,
            const ModPlug::ModPlug_Settings &settings);

    static AudioSourcePointer create(QString fileName);

    ~AudioSourceModPlug();

    diff_type seekFrame(diff_type frameIndex) /*override*/;

    size_type readFrameSamplesInterleaved(size_type frameCount, sample_type* sampleBuffer) /*override*/;

private:
    static unsigned int s_bufferSizeLimit; // max track buffer length (bytes)

    AudioSourceModPlug();

    Result postConstruct(QString fileName);
    void preDestroy();

    ModPlug::ModPlugFile *m_pModFile; // modplug file descriptor
    unsigned long m_fileLength; // length of file in samples
    unsigned long m_seekPos; // current read position
    QByteArray m_fileBuf; // original module file data
    std::vector<SAMPLE> m_sampleBuf; // 16bit stereo samples, 44.1kHz
};

}

#endif
