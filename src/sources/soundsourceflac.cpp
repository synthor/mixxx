/**
 * \file soundsourceflac.cpp
 * \author Bill Good <bkgood at gmail dot com>
 * \date May 22, 2010
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sources/soundsourceflac.h"

#include "metadata/trackmetadatataglib.h"
#include "sources/audiosourceflac.h"

#include <taglib/flacfile.h>

#include <QtDebug>

QList<QString> SoundSourceFLAC::supportedFileExtensions() {
    QList<QString> list;
    list.push_back("flac");
    return list;
}

<<<<<<< HEAD
SoundSourceFLAC::SoundSourceFLAC(QString filename)
<<<<<<< HEAD
    : Mixxx::SoundSource(filename)
    , m_file(filename)
    , m_decoder(NULL)
    , m_samples(0)
    , m_bps(0)
    , m_minBlocksize(0)
    , m_maxBlocksize(0)
    , m_minFramesize(0)
    , m_maxFramesize(0)
    , m_flacBuffer(NULL)
    , m_flacBufferLength(0)
    , m_leftoverBuffer(NULL)
    , m_leftoverBufferLength(0) {
    setType("flac");
=======
        : Super(filename, "flac"), m_file(filename), m_decoder(NULL), m_minBlocksize(
                0), m_maxBlocksize(0), m_minFramesize(0), m_maxFramesize(0), m_sampleScale(
                kSampleValueZero), m_decodeSampleBufferReadOffset(0), m_decodeSampleBufferWriteOffset(
                0) {
>>>>>>> New SoundSource/AudioSource API
=======
SoundSourceFLAC::SoundSourceFLAC(QString fileName)
<<<<<<< HEAD
        : Super(fileName, "flac") {
>>>>>>> Split AudioSource from SoundSource
=======
        : SoundSource(fileName, "flac") {
>>>>>>> Delete typedef Super (review comments)
}

Result SoundSourceFLAC::parseMetadata(Mixxx::TrackMetadata* pMetadata) const {
    TagLib::FLAC::File f(getFilename().toLocal8Bit().constData());

    if (!readAudioProperties(pMetadata, f)) {
        return ERR;
    }

    TagLib::Ogg::XiphComment *xiph(f.xiphComment());
    if (xiph) {
        readXiphComment(pMetadata, *xiph);
    } else {
        TagLib::ID3v2::Tag *id3v2(f.ID3v2Tag());
        if (id3v2) {
            readID3v2Tag(pMetadata, *id3v2);
        } else {
            // fallback
            const TagLib::Tag *tag(f.tag());
            if (tag) {
                readTag(pMetadata, *tag);
            } else {
                return ERR;
            }
        }
    }

    return OK;
}

QImage SoundSourceFLAC::parseCoverArt() const {
    TagLib::FLAC::File f(getFilename().toLocal8Bit().constData());
    QImage coverArt;
    TagLib::Ogg::XiphComment *xiph(f.xiphComment());
    if (xiph) {
        coverArt = Mixxx::readXiphCommentCover(*xiph);
    }
    if (coverArt.isNull()) {
        TagLib::ID3v2::Tag *id3v2(f.ID3v2Tag());
        if (id3v2) {
            coverArt = Mixxx::readID3v2TagCover(*id3v2);
        }
        if (coverArt.isNull()) {
            TagLib::List<TagLib::FLAC::Picture*> covers = f.pictureList();
            if (!covers.isEmpty()) {
                std::list<TagLib::FLAC::Picture*>::iterator it = covers.begin();
                TagLib::FLAC::Picture* cover = *it;
                coverArt = QImage::fromData(
                        QByteArray(cover->data().data(), cover->data().size()));
            }
        }
    }
    return coverArt;
}

<<<<<<< HEAD
// flac callback methods
FLAC__StreamDecoderReadStatus SoundSourceFLAC::flacRead(FLAC__byte buffer[],
        size_t *bytes) {
    *bytes = m_file.read((char*) buffer, *bytes);
    if (*bytes > 0) {
        return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    } else if (*bytes == 0) {
        return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
    } else {
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    }
}

FLAC__StreamDecoderSeekStatus SoundSourceFLAC::flacSeek(FLAC__uint64 offset) {
    if (m_file.seek(offset)) {
        return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
    } else {
        qWarning() << "SSFLAC: An unrecoverable error occurred ("
                << getFilename() << ")";
        return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    }
}

FLAC__StreamDecoderTellStatus SoundSourceFLAC::flacTell(FLAC__uint64 *offset) {
    if (m_file.isSequential()) {
        return FLAC__STREAM_DECODER_TELL_STATUS_UNSUPPORTED;
    }
    *offset = m_file.pos();
    return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

FLAC__StreamDecoderLengthStatus SoundSourceFLAC::flacLength(
        FLAC__uint64 *length) {
    if (m_file.isSequential()) {
        return FLAC__STREAM_DECODER_LENGTH_STATUS_UNSUPPORTED;
    }
    *length = m_file.size();
    return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

FLAC__bool SoundSourceFLAC::flacEOF() {
    if (m_file.isSequential()) {
        return false;
    }
    return m_file.atEnd();
}

FLAC__StreamDecoderWriteStatus SoundSourceFLAC::flacWrite(
<<<<<<< HEAD
    const FLAC__Frame *frame, const FLAC__int32 *const buffer[]) {
    unsigned int i(0);
    m_flacBufferLength = 0;
    if (getSampleRate() != frame->header.sample_rate) {
        qWarning() << "Corrupt FLAC file:"
                << "Invalid sample rate in FLAC frame header"
                << frame->header.sample_rate << "<>" << getSampleRate();
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }
    if (frame->header.blocksize > m_maxBlocksize) {
        qWarning() << "Corrupt FLAC file:"
                   << "Block size in FLAC frame header exceeds the maximum block size"
                   << frame->header.blocksize << ">" << m_maxBlocksize;
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }
    if (frame->header.channels > 1) {
        // stereo (or greater)
        for (i = 0; i < frame->header.blocksize; ++i) {
            m_flacBuffer[m_flacBufferLength++] = shift(buffer[0][i]); // left channel
            m_flacBuffer[m_flacBufferLength++] = shift(buffer[1][i]); // right channel
=======
        const FLAC__Frame *frame, const FLAC__int32 * const buffer[]) {
    if (getChannelCount() != frame->header.channels) {
        qWarning() << "Invalid number of channels in FLAC frame header:"
                << "expected" << getChannelCount() << "actual"
                << frame->header.channels;
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }
    Q_ASSERT(m_decodeSampleBufferReadOffset <= m_decodeSampleBufferWriteOffset);
    Q_ASSERT(
            (m_decodeSampleBuffer.size() - m_decodeSampleBufferWriteOffset)
                    >= frames2samples(frame->header.blocksize));
    switch (getChannelCount()) {
    case 1: {
        // optimized code for 1 channel (mono)
        Q_ASSERT(1 <= frame->header.channels);
        for (unsigned i = 0; i < frame->header.blocksize; ++i) {
            m_decodeSampleBuffer[m_decodeSampleBufferWriteOffset++] =
                    buffer[0][i] * m_sampleScale;
>>>>>>> New SoundSource/AudioSource API
        }
        break;
    }
    case 2: {
        // optimized code for 2 channels (stereo)
        Q_ASSERT(2 <= frame->header.channels);
        for (unsigned i = 0; i < frame->header.blocksize; ++i) {
            m_decodeSampleBuffer[m_decodeSampleBufferWriteOffset++] =
                    buffer[0][i] * m_sampleScale;
            m_decodeSampleBuffer[m_decodeSampleBufferWriteOffset++] =
                    buffer[1][i] * m_sampleScale;
        }
        break;
    }
    default: {
        // generic code for multiple channels
        for (unsigned i = 0; i < frame->header.blocksize; ++i) {
            for (unsigned j = 0; j < frame->header.channels; ++j) {
                m_decodeSampleBuffer[m_decodeSampleBufferWriteOffset++] =
                        buffer[j][i] * m_sampleScale;
            }
        }
    }
    }
    Q_ASSERT(m_decodeSampleBufferReadOffset <= m_decodeSampleBufferWriteOffset);
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void SoundSourceFLAC::flacMetadata(const FLAC__StreamMetadata *metadata) {
    switch (metadata->type) {
    case FLAC__METADATA_TYPE_STREAMINFO:
        setChannelCount(metadata->data.stream_info.channels);
        setFrameRate(metadata->data.stream_info.sample_rate);
        setFrameCount(metadata->data.stream_info.total_samples);
        m_sampleScale = kSampleValuePeak
                / sample_type(
                        FLAC__int32(1)
                                << metadata->data.stream_info.bits_per_sample);
        qDebug() << "FLAC file " << getFilename();
        qDebug() << getChannelCount() << " @ " << getFrameRate() << " Hz, "
                << getFrameCount() << " total, " << "bit depth"
                << " metadata->data.stream_info.bits_per_sample";
        m_minBlocksize = metadata->data.stream_info.min_blocksize;
        m_maxBlocksize = metadata->data.stream_info.max_blocksize;
        m_minFramesize = metadata->data.stream_info.min_framesize;
        m_maxFramesize = metadata->data.stream_info.max_framesize;
        qDebug() << "Blocksize in [" << m_minBlocksize << ", " << m_maxBlocksize
                << "], Framesize in [" << m_minFramesize << ", "
                << m_maxFramesize << "]";
        m_decodeSampleBufferReadOffset = 0;
        m_decodeSampleBufferWriteOffset = 0;
        m_decodeSampleBuffer.resize(m_maxBlocksize * getChannelCount());
        break;
    default:
        break;
    }
}

void SoundSourceFLAC::flacError(FLAC__StreamDecoderErrorStatus status) {
    QString error;
    // not much can be done at this point -- luckly the decoder seems to be
    // pretty forgiving -- bkgood
    switch (status) {
    case FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC:
        error = "STREAM_DECODER_ERROR_STATUS_LOST_SYNC";
        break;
    case FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER:
        error = "STREAM_DECODER_ERROR_STATUS_BAD_HEADER";
        break;
    case FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH:
        error = "STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH";
        break;
    case FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM:
        error = "STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM";
        break;
    }
    qWarning() << "SSFLAC got error" << error << "from libFLAC for file"
            << getFilename();
    // not much else to do here... whatever function that initiated whatever
    // decoder method resulted in this error will return an error, and the caller
    // will bail. libFLAC docs say to not close the decoder here -- bkgood
}

// begin callbacks (have to be regular functions because normal libFLAC isn't C++-aware)

FLAC__StreamDecoderReadStatus FLAC_read_cb(const FLAC__StreamDecoder*,
        FLAC__byte buffer[], size_t *bytes, void *client_data) {
    return ((SoundSourceFLAC*) client_data)->flacRead(buffer, bytes);
}

FLAC__StreamDecoderSeekStatus FLAC_seek_cb(const FLAC__StreamDecoder*,
        FLAC__uint64 absolute_byte_offset, void *client_data) {
    return ((SoundSourceFLAC*) client_data)->flacSeek(absolute_byte_offset);
}

FLAC__StreamDecoderTellStatus FLAC_tell_cb(const FLAC__StreamDecoder*,
        FLAC__uint64 *absolute_byte_offset, void *client_data) {
    return ((SoundSourceFLAC*) client_data)->flacTell(absolute_byte_offset);
}

FLAC__StreamDecoderLengthStatus FLAC_length_cb(const FLAC__StreamDecoder*,
        FLAC__uint64 *stream_length, void *client_data) {
    return ((SoundSourceFLAC*) client_data)->flacLength(stream_length);
}

FLAC__bool FLAC_eof_cb(const FLAC__StreamDecoder*, void *client_data) {
    return ((SoundSourceFLAC*) client_data)->flacEOF();
}

FLAC__StreamDecoderWriteStatus FLAC_write_cb(const FLAC__StreamDecoder*,
        const FLAC__Frame *frame, const FLAC__int32 * const buffer[],
        void *client_data) {
    return ((SoundSourceFLAC*) client_data)->flacWrite(frame, buffer);
}

void FLAC_metadata_cb(const FLAC__StreamDecoder*,
        const FLAC__StreamMetadata *metadata, void *client_data) {
    ((SoundSourceFLAC*) client_data)->flacMetadata(metadata);
}

void FLAC_error_cb(const FLAC__StreamDecoder*,
        FLAC__StreamDecoderErrorStatus status, void *client_data) {
    ((SoundSourceFLAC*) client_data)->flacError(status);
=======
Mixxx::AudioSourcePointer SoundSourceFLAC::open() const {
<<<<<<< HEAD
    return Mixxx::AudioSourceFLAC::open(getFilename());
>>>>>>> Split AudioSource from SoundSource
=======
    return Mixxx::AudioSourceFLAC::create(getFilename());
>>>>>>> Rename static factory method of AudioSources: open() -> create()
}