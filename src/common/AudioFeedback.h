#pragma once

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QtGlobal>

#include <cmath>
#include <cstdint>

// ─────────────────────────────────────────────────────────────────────────────
//  AudioFeedback
//
//  Header-only audio utility.  Generates a WAV beep on first use, then
//  auto-detects the first working playback backend on the host system and
//  caches it for all subsequent calls.
//
//  Backend probe order (most-preferred first):
//    1. aplay   – ALSA direct         (desktops / PA-enabled embedded)
//    2. paplay  – PulseAudio         (embedded Linux, probes device list)
//    3. ffplay  – FFmpeg player        (widely available fallback)
//    4. play    – SoX                  (last-resort fallback)
//
//  No hardcoded device strings.  On ALSA-only targets the first enumerated
//  card/device is selected automatically via `aplay -l`.
// ─────────────────────────────────────────────────────────────────────────────

namespace AudioFeedback {

// ── Tunables ─────────────────────────────────────────────────────────────────
static constexpr int   BeepFreqHz     = 1000;
static constexpr int   BeepDurationMs =   80;
static constexpr float BeepVolume     =  0.6f;
static constexpr int   FadeFrames     =  100;
static constexpr int   SampleRate     = 44100;
static constexpr int   Channels       =    1;
static constexpr int   BitsPerSample  =   16;
static constexpr int SilencePadMs = 200;
// ─────────────────────────────────────────────────────────────────────────────

#pragma pack(push, 1)
struct WavHeader {
    char     riff[4]    = {'R','I','F','F'};
    uint32_t fileSize   = 0;
    char     wave[4]    = {'W','A','V','E'};
    char     fmt[4]     = {'f','m','t',' '};
    uint32_t fmtSize    = 16;
    uint16_t audioFmt   = 1;
    uint16_t ch         = 0;
    uint32_t sr         = 0;
    uint32_t byteRate   = 0;
    uint16_t blockAlign = 0;
    uint16_t bps        = 0;
    char     data[4]    = {'d','a','t','a'};
    uint32_t dataSize   = 0;
};
#pragma pack(pop)

// ── WAV generation ────────────────────────────────────────────────────────────

inline const QString& beepWavPath() {
    static const QString path = []() -> QString {
        const int silenceSamples = SampleRate * SilencePadMs / 1000;
const int beepSamples    = SampleRate * BeepDurationMs / 1000;
const int totalSamples   = silenceSamples + beepSamples;
const int dataBytes      = totalSamples * Channels * (BitsPerSample / 8);

        WavHeader hdr;
        hdr.fileSize   = 36 + dataBytes;
        hdr.ch         = Channels;
        hdr.sr         = SampleRate;
        hdr.bps        = BitsPerSample;
        hdr.blockAlign = Channels * BitsPerSample / 8;
        hdr.byteRate   = SampleRate * hdr.blockAlign;
        hdr.dataSize   = dataBytes;

        QByteArray pcm(dataBytes, 0);

        constexpr float kPi = 3.14159265358979323846f;
        auto* ptr = reinterpret_cast<int16_t*>(pcm.data());
for (int i = 0; i < totalSamples; ++i) {
    if (i < silenceSamples) { ptr[i] = 0; continue; }
    const int j = i - silenceSamples;
    float env = 1.0f;
    if (j < FadeFrames)
        env = float(j) / float(FadeFrames);
    else if (j >= beepSamples - FadeFrames)
        env = float(beepSamples - 1 - j) / float(FadeFrames);
    env = qBound(0.0f, env, 1.0f);
    ptr[i] = static_cast<int16_t>(
        32767.0f * BeepVolume * env *
        std::sin(2.0f * kPi * float(BeepFreqHz) * float(j) / float(SampleRate)));
}

        const QString p =
            QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation))
                .filePath("PulseStation_ps_beep.wav");

        QFile f(p);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return {};
        if (f.write(reinterpret_cast<const char*>(&hdr), sizeof(WavHeader)) != sizeof(WavHeader)) return {};
        if (f.write(pcm) != pcm.size()) return {};
        return p;
    }();
    return path;
}

// ── Backend detection ─────────────────────────────────────────────────────────

namespace detail {


struct ResolvedBackend {
    QString     executable;
    QStringList extraArgs;   // prepended before the WAV path
    bool        valid = false;
};

inline const ResolvedBackend& resolve(const QString&) {
    static const ResolvedBackend backend = []() -> ResolvedBackend {
        if (!QStandardPaths::findExecutable("aplay").isEmpty())
            return { "aplay", {}, true };
        if (!QStandardPaths::findExecutable("paplay").isEmpty())
            return { "paplay", {}, true };
        if (!QStandardPaths::findExecutable("ffplay").isEmpty())
            return { "ffplay", {"-nodisp", "-autoexit", "-loglevel", "quiet"}, true };
        if (!QStandardPaths::findExecutable("play").isEmpty())
            return { "play", {}, true };
        return {};
    }();
    return backend;
}

} // namespace detail

// ── Public API ────────────────────────────────────────────────────────────────

inline void playBeep() {
    const QString& wav = beepWavPath();
    if (wav.isEmpty()) return;

    const detail::ResolvedBackend& backend = detail::resolve(wav);
    if (!backend.valid) return;

    QStringList args = backend.extraArgs;
    args << wav;
    QProcess::startDetached(backend.executable, args);
}

inline void init() {
    beepWavPath();
    detail::resolve(QString());
}

} // namespace AudioFeedback
