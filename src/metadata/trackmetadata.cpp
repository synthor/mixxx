#include "metadata/trackmetadata.h"

#include "util/math.h"

namespace Mixxx {

/*static*/ const double TrackMetadata::kBpmUndefined = 0.0;
/*static*/ const double TrackMetadata::kBpmMin = 0.0; // lower bound (exclusive)
/*static*/ const double TrackMetadata::kBpmMax = 300.0; // upper bound (inclusive)

/*static*/ const double TrackMetadata::kReplayGainUndefined = 0.0;
/*static*/ const double TrackMetadata::kReplayGainMin = 0.0; // lower bound (inclusive)
/*static*/ const double TrackMetadata::kReplayGain0dB = 1.0;

double TrackMetadata::parseBpm(const QString& sBpm, bool* pValid) {
    if (pValid) {
        *pValid = false;
    }
    if (sBpm.trimmed().isEmpty()) {
        return kBpmUndefined;
    }
    bool bpmValid = false;
    double bpm = sBpm.toDouble(&bpmValid);
    if (bpmValid) {
        if (kBpmUndefined == bpm) {
            // special case
            if (pValid) {
                *pValid = true;
            }
            return bpm;
        }
        while (kBpmMax < bpm) {
            // Some applications might store the BPM as an
            // integer scaled by a factor of 10 or 100 to
            // preserve fractional digits.
            qDebug() << "Scaling BPM value:" << bpm;
            bpm /= 10.0;
        }
        if (TrackMetadata::isBpmValid(bpm)) {
            if (pValid) {
                *pValid = true;
            }
            return bpm;
        } else {
            qDebug() << "Invalid BPM value:" << sBpm << "->" << bpm;
        }
    } else {
        qDebug() << "Failed to parse BPM:" << sBpm;
    }
    return kBpmUndefined;
}

QString TrackMetadata::formatBpm(double bpm) {
    if (TrackMetadata::isBpmValid(bpm)) {
        return QString::number(bpm);
    } else {
        return QString();
    }
}

QString TrackMetadata::formatBpm(int bpm) {
    if (TrackMetadata::isBpmValid(bpm)) {
        return QString::number(bpm);
    } else {
        return QString();
    }
}

namespace {

const QString kReplayGainUnit("dB");
const QString kReplayGainSuffix(" " + kReplayGainUnit);

} // anonymous namespace

double TrackMetadata::parseReplayGain(QString sReplayGain, bool* pValid) {
    if (pValid) {
        *pValid = false;
    }
    QString normalizedReplayGain(sReplayGain.trimmed());
    const int plusIndex = normalizedReplayGain.indexOf('+');
    if (0 == plusIndex) {
        // strip leading "+"
        normalizedReplayGain = normalizedReplayGain.mid(plusIndex + 1).trimmed();
    }
    const int unitIndex = normalizedReplayGain.lastIndexOf(kReplayGainUnit, -1, Qt::CaseInsensitive);
    if ((0 <= unitIndex) && ((normalizedReplayGain.length() - 2) == unitIndex)) {
        // strip trailing unit suffix
        normalizedReplayGain = normalizedReplayGain.left(unitIndex).trimmed();
    }
    if (normalizedReplayGain.isEmpty()) {
        return kReplayGainUndefined;
    }
    bool replayGainValid = false;
    const double replayGainDb = normalizedReplayGain.toDouble(&replayGainValid);
    if (replayGainValid) {
        const double replayGain = db2ratio(replayGainDb);
        DEBUG_ASSERT(kReplayGainUndefined != replayGain);
        // Some applications (e.g. Rapid Evolution 3) write a replay gain
        // of 0 dB even if the replay gain is undefined. To be safe we
        // ignore this special value and instead prefer to recalculate
        // the replay gain.
        if (kReplayGain0dB == replayGain) {
            // special case
            qDebug() << "Ignoring possibly undefined replay gain:" << formatReplayGain(replayGain);
            if (pValid) {
                *pValid = true;
            }
            return kReplayGainUndefined;
        }
        if (TrackMetadata::isReplayGainValid(replayGain)) {
            if (pValid) {
                *pValid = true;
            }
            return replayGain;
        } else {
            qDebug() << "Invalid replay gain value:" << sReplayGain << " -> "<< replayGain;
        }
    } else {
        qDebug() << "Failed to parse replay gain:" << sReplayGain;
    }
    return kReplayGainUndefined;
}

QString TrackMetadata::formatReplayGain(double replayGain) {
    if (isReplayGainValid(replayGain)) {
        return QString::number(ratio2db(replayGain)) + kReplayGainSuffix;
    } else {
        return QString();
    }
}

QString TrackMetadata::normalizeYear(QString year) {
    const QDateTime yearDateTime(parseDateTime(year));
    if (yearDateTime.isValid()) {
        return yearDateTime.toString();
    }
    const QDate yearDate(yearDateTime.date());
    if (yearDate.isValid()) {
        return yearDate.toString();
    }
    bool yearUIntValid = false;
    const int yearUInt = year.toUInt(&yearUIntValid);
    if (yearUIntValid) {
        if (0 == yearUInt) {
            // special case: empty
            return QString();
        }
        const QString yyyy(QString::number(yearUInt));
        if (4 == yyyy.length()) {
            return yyyy;
        }
    }
    // unmodified
    return year;
}

uint TrackMetadata::parseCalendarYear(QString year) {
    const QDateTime yearDateTime(parseDateTime(year));
    if (yearDateTime.date().isValid()) {
        return yearDateTime.date().year();
    } else {
        bool uintValid = false;
        uint uintYear = year.toUInt(&uintValid);
        if (uintValid) {
            return uintYear;
        }
    }
    return kCalendarYearInvalid;
}

QString TrackMetadata::formatCalendarYear(QString year) {
    const QDateTime yearDateTime(parseDateTime(year));
    if (yearDateTime.date().isValid()) {
        // numeric year
        return QString::number(yearDateTime.date().year());
    } else {
        bool uintValid = false;
        uint uintYear = year.toUInt(&uintValid);
        if (uintValid) {
            if (kCalendarYearInvalid == uintYear) {
                // empty string
                return QString();
            } else {
                // numeric string
                return QString::number(uintYear);
            }
        }
    }
    // unmodified
    return year;
}

TrackMetadata::TrackMetadata() :
        m_channels(0),
        m_sampleRate(0),
        m_bitrate(0),
        m_duration(0),
        m_bpm(kBpmUndefined),
        m_replayGain(kReplayGainUndefined) {
}

} //namespace Mixxx
