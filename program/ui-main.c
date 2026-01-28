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

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable: 5287)
#endif
#include "dcimgui_internal.h" // Needed for unfinished DockBuilder API
#ifdef _MSC_VER
  #pragma warning(pop)
#endif

// TODO rewire this data to ECS when ready
static bool show_demo_window = false;
static bool show_help_window = false;
static ecs_i32_t app_status = 1;

void CustomMainMenu(ecs_world_t *world);
void CustomStatusWidget(ecs_i32_t app_status);

// Task, ran once per frame
void UIMain(ecs_iter_t *it) {
    const AppSDLContext *sdl_context = ecs_singleton_get_mut(it->world, AppSDLContext);
    const ImGuiViewport* viewport = ImGui_GetMainViewport();
//  const AppImGuiContext *ui_state = ecs_singleton_get_mut(it->world, UIMainRuntimeState);

    // Set a bunch of special properties for an always backgrounded and full-screened window
    ImGui_SetNextWindowPos(viewport->WorkPos, 0);
    ImGui_SetNextWindowSize(viewport->WorkSize, 0);
    ImGui_SetNextWindowViewport(viewport->ID);
    ImGui_PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui_PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGui_PushStyleVarImVec2(ImGuiStyleVar_WindowPadding, (ImVec2){ 0.0f, 0.0f });

    ImGui_Begin("DockspaceWithMenuBar", NULL, window_flags);
    ImGui_PopStyleVarEx(3);
    ImGuiID dockspace_id = ImGui_GetID("DockspaceWithMenuBar");
    // TODO query all ECS defined global bindings and set it here, to have them always routable

    {
        // Create dockspace default layout settings if they were never defined
        if (ImGui_DockBuilderGetNode(dockspace_id) == NULL) {
            ImGui_DockBuilderAddNodeEx(dockspace_id, ImGuiDockNodeFlags_DockSpace);
            ImGui_DockBuilderSetNodeSize(dockspace_id, viewport->Size);
            ImGuiID dock_id_right = 0;
            ImGuiID dock_id_main = dockspace_id;
            ImGui_DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.20f, &dock_id_right, &dock_id_main);
            ImGuiID dock_id_right_top = 0;
            ImGuiID dock_id_right_bottom = 0;
            ImGui_DockBuilderSplitNode(dock_id_right, ImGuiDir_Up, 0.50f, &dock_id_right_top, &dock_id_right_bottom);
            ImGui_DockBuilderDockWindow("Intro", dock_id_main);
            ImGui_DockBuilderDockWindow("FullCam", dock_id_main);
            ImGui_DockBuilderDockWindow("ScreenCam", dock_id_main);
            ImGui_DockBuilderDockWindow("Properties", dock_id_right_top);
            ImGui_DockBuilderDockWindow("Layers", dock_id_right_bottom);
            ImGui_DockBuilderFinish(dockspace_id);
        }
        // Submit dockspace
        ImGui_DockSpace(dockspace_id);
    }
    if (ImGui_BeginMenuBar()) {
        // Submit global Menu bar
        CustomMainMenu(it->world);
        // Submit global Status items inside the Menu bar
        ImGui_SetCursorPosX(SDL_max(ImGui_GetCursorPosX(), 500.f));
        ImGui_BeginGroup();
        ImGui_Text("Idling, %2.1f FPS", ImGui_GetIO()->Framerate);

        ImGui_SameLineEx(0.0f, 8.0f);
        CustomStatusWidget(app_status);

        if ( ImGui_ArrowButton("Fullscreen", ImGuiDir_Up) ) {
            SDL_Window *main_window = sdl_context->main_window;
            bool borderless = SDL_GetWindowFlags(main_window) & SDL_WINDOW_BORDERLESS;
            if (borderless) SDL_RestoreWindow(main_window); else SDL_MaximizeWindow(main_window);
            SDL_SetWindowBordered(main_window, borderless);

            app_status = (app_status+1)%4; //TODO: remove this dummy color test
        }

        ImGui_EndGroup();

        ImGui_EndMenuBar();
    }

    ImGui_End(); /* DockspaceWithMenuBar Window */

    // Submit modal popups
    ImVec2 center = viewport->Size;
    center.x /= 2; center.y /= 2; center.x += viewport->Pos.x; center.y += viewport->Pos.y;
    ImVec2 pivot = {0.5f, 0.5f};
    ImGui_SetNextWindowPosEx(center, ImGuiCond_Appearing, pivot);
    if (ImGui_BeginPopupModal("Quit?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui_TextUnformatted("Do you really want to quit?");
        ImGui_Separator();

        if (ImGui_ButtonEx("Yes", (ImVec2) {120.0f, 0.0f})) {
            ecs_add_pair(it->world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateAccepted);
            ImGui_CloseCurrentPopup();
        }
        ImGui_SetItemDefaultFocus();
        ImGui_SameLine();
        if (ImGui_ButtonEx("No", (ImVec2) {120.0f, 0.0f})) {
            ecs_remove_pair(it->world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateWaitingUserReply);
            ImGui_CloseCurrentPopup();
        }
        ImGui_EndPopup();
    }
    // We trigger this on a ECS condition check because external (non ImGui) events could trigger it
    if ( ecs_has_pair(it->world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateResquested) ) {
        ecs_add_pair(it->world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateWaitingUserReply);
        ImGui_OpenPopup("Quit?", ImGuiPopupFlags_NoReopen);
    }


    // Submit windows
    ImGui_Begin("Intro", NULL, 0);
    ImGui_End();
    ImGui_Begin("FullCam", NULL, 0);
    ImGui_End();
    ImGui_Begin("ScreenCam", NULL, 0);
    ImGui_End();
    ImGui_Begin("Properties", NULL, 0);
    ImGui_End();
    ImGui_Begin("Layers", NULL, 0);
    ImGui_End();

    if (show_demo_window)
        ImGui_ShowDemoWindow(&show_demo_window);
}

void CustomMainMenu(ecs_world_t *world) {
        if (ImGui_BeginMenu("File")) {
            if ( ImGui_MenuItem("Quit") ) {
                ecs_add_pair(world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateResquested);
            }
            ImGui_EndMenu();
        }
        if (ImGui_BeginMenu("Edit")) {
            ImGui_MenuItemEx("Preferences", NULL, false, true);
            ImGui_EndMenu();
        }
        if (ImGui_BeginMenu("Options")) {
            ImGui_MenuItemBoolPtr("Demo Window", NULL, &show_demo_window, true);
            ImGui_EndMenu();
        }
        if (ImGui_BeginMenu("Help")) {
            ImGui_MenuItemBoolPtr("Help Window", NULL, &show_help_window, true);
            ImGui_EndMenu();
        }
}

void CustomStatusWidget(ecs_i32_t app_status) {
        ImDrawList* draw_list = ImGui_GetWindowDrawList();
        ImVec2 screen_pos = ImGui_GetCursorScreenPos();

        float radius = 7.0f;

        ImVec2 center = {
            screen_pos.x + radius,
            screen_pos.y + ImGui_GetStyle()->FramePadding.y + radius
        };

        ImU32 color;
        switch (app_status) {
            case 1: color = IM_COL32(0, 200, 0, 255); break;
            case 2: color = IM_COL32(200, 200, 0, 255); break;
            case 3: color = IM_COL32(200, 0, 0, 255); break;
            default: color = IM_COL32(200, 200, 200, 255); break;
        }
        ImDrawList_AddCircleFilled(draw_list, center, radius, color, 16);
        ImGui_Dummy((ImVec2){radius*2.0f, radius*2.0f});
}

