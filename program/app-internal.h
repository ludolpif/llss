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
#include "mod-host.h"

// Same as in FFMPEG libavcodec, manually imported
// We aim to pipe data to ffmpeg program without linking to libav* using "ffmpeg -f nut -i -"
typedef struct AVRational {
	int num;
	int den;
} AVRational;

typedef struct appinternal {
	appmods_t mods;	
	AVRational framerate;
	Uint64 video_ts_origin;
	Uint64 video_frameid_origin;
	//TODO remove this dummy demo code, will be in the ECS
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;
	//...
} appinternal_t;
