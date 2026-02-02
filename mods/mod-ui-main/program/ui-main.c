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
#define MOD_TEMPLATE_UI_MAIN_IMPL
#include "ui-main.h"

void ModUiMainImport(ecs_world_t *world) {
    ECS_MODULE(world, ModUiMain);

    // Component definition (+ setup metadata for reflection)
    ECS_META_COMPONENT(world, ModUiMainState);
    // Set this component as Singleton, set default values here at Import time
    ecs_singleton_set(world, ModUiMainState, {
        .show_demo_window = false,
        .show_help_window = false,
        .statusbar_posx = 10000.0f,
    });
    //FIXME need to separate module definition and serializable entities
    // by design, ecs_world_to_json() excludes modules by default
    //ecs_add_id(world, ecs_id(ModUiMainState), Persisted);

    // Tasks definitions that will run once per frame
    ECS_SYSTEM(world, ModUiMainTask, RenderingOnImGui, 0);
}

// TODO rewire this data to ECS when ready
static ecs_i32_t app_status = 0;

void CustomMainMenu(ecs_world_t *world, ModUiMainState *ui_state);
void CustomStatusBar(SDL_Window *main_window);
void CustomStatusWidget();
// temporary mockup
ImU32 DummyStatusColor();

// Task, ran once per frame
void ModUiMainTask(ecs_iter_t *it) {
    const AppSDLContext *sdl_context = ecs_singleton_get_mut(it->world, AppSDLContext);
    SDL_Window *main_window = sdl_context->main_window;
    const ImGuiViewport* viewport = ImGui_GetMainViewport();
    ModUiMainState *ui_state = ecs_singleton_get_mut(it->world, ModUiMainState);

    // This has been already created in ImGuiSetupDockSpace(ecs_iter_t *it) in app-systems-core.c
    // Calling again _Begin allows to append to end (and nothing have been already added)
    ImGui_Begin("DockspaceWithMenuBar", NULL, 0);

    // Submit Main Menu Bar
    if (ImGui_BeginMenuBar()) {
        CustomMainMenu(it->world, ui_state);

        // Status Bar embed in Menu bar, right-aligned (1 frame late for simplicity)
        ImGui_SetCursorPosX(SDL_max(ImGui_GetCursorPosX(), ui_state->statusbar_posx));
        CustomStatusBar(main_window);
        ui_state->statusbar_posx += ImGui_GetWindowSize().x - ImGui_GetCursorPosX();

        ImGui_EndMenuBar();
    }

    ImGui_End(); /* DockspaceWithMenuBar Window */

    // Submit Quit confirmation modal popup
    ImVec2 pivot = {0.5f, 0.5f}, button_size = {120.0f, 0.0f};
    ImGui_SetNextWindowPosEx(ImGuiViewport_GetCenter(viewport), ImGuiCond_Appearing, pivot);
    if (ImGui_BeginPopupModal("Quit?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui_TextUnformatted("Do you really want to quit?");
        ImGui_Separator();

        if (ImGui_ButtonEx("Yes", button_size)) {
            ecs_add_pair(it->world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateAccepted);
            ImGui_CloseCurrentPopup();
        }
        ImGui_SetItemDefaultFocus();
        ImGui_SameLine();
        if (ImGui_ButtonEx("No", button_size)) {
            ecs_remove_pair(it->world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateWaitingUserReply);
            ImGui_CloseCurrentPopup();
        }
        ImGui_EndPopup();
    }
    // We trigger Quit confirmation on a ECS condition check because external (non ImGui) events could trigger it
    if ( ecs_has_pair(it->world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateResquested) ) {
        ecs_add_pair(it->world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateWaitingUserReply);
        ImGui_OpenPopup("Quit?", ImGuiPopupFlags_NoReopen);
    }

    /*
     * TODO put this at SceneLayout creation time (code does not exists yet)
     * ecs_get_name() garanties unique names so it's good for ImGui windows str_id
     * but user may want to rename scenes without loosing ImGui windows ID (to not loose some settings)
     * or app defined ImGui Window like "Properties" or so could be localized and user can change lang
     * so we store in in doc name the preformatted name as ImGui use it "Display Name###str_id"
        if (!SDL_asprintf(&win_name, "%s###%s", display_name, strid)) {
            app_error("%016"PRIu64" XXX(%s): SDL_asprintf(&win_name,...): %s",
                    SDL_GetTicksNS(), SDL_GetError());
        }
     */

    // Submit empty windows for now
    ecs_iter_t it2 = ecs_query_iter(it->world, ActivateLayoutVariantsQuery);
    while (ecs_query_next(&it2)) {
        for (int i = 0; i < it2.count; i++) {
            // ecs doc name have been feeded an ImGui compatible "Display Name###str_id"
            const char *win_name = ecs_doc_get_name(it->world, it2.entities[i]);
            ImGui_Begin(win_name, NULL, 0);
            ImGui_TextUnformatted(ecs_get_name(it->world, it2.entities[i]));
            ImGui_End();
        }
    }
    ImGui_Begin(_("Properties###Properties"), NULL, 0);
    ImGui_End();
    ImGui_Begin(_("Layers###Layers"), NULL, 0);
    ImGui_End();

    if (ui_state->show_demo_window)
        ImGui_ShowDemoWindow(&(ui_state->show_demo_window));
}

void CustomMainMenu(ecs_world_t *world, ModUiMainState *ui_state) {
        if (ImGui_BeginMenu(_("File###Menu|File"))) {
            if ( ImGui_MenuItem(_("Quit###Menu|Quit")) ) {
                ecs_add_pair(world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateResquested);
            }
            ImGui_EndMenu();
        }
        if (ImGui_BeginMenu(_("Edit###Edit"))) {
            ImGui_MenuItemEx(_("Preferences###Menu|show_preferences_window"), NULL, false, true);
            ImGui_EndMenu();
        }
        if (ImGui_BeginMenu(_("Options###Options"))) {
            ImGui_MenuItemBoolPtr(_("Demo Window###Menu|Options|show_demo_window"), NULL, &(ui_state->show_demo_window), true);

            ImGui_EndMenu();
        }
        if (ImGui_BeginMenu(_("Help###Help"))) {
            ImGui_MenuItemBoolPtr(_("Help Window###Menu|show_help_window"), NULL, &(ui_state->show_help_window), true);
            ImGui_EndMenu();
        }
}

void CustomStatusBar(SDL_Window *main_window) {
    //TODO consider putting it in a Window to let plugins append items on the status bar,
    //like a systray icon space or a browser extensions pinned items
    ImGui_BeginGroup();
    ImGui_Text("%s, %2.1f FPS", _("Idling"), ImGui_GetIO()->Framerate);

    ImGui_SameLineEx(0.0f, 8.0f);
    CustomStatusWidget();

    ImGui_SetNextItemShortcut(ImGuiKey_F11, 0);
    if ( ImGui_ArrowButton(_("Fullscreen###Fullscreen"), ImGuiDir_Up) ) {
        bool fullscreen = SDL_GetWindowFlags(main_window) & SDL_WINDOW_FULLSCREEN;
        SDL_SetWindowFullscreen(main_window, !fullscreen);

        app_status = (app_status+1)%4; //TODO: remove this dummy color test
    }
    ImGui_EndGroup();
}

ImU32 DummyStatusColor() {
    //TODO may directly choose enum values to colors? Offers plugin ways to define a new status.
    ImU32 color;
    switch (app_status) {
        case 1: color = IM_COL32(0, 200, 0, 255); break;
        case 2: color = IM_COL32(200, 200, 0, 255); break;
        case 3: color = IM_COL32(200, 0, 0, 255); break;
        default: color = IM_COL32(200, 200, 200, 255); break;
    }
    return color;
}

void CustomStatusWidget() {
    ImDrawList* draw_list = ImGui_GetWindowDrawList();
    ImVec2 screen_pos = ImGui_GetCursorScreenPos();

    float radius = 7.0f;

    ImVec2 center = {
        screen_pos.x + radius,
        screen_pos.y + ImGui_GetStyle()->FramePadding.y + radius
    };

    ImDrawList_AddCircleFilled(draw_list, center, radius, DummyStatusColor(), 16);
    ImGui_Dummy((ImVec2){radius*2.0f, radius*2.0f});
}
