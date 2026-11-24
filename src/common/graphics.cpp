#include "common/graphics.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iterator>

#include "app/app.h"
#include "core/log.h"
#include "kernel/kernel.h"

namespace graphics {

namespace {

// The game steps once per Swap by a fixed dt: a float at 0x664C8C (1/60 s, or 1/50 s on a PAL console) with the same
// in whole milliseconds beside it at 0x664C88. The F1 frame rate cap is the shown rate, at most 60 (unlimited and the
// monitor's refresh rate both mean 60; caps above it are treated as 60, stepping faster was choppy), and 30 shows
// every second step. When the machine cannot keep the steps up the dt follows the rate it manages, so the game keeps
// its speed and only the frame rate drops. Rates above 60 need frame interpolation (see TODO.md).
constexpr uint32_t kDtFloat = 0x664C8C, kDtMs = 0x664C88;
constexpr uint32_t kMinSteps = 60;
constexpr uint32_t kFpsCapFallback = 60;  // when the monitor's refresh rate is unreadable
uint32_t Bits(float f) {
    uint32_t b;
    std::memcpy(&b, &f, 4);
    return b;
}

// Capped at the game's own 60: stepping faster made the frame rate choppy.
uint32_t ShownRate() {
    uint32_t cap = app::Get().fps_cap;
    if (cap && cap != app::kFpsCapRefresh) return std::min(cap, kMinSteps);
    uint32_t refresh = app::RefreshRate();
    return refresh ? std::min(refresh, kMinSteps) : kFpsCapFallback;
}

// The rate the game steps at: the wanted one, or what the machine manages when it falls short. The step's dt follows it,
// so the game keeps its speed and only the frame rate drops.
double AchievedSteps(double shown_fps, uint32_t every, uint32_t steps) {
    constexpr double kShortfall = 0.97;  // under this share of the wanted rate the dt follows the real one
    constexpr double kSlowest = 30;      // below this the game slows down rather than step with a larger dt
    static double held = 0;
    double real = shown_fps * every;
    if (real <= 0) return held ? held : steps;
    double rate = real >= steps * kShortfall ? double(steps) : std::max(real, kSlowest);
    // the measure is a half second's average: a step too small to be real leaves the dt alone
    if (!held || std::abs(rate - held) > held * 0.03 || rate == steps) held = rate;
    return held;
}

void ApplyGameFps(double shown_fps) {
    if (!kernel::g_base) return;
    uint32_t fps = ShownRate();
    uint32_t steps = std::max(kMinSteps, fps);
    uint32_t every = std::max<uint32_t>(1, (steps + fps / 2) / fps);
    static uint32_t logged = 0;
    if (logged != fps) {
        logged = fps;
        LOGR("graphics: frame rate %u (steps %u/s, one in %u shown)", fps, steps, every);
    }
    app::g_step_rate = steps;
    app::g_show_every = every;
    app::g_vblank_rate = steps;
    app::SetEngineFps(steps);

    double rate = AchievedSteps(shown_fps, every, steps);
    // the game's own value (either video standard's) or our earlier one, nothing else is rewritten
    static uint32_t written_dt = 0, written_ms = 0;
    uint32_t dt = kernel::R32(kDtFloat), ms = kernel::R32(kDtMs);
    if (dt == Bits(1.0f / 60) || dt == Bits(1.0f / 50) || (written_dt && dt == written_dt)) {
        written_dt = Bits(float(1.0 / rate));
        kernel::W32(kDtFloat, written_dt);
    }
    if (ms == 16 || ms == 20 || (written_ms && ms == written_ms)) {
        written_ms = uint32_t(1000 / rate + 0.5);
        kernel::W32(kDtMs, written_ms);
    }
}

}  // namespace

void Ui(double fps) { ApplyGameFps(fps); }

}  // namespace graphics
