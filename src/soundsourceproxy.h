#ifndef SOUNDSOURCEPROXY_H
#define SOUNDSOURCEPROXY_H

#include "trackinfoobject.h"

#include "sources/soundsourceproviderregistry.h"

#include "util/sandbox.h"

// Creates sound sources for filenames or tracks
class SoundSourceProxy: public Mixxx::MetadataSource {
public:
    static void loadPlugins(); // not thread-safe

    static QStringList supportedFileExtensions();
    static QStringList supportedFileExtensionsByPlugins();
    static QStringList supportedFileNamePatterns();
    static QRegExp supportedFileNameRegex();
    static bool isFilenameSupported(const QString& fileName);

    explicit SoundSourceProxy(QString qFilename, SecurityTokenPointer pToken = SecurityTokenPointer());
    explicit SoundSourceProxy(TrackPointer pTrack);

    QString getType() const {
        if (m_pSoundSource) {
            return m_pSoundSource->getType();
        } else {
            return QString();
        }
    }

    Result parseTrackMetadata(Mixxx::TrackMetadata* pMetadata) const /*override*/ {
        if (m_pSoundSource) {
            return m_pSoundSource->parseTrackMetadata(pMetadata);
        } else {
            return ERR;
        }
    }

    QImage parseCoverArt() const /*override*/ {
        if (m_pSoundSource) {
            return m_pSoundSource->parseCoverArt();
        } else {
            return QImage();
        }
    }

    // Opening the audio data through the proxy will
    // update the some metadata of the track object.
    // Returns a null pointer on failure.
    Mixxx::AudioSourcePointer openAudioSource(const Mixxx::AudioSourceConfig& audioSrcCfg = Mixxx::AudioSourceConfig());

    void closeAudioSource();

private:
    static Mixxx::SoundSourceProviderRegistry s_soundSourceProviders;

    static Mixxx::SoundSourcePointer initialize(const QString& qFilename);

    const TrackPointer m_pTrack;
    const SecurityTokenPointer m_pSecurityToken;

    const Mixxx::SoundSourcePointer m_pSoundSource;

    // Just an alias that keeps track of opening and closing
    // the corresponding SoundSource.
    Mixxx::AudioSourcePointer m_pAudioSource;
};

#endif
