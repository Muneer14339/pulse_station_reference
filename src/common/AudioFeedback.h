#pragma once

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QtGlobal>

#include <cmath>
#include <cstdint>

namespace AudioFeedback {

// ── Tunables ──────────────────────────────────────────────────────────────
static constexpr int   BeepFreqHz     = 1000;
static constexpr int   BeepDurationMs =   80;
static constexpr float BeepVolume     =  0.6f;
static constexpr int   FadeFrames     =  100;
static constexpr int   SampleRate     = 44100;
static constexpr int   Channels       = 1;
static constexpr int   BitsPerSample  = 16;
// ─────────────────────────────────────────────────────────────────────────

#pragma pack(push, 1)
struct WavHeader {
    char     riff[4]     = {'R','I','F','F'};
    uint32_t fileSize    = 0;
    char     wave[4]     = {'W','A','V','E'};
    char     fmt[4]      = {'f','m','t',' '};
    uint32_t fmtSize     = 16;
    uint16_t audioFmt    = 1;
    uint16_t ch          = 0;
    uint32_t sr          = 0;
    uint32_t byteRate    = 0;
    uint16_t blockAlign  = 0;
    uint16_t bps         = 0;
    char     data[4]     = {'d','a','t','a'};
    uint32_t dataSize    = 0;
};
#pragma pack(pop)

inline const QString& beepWavPath() {
    static const QString path = []() -> QString {
        const int samples   = SampleRate * BeepDurationMs / 1000;
        const int dataBytes = samples * Channels * (BitsPerSample / 8);

        WavHeader hdr;
        hdr.fileSize   = 36 + dataBytes;
        hdr.ch         = Channels;
        hdr.sr         = SampleRate;
        hdr.bps        = BitsPerSample;
        hdr.blockAlign = Channels * BitsPerSample / 8;
        hdr.byteRate   = SampleRate * hdr.blockAlign;
        hdr.dataSize   = dataBytes;

        QByteArray pcm(dataBytes, 0);
        auto* ptr = reinterpret_cast<int16_t*>(pcm.data());

        constexpr float kPi = 3.14159265358979323846f;
        for (int i = 0; i < samples; ++i) {
            float env = 1.0f;
            if (i < FadeFrames) {
                env = float(i) / float(FadeFrames);
            } else if (i >= samples - FadeFrames) {
                env = float(samples - 1 - i) / float(FadeFrames);
            }
            env = qBound(0.0f, env, 1.0f);

            const float sample =
                std::sin(2.0f * kPi * float(BeepFreqHz) * float(i) / float(SampleRate));

            ptr[i] = static_cast<int16_t>(32767.0f * BeepVolume * env * sample);
        }

        const QString p =
            QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                .filePath("PulseStation_ps_beep.wav");

        QFile f(p);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return QString();
        }

        if (f.write(reinterpret_cast<const char*>(&hdr), sizeof(WavHeader)) != sizeof(WavHeader)) {
            return QString();
        }
        if (f.write(pcm) != pcm.size()) {
            return QString();
        }

        f.close();
        return p;
    }();

    return path;
}

inline void playBeep() {
    const QString path = beepWavPath();
    if (path.isEmpty()) return;

    const QString aplay = QStandardPaths::findExecutable("aplay");
    if (!aplay.isEmpty()) {
        if (QProcess::startDetached(aplay, { path })) return;
    }

    const QString paplay = QStandardPaths::findExecutable("paplay");
    if (!paplay.isEmpty()) {
        QProcess::startDetached(paplay, { path });
    }
}

} // namespace AudioFeedback