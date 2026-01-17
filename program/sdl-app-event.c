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
        //TODO pass to ECS
      }
      break;
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
      if (!imgui_io->WantCaptureKeyboard) {
        // SDL_KeyboardEvent key
        //TODO pass to ECS
      }
      break;
    case SDL_EVENT_USER:
      switch ( event->user.code ) {
#ifdef HAVE_INOTIFY_H
          case APP_USER_EVENT_INOTIFY:
              const struct inotify_event ievent = (const struct inotify_event *) event->user.data1;
            /* Afficher le type d’événement. */
            if (event->mask & IN_OPEN)
                printf("IN_OPEN: ");
            if (event->mask & IN_CLOSE_NOWRITE)
                printf("IN_CLOSE_NOWRITE: ");
            if (event->mask & IN_CLOSE_WRITE)
                printf("IN_CLOSE_WRITE: ");

            /* Afficher le nom du répertoire surveillé. */
            for (size_t i = 1; i < argc; ++i) {
                if (wd[i] == event->wd) {
                    printf("%s/", argv[i]);
                    break;
                }
            }

            /* Afficher le nom du fichier. */
            if (event->len)
                printf("%s", event->name);

            /* Afficher le type d’objet de système de fichiers. */
            if (event->mask & IN_ISDIR)
                printf(" [répertoire]\n");
            else
                printf(" [fichier]\n");
            break;
#endif /*HAVE_INOTIFY_H*/
          default:
            //TODO log?
        }
    default:
      //TODO log?
      break;
  }
  return then;
}

#ifdef HAVE_INOTIFY_H
void push_inotify_event_to_sdl_queue(const struct inotify_event *ievent) {
    SDL_Event user_event;
    SDL_zero(user_event);  /* SDL will copy this entire struct! Initialize to keep memory checkers happy. */
    user_event.type = SDL_EVENT_USER;
    user_event.user.code = APP_USER_EVENT_INOTIFY;
    user_event.user.data1 = ievent;
    user_event.user.data2 = NULL;
    SDL_PushEvent(&user_event);
}
#endif

