#ifndef MIXXX_SOUNDSOURCEPROVIDERREGISTRY_H
#define MIXXX_SOUNDSOURCEPROVIDERREGISTRY_H

#include "sources/soundsourcepluginlibrary.h"

#include <QMap>
#include <QRegExp>

namespace Mixxx {

// Registry for SoundSourceProviders
class SoundSourceProviderRegistry {
public:
    void registerProvider(
            const SoundSourceProviderPointer& pProvider) {
        registerProviderPlugin(pProvider, SoundSourcePluginLibraryPointer());
    }
    void registerProviderPlugin(
            const SoundSourceProviderPointer& pProvider,
            const SoundSourcePluginLibraryPointer& pPluginLibrary);

    SoundSourceProviderPointer getProviderForFileType(
            const QString& fileType) const {
        return m_entries.value(fileType).pProvider;
    }

    QStringList getSupportedFileTypes() const {
        return m_entries.keys();
    }
    QStringList getSupportedPluginFileTypes() const;

    QStringList getSupportedFileNamePatterns() const;

    QRegExp getSupportedFileNameRegex() const;

    bool isSuppportedFileName(const QString& fileName) const {
        return fileName.contains(getSupportedFileNameRegex());
    }

private:
    struct Entry {
        SoundSourceProviderPointer pProvider;
        SoundSourcePluginLibraryPointer pPluginLibrary;
    };
    typedef QMap<QString, Entry> FileType2Entry;

    FileType2Entry m_entries;

    mutable QRegExp m_supportedFileNameRegex;
};

} // namespace Mixxx

#endif // MIXXX_SOUNDSOURCEPROVIDERREGISTRY_H
