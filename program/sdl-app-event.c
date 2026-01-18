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

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  ecs_world_t *world = (ecs_world_t *)appstate;

  const AppSDLContext *app_sdl_context = ecs_singleton_get(world, AppSDLContext);
  const AppImGuiContext *app_imgui_context = ecs_singleton_get(world, AppImGuiContext);
  SDL_Window *main_window = app_sdl_context->main_window;
  ImGuiIO *imgui_io = app_imgui_context->imgui_io;

  SDL_AppResult then = SDL_APP_CONTINUE;
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
  cImGui_ImplSDL3_ProcessEvent(event);
  switch (event->type) {
    case SDL_EVENT_QUIT:
      then = SDL_APP_SUCCESS;
      //TODO pass to ECS
      break;
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
      if ( event->window.windowID == SDL_GetWindowID(main_window) ) {
         then = SDL_APP_SUCCESS;
         //TODO pass to ECS
       }
      break;
    case SDL_EVENT_MOUSE_MOTION:
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
    case SDL_EVENT_MOUSE_WHEEL:
      if (!imgui_io->WantCaptureMouse) {
        // SDL_MouseMotionEvent motion
        // SDL_MouseButtonEvent button
        // SDL_MouseWheelEvent wheel
        // app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_MOUSE_*, unhandled event",
        //        SDL_GetTicksNS());
      }
      break;
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
      if (!imgui_io->WantCaptureKeyboard) {
        // SDL_KeyboardEvent key
          app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_KEY_*, unhandled event",
                  SDL_GetTicksNS());
      }
      break;
    case SDL_EVENT_USER:
      switch ( event->user.code ) {
          case APP_USER_EVENT_FILESYSTEM:
              {} // C99 can't have declaration right after a label

              // Create current event entity
              ecs_entity_t parent = ecs_new_from_path(world, 0, "events.filesystem");
              ecs_entity_t event_entity = ecs_entity(world, { .parent = parent });
              ecs_set_ptr(world, event_entity, AppDmonEvent, event->user.data1);

              //TODO SDL_free(dmon_event) when consumed;
          default:
              app_info("%016"PRIu64" SDL_AppEvent(): SDL_EVENT_USER, unknown event->user.code: %i",
                      SDL_GetTicksNS(), event->user.code);
        }
      break;
    default:
      app_info("%016"PRIu64" SDL_AppEvent(): unknown event->type: %i",
              SDL_GetTicksNS(), event->type);
      break;
  }
  return then;
}
