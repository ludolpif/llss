#pragma once
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
#include "app-components-mods.h"

#define APP_MOD_PATH_FROM_BASEPATH "%s../../../mods/"

#ifdef _DEBUG
#define APP_MOD_SUBDIR "program/x64/Debug/"
#else
#define APP_MOD_SUBDIR "program/x64/Release/"
#endif

#if defined(SDL_PLATFORM_WINDOWS)
#define APP_MOD_FILEEXT ".dll"
#elif defined(SDL_PLATFORM_APPLE)
#define APP_MOD_FILEEXT ".dylib"
#else
#define APP_MOD_FILEEXT ".so"
#endif

extern ECS_QUERY_DECLARE(ModReadyQuery);
void AppSystemsModsImport(ecs_world_t *world);
void ModRunInit(ecs_iter_t *it);
