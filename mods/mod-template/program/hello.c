/*
 * This file is part of Foobar.
 *
 * Foobar is free software: you can redistribute it and/or modify it under the terms of the
 * Affero GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/>. See LICENSE file at root of this git repo.
 *
 * Copyright 2025 Author <author-contact@email-domain.tld>
 */
#define MOD_TEMPLATE_UI_STATE_IMPL
#include "hello.h"

void ModTemplateUIHello(ecs_iter_t *it) {
    ModTemplateUIState *data = ecs_singleton_get_mut(it->world, ModTemplateUIState);

    if (!data->show_template_window) return; // TODO make it as query filter ?

    ImGui_Begin("A mod Window", &data->show_template_window, ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui_Text("Hello from mod-template v231!");
    if (ImGui_Button("Close Me"))
        data->show_template_window = false;
    ImGui_End();
};

void ModTemplateUIInjectDemo(ecs_iter_t *it) {
    ModTemplateUIState *data = ecs_singleton_get_mut(it->world, ModTemplateUIState);

    ImGui_Begin("Hello, world!", NULL, 0);
    ImGui_Checkbox("mod-template Window", &data->show_template_window);
    ImGui_End();
};
void ModTemplateHelloImport(ecs_world_t *world) {
    ECS_MODULE(world, ModTemplateHello);

    // Component definition (+ setup metadata for reflection)
    ECS_META_COMPONENT(world, ModTemplateUIState);
    // Set this component as Singleton, set default values here at Import time
    ecs_singleton_set(world, ModTemplateUIState, {
        .show_template_window = true
    });

    // Systems definitions with a query on a Singleton
    //  Will run once per frame (because query will have always exactly 1 match)
    ECS_SYSTEM(world, ModTemplateUIHello, RenderingOnImGui, [inout] ModTemplateUIState);
    // We can inject new UI items at bottom of a Dear ImGui existing window by calling
    //  ImGui_Begin("Some window") a second time.
    ECS_SYSTEM(world, ModTemplateUIInjectDemo, RenderingOnImGui, [inout] ModTemplateUIState);
}

/*
 * Remarks about specificities of Flecs ECS :
 *
 * A component is an entity associated with the Component component.
 * A singleton in an entity associated with the Singleton component
 *  and a user defined component to hold the data.
 * Flecs use one entity for both, defining the user defined component and hold the data.
 */
