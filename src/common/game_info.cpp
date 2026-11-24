#include <cstring>

#include "app/app.h"
#include "common/graphics.h"
#include "common/overlay.h"
#include "kernel/kernel.h"

// "Fill the screen" is ultrawide: the picture is 21:9, with bars above and below on a screen that is narrower (16:9),
// and fills a screen that is 21:9 or wider. On the narrower screen the shape is picked here, and g_wide_2d has the
// renderer stretch the targets and keep the 2D (menus, HUD) in a centred part. The game lays its 2D out for 4:3 even
// when it draws a widescreen picture, so that part is 4:3 (g_hud_4x3), at 16:9 as well: the 16:9 setting, or auto on a
// widescreen output, widens the targets the same way.
static void OnInit() {
    app::g_hud_4x3 = true;
    constexpr float kUltrawide = 21.0f / 9.0f;
    const app::Settings& s = app::Get();
    if (s.console_look || s.aspect == 1) return;
    if (s.aspect == 3) {
        app::Mode out = app::CachedOutput();
        if (!out.height || float(out.width) / float(out.height) < kUltrawide) {
            app::g_aspect = kUltrawide;
            app::g_wide_2d = true;
        }
    } else if (app::Widescreen()) {
        app::g_aspect = 16.0f / 9.0f;
        app::g_wide_2d = true;
    }
}

// The game takes the shape of a widescreen picture from a 16/9 constant in .rdata (0x189ACC, read when it sets up its
// views at boot: the 3D projection's aspect among them), the 4:3 one is the next float. The shown aspect goes in its place.
static void OnLoaded() {
    constexpr uint32_t kWideAspect = 0x189ACC;
    float aspect = app::Aspect();
    if (aspect > 16.0f / 9.0f + 0.01f) std::memcpy(kernel::HostPtr(kWideAspect), &aspect, 4);
}

static void OnUi(double fps) {
    overlay::Ui(fps);
    graphics::Ui(fps);
}

// what the runtime asks of the game; strings must be set, hooks may stay null
const app::GameInfo app::kGame = {
    .title = L"Burnout",                  // window and splash
    .short_title = L"Burnout",            // message boxes
    .name = L"Burnout",                   // window classes, crash dumps, handover event
    .log = "burnout",                     // log file prefix
    .settings = "settings.toml",          // settings file beside the exe
    .splash = L"splash",                  // optional splash.png or splash.bmp beside the exe
    .missing = L"Put the game in the assets folder: its disc image (.iso), or default.xbe with its folders beside it.",
    .images = {{"default.xbe", "Burnout"}},  // each recompiled xbe and its exe, for title launches
    .options = {{"show_fps", &overlay::show_fps, nullptr, "Show frame rate", "Also F10."}},
    .init = OnInit,                       // after the settings load, before the window and boot
    .loaded = OnLoaded,                   // image in memory, before its entry point runs
    .ui = OnUi,                           // ImGui drawing each frame: FPS (F10) and the game's frame rate
    .settings_window = true,              // F1: the SDK's settings window
};
