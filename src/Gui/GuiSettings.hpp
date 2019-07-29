#pragma once

#include "imgui.h"

#define NTL_IMGUI_WINDOW_NO_FLAGS 0
#define NTL_IMGUI_WINDOW_FLAGS_BASIC (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)
#define NTL_IMGUI_WINDOW_FLAGS_NO_INPUT (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMouseInputs)

#define NTL_IMGUI_COMBO_FLAGS (0)