#pragma once

#include <cstdint>

// The game's frame rate, taken from the SDK's settings window (F1, Frame rate cap)
namespace graphics {

void Ui(double fps);  // inside the ImGui frame: applies the cap to the game's step and shown rate

}  // namespace graphics
