#pragma once
/*
 * This file is part of LLSS.
 *
 * LLSS is free software: you can redistribute it and/or modify it under the terms of the
 * Affero GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * LLSS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with LLSS.
 * If not, see <https://www.gnu.org/licenses/>. See LICENSE file at root of this git repo.
 *
 * Copyright 2025 ludolpif <ludolpif@gmail.com>
 */
#include "app.h"

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// FLECS Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef MOD_TEMPLATE_UI_MAIN_IMPL
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

void ModUiMainImport(ecs_world_t *world);

// Components
MOD_API ECS_STRUCT(ModUiMainState, {
    bool show_demo_window;
    bool show_help_window;
    float statusbar_posx;
});
void ModUiMainTask(ecs_iter_t *it);
