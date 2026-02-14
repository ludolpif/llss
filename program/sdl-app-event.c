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
#include "dcimgui_impl_sdl3.h"

bool consume_user_defined_events(ecs_world_t *world, SDL_Event *event, Uint32 type);

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    ecs_world_t *world = (ecs_world_t *)appstate;
    /* From ImGui SDL3 integration examples:
     *  Poll and handle events (inputs, window resize, etc.)
     *  You can read the io->WantCaptureMouse, io->WantCaptureKeyboard flags to tell
     *   if dear imgui wants to use your inputs.
     *  When io->WantCaptureMouse is true, do not dispatch mouse input data to your main application,
     *   or clear/overwrite your copy of the mouse data.
     *  When io->WantCaptureKeyboard is true, do not dispatch keyboard input data to your main
     *   application, or clear/overwrite your copy of the keyboard data.
     * Generally you may always pass all inputs to dear imgui,
     *  and hide them from your application based on those two flags.
     */
    if ( cImGui_ImplSDL3_ProcessEvent(event) ) {
        // ImGui has processed current event, so don't try to process it furthuermore at SDL level
        return SDL_APP_CONTINUE;
    }

    const AppSDLContext *app_sdl_context = ecs_singleton_get(world, AppSDLContext);
    SDL_Window *main_window = app_sdl_context->main_window;

    switch (event->type) {
        case SDL_EVENT_QUIT:
            app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_QUIT", SDL_GetTicksNS());
            if ( ecs_has_pair(world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateWaitingUserReply) ) {
                // User has already been solicited, and doesn't have replied, forcibly quit
                ecs_add_pair(world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateAccepted);
            } else if ( ecs_has_pair(world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateResquested) ) {
                // The UI never display confirmation, but we are in a second SDL_EVENT_QUIT, forcibly quit
                ecs_add_pair(world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateAccepted);
            } else {
                ecs_add_pair(world, ecs_id(AppSDLContext), AppQuitState, AppQuitStateResquested);
            }
            break;
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            if ( event->window.windowID == SDL_GetWindowID(main_window) ) {
                app_info("%016"PRIu64" SDL_AppEvent(): main_window SDL_EVENT_WINDOW_CLOSE_REQUESTED", SDL_GetTicksNS());
                // If we continue here, there will be also an SDL_EVENT_QUIT event
            }
            break;
        case SDL_EVENT_DROP_FILE:
            app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_DROP_FILE: %s",
                    SDL_GetTicksNS(), event->drop.data);
            break;
        case SDL_EVENT_WINDOW_MOVED:
            app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_WINDOW_MOVED", SDL_GetTicksNS());
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_WINDOW_RESIZED", SDL_GetTicksNS());
            break;
        case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
            app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_WINDOW_ENTER_FULLSCREEN", SDL_GetTicksNS());
            break;
        case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
            app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_WINDOW_LEAVE_FULLSCREEN", SDL_GetTicksNS());
            break;
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED", SDL_GetTicksNS());
            break;
        case SDL_EVENT_WINDOW_SAFE_AREA_CHANGED:
            app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_WINDOW_SAFE_AREA_CHANGED", SDL_GetTicksNS());
            break;
        case SDL_EVENT_WINDOW_EXPOSED:
            app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_WINDOW_EXPOSED", SDL_GetTicksNS());
            break;

        default:
            if (!consume_user_defined_events(world, event, event->type)) {
                app_info("%016"PRIu64" SDL_AppEvent(): unhandled event->type: 0x%x",
                        SDL_GetTicksNS(), event->type);
            }
            break;
    }
    // See sdl-app-iterate.c for SDL_APP_SUCCESS / SDL_APP_FAILURE cases
    return SDL_APP_CONTINUE;
}

//TODO find a solution that allows plugin to do this too, and find a good way to represent it in the ECS
bool consume_user_defined_events(ecs_world_t *world, SDL_Event *event, Uint32 type) {
    if ( type == APP_USER_EVENT_FILESYSTEM ) {
        // Create current event entity
        ecs_entity_t parent = ecs_new_from_path(world, 0, "events.filesystem");
        ecs_entity_t event_entity = ecs_entity(world, { .parent = parent });
        // Set memory-managed AppDmonEvent component
        AppDmonEvent *devent = event->user.data1;
        ecs_set_ptr(world, event_entity, AppDmonEvent, devent);
        SDL_free(devent->rootdir);
        SDL_free(devent->filepath);
        SDL_free(devent->oldfilepath);
        SDL_free(devent);
        return true;
    }
    return false;
}
