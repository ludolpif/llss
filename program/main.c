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
#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
/*
 * This compilation unit isn't empty because including SDL_main.h include implementation code
 *  and define the correct program entrypoint, usually main() but can be WinMain() or so.
 *
 * SDL main callbacks implementations are in sdl-app-init.c, sdl-app-event.c, sdl-app-iterate.c
 */
