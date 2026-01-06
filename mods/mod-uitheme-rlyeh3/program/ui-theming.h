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
#include "app.h"

/* Many themes in a non-code format
 * https://github.com/Patitotective/ImThemes/blob/main/themes.toml
 * 3 themes I feel may fit the project:
 * https://github.com/ocornut/imgui/issues/707#issuecomment-2993499592
 * https://github.com/ocornut/imgui/issues/707#issuecomment-2732535348
 * https://github.com/ocornut/imgui/issues/707#issuecomment-3341945111
 */

int igThemeV3(int hue07, int alt07, int nav07, int lit01 /*= 0*/, int compact01 /*= 0*/, int border01 /*= 1*/, int shape0123 /*= 1*/);
