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

SDL_AppResult SDL_AppEvent(void *_appstate, SDL_Event *event) {
	appstate_t *appstate = (appstate_t *) _appstate;
	SDL_Window *window = appstate->window;

	SDL_AppResult then = SDL_APP_CONTINUE;
	// Poll and handle events (inputs, window resize, etc.)
	// You can read the io->WantCaptureMouse, io->WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
	// - When io->WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
	// - When io->WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
	// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	cImGui_ImplSDL3_ProcessEvent(event);
	if (event->type == SDL_EVENT_QUIT)
		then = SDL_APP_SUCCESS;
	if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event->window.windowID == SDL_GetWindowID(window))
		then = SDL_APP_SUCCESS;

	return then;
}

