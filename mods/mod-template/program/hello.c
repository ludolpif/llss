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
    ImGui_Begin("A mod Window", /*&data->show_another_window*/ NULL, 0);
    ImGui_Text("Hello from mod-template!");
//    if (ImGui_Button("Close Me"))
//        then = MOD_RESULT_SUCCESS;
    ImGui_End();
};

void ModTemplateHelloImport(ecs_world_t *world) {
    ECS_MODULE(world, ModTemplateHello);

    // Component definition (+ setup metadata for reflection)
    ECS_META_COMPONENT(world, ModTemplateUIState);

    // Tasks definitions, will run once per frame
    ECS_SYSTEM(world, ModTemplateUIHello, RenderingOnImGui, 0);
}
