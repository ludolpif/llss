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
#define MOD_TEMPLATE_UI_STATE_IMPL
#include "compositing.h"

void ModScratchpadUIHello(ecs_iter_t *it) {
    ModScratchpadCompositingState *data = ecs_singleton_get_mut(it->world, ModScratchpadCompositingState);

    if (!data->show_scratchpad_window) return; // TODO make it as query filter ?

    ImGui_Begin("ModScratchpadCompositing", &data->show_scratchpad_window, ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui_Text("Hello from mod-scratchpad v3!");
    if (ImGui_Button("Close Me"))
        data->show_scratchpad_window = false;
    ImGui_End();
};

void ModScratchpadUIInjectDemo(ecs_iter_t *it) {
    ModScratchpadCompositingState *data = ecs_singleton_get_mut(it->world, ModScratchpadCompositingState);

    ImGui_Begin("Main", NULL, 0);
    ImGui_Checkbox("mod-scratchpad Window", &data->show_scratchpad_window);
    ImGui_End();
};
void ModScratchpadCompositingImport(ecs_world_t *world) {
    ECS_MODULE(world, ModScratchpadCompositing);

    // Component definition (+ setup metadata for reflection)
    ECS_META_COMPONENT(world, ModScratchpadCompositingState);
    // Set this component as Singleton, set default values here at Import time
    ecs_singleton_set(world, ModScratchpadCompositingState, {
        .show_scratchpad_window = true
    });

    // Systems definitions with a query on a Singleton
    //  Will run once per frame (because query will have always exactly 1 match)
    ECS_SYSTEM(world, ModScratchpadUIHello, RenderingOnImGui, [inout] ModScratchpadCompositingState);
    // We can inject new UI items at bottom of a Dear ImGui existing window by calling
    //  ImGui_Begin("Some window") a second time.
    ECS_SYSTEM(world, ModScratchpadUIInjectDemo, RenderingOnImGui, [inout] ModScratchpadCompositingState);
}

/*
 * Remarks about specificities of Flecs ECS :
 *
 * A component is an entity associated with the Component component.
 * A singleton in an entity associated with the Singleton component
 *  and a user defined component to hold the data.
 * Flecs use one entity for both, defining the user defined component and hold the data.
 */
