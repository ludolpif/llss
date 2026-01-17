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
#include "ui-main.h"

// Task, ran once per frame
void UIMain(ecs_iter_t *it) {
    const AppImGuiContext *app_imgui_context = ecs_singleton_get(it->world, AppImGuiContext);
    float fr_float = app_imgui_context->imgui_io->Framerate;

    // TODO rewire this data to ECS when ready
    static bool show_demo_window;
    static ImVec4 clear_color;

    // 1. Show the big demo window (Most of the sample code is in ImGui_ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui_ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui_Begin("Main", NULL, 0);                          // Create a window called "Main" and append into it.

        ImGui_Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui_Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

        ImGui_SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui_ColorEdit3("clear color", (float*)&clear_color, 0); // Edit 3 floats representing a color

        if (ImGui_Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui_SameLine();
        ImGui_Text("counter = %d", counter);

        ImGui_Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / fr_float, fr_float);
        ImGui_Text("Some room for mods below this text.");

        ImGui_End();
    }
}
