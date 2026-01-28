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


/* Those code snippets aren't part of the compilation process, they are there because
 * I will need them later.
 */

#include <app.h>

void file_dialog_callback(void *userdata, const char * const *filelist, int filter) {
    app_info("first file: %s", filelist[0]);
}
void snip_collection() {
    SDL_Window *main_window = NULL;

    // Allow select only one file (bool at the end) with native file dialog
    SDL_ShowOpenFileDialog(file_dialog_callback, NULL, main_window, &(SDL_DialogFileFilter) { "All", "*" }, 1, SDL_GetBasePath(), false);
}
