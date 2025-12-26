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
#include "app-internal.h"
#include "dcimgui_impl_sdl3.h"
#include "dcimgui_impl_sdlgpu3.h"
#include "alloc.h"
#include "mod-host.h"

#define app_failure(...) do { SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__); return SDL_APP_FAILURE; } while(SDL_NULL_WHILE_LOOP_CONDITION)

// For now disable multiple viewports and docking as API may change, because Omar want to rework them
#undef IMGUI_HAS_VIEWPORT
#undef IMGUI_HAS_DOCK

#define APP_UI_DEFAULT_WIDTH 1280
#define APP_UI_DEFAULT_HEIGHT 720

// Forward declarations
void app_dummy_demo_code_init(appstate_t *appstate);

// Implementations
SDL_AppResult SDL_AppInit(void **_appstate, int argc, char **argv) {
	// Configure memory functions before the first dynamic allocation
	alloc_count_install_hooks();

	// Allocate on heap applications state structures
	*_appstate = SDL_calloc(1,sizeof(appstate_t));
	if (!*_appstate)
		app_failure("SDL_calloc(1,sizeof(appstate_t))");
	appstate_t *appstate = *_appstate;

	appinternal_t *internal = SDL_calloc(1,sizeof(appinternal_t));
	if (!internal)
		app_failure("SDL_calloc(1,sizeof(appinternal_t))");

	// Configure logging
	SDL_LogPriority logpriority_earlyskip = SDL_GetLogPriority(SDL_LOG_CATEGORY_APPLICATION);
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_TRACE,    "TRACE ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_VERBOSE,  "VERB  ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_DEBUG,    "DEBUG ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INFO,     "INFO  ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_WARN,     "WARN  ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_ERROR,    "ERROR ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_CRITICAL, "CRIT  ");

	// Set metadata before SDL_Init because it will print it if loglevel is high enough
	// We don't check return value, we don't want to abort the app startup if this fails anyway.
	app_warn("Starting %s %s with log priority %d for app", APP_METADATA_NAME_STRING, APP_VERSION_STR, logpriority_earlyskip);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, APP_METADATA_NAME_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, APP_VERSION_STR);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, APP_METADATA_IDENTIFIER_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, APP_METADATA_CREATOR_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, APP_METADATA_COPYRIGHT_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, APP_METADATA_URL_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, APP_METADATA_TYPE_STRING);

	// Early log message to help troubleshoot application init and allow human readable timestamps later conversion
	// Note SDL_Ticks should be a CLOCK_MONOTONIC source, but some platforms may not provide it
	// SDL_Time is real time, subject to system clock adjustment
	SDL_Time tick0_wallclock;
	if (!SDL_GetCurrentTime(&tick0_wallclock))
		app_failure("SDL_GetCurrentTime(): %s", SDL_GetError());

	Uint64 tick = SDL_GetTicksNS();
	tick0_wallclock -= tick;
	app_info("%016"PRIu64" SDL_AppInit(): tick0_wallclock==%"PRId64, tick, tick0_wallclock);
	app_info("%016"PRIu64" SDL_GetBasePath(): %s", SDL_GetTicksNS(), SDL_GetBasePath());

	if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_CAMERA))
		app_failure("SDL_Init(): %s", SDL_GetError());

	// Create SDL window graphics context
	float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	SDL_Window* window = SDL_CreateWindow("LLSS Mockup", (int)(APP_UI_DEFAULT_WIDTH * main_scale), (int)(APP_UI_DEFAULT_HEIGHT * main_scale), window_flags);
	if (window == NULL)
		app_failure("SDL_CreateWindow(): %s", SDL_GetError());
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window);

	// Create GPU Device
	SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB,true,NULL);
	if (gpu_device == NULL)
		app_failure("SDL_CreateGPUDevice(): %s", SDL_GetError());

	// Claim window for GPU Device
	if (!SDL_ClaimWindowForGPUDevice(gpu_device, window))
		app_failure("SDL_ClaimWindowForGPUDevice(): %s", SDL_GetError());

	// Try to apply a new SDL_GPUPresentMode, respecting this preference order : MAILBOX > IMMEDIATE > VSYNC
	// VSYNC mode will always be supported. IMMEDIATE and MAILBOX modes may not be supported on certain systems.
	// IMMEDIATE: Immediately presents. Lowest latency option, but tearing may occur.
	// MAILBOX: Waits for vblank before presenting. No tearing is possible.
	// If there is a pending image to present, the pending image is replaced by the new image.
	// Similar to VSYNC, but with reduced visual latency.

	SDL_GPUPresentMode present_mode = SDL_GPU_PRESENTMODE_VSYNC;
	// API doc says: The swapchain will be created with SDL_GPU_SWAPCHAINCOMPOSITION_SDR and SDL_GPU_PRESENTMODE_VSYNC.

	bool can_mailbox = SDL_WindowSupportsGPUPresentMode(gpu_device, window, SDL_GPU_PRESENTMODE_MAILBOX);
	app_info("%016"PRIu64" SDL_WindowSupportsGPUPresentMode(..., MAILBOX): %s", SDL_GetTicksNS(), can_mailbox?"true":"false");

	if ( present_mode != SDL_GPU_PRESENTMODE_MAILBOX ) {
		// Switch to MAILBOX mode if avaiable
		if (can_mailbox) {
			if (!SDL_SetGPUSwapchainParameters(gpu_device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX)) {
				app_warn("%016"PRIu64" SDL_SetGPUSwapchainParameters(..., SDR, MAILBOX) failed", SDL_GetTicksNS());
			}
			present_mode = SDL_GPU_PRESENTMODE_MAILBOX;
		}
	}
	if ( present_mode != SDL_GPU_PRESENTMODE_MAILBOX ) {
		// Fallback to IMMEDIATE mode if avaiable
		bool can_immediate = SDL_WindowSupportsGPUPresentMode(gpu_device, window, SDL_GPU_PRESENTMODE_IMMEDIATE);
		app_info("%016"PRIu64" SDL_WindowSupportsGPUPresentMode(..., IMMEDIATE): %s", SDL_GetTicksNS(), can_immediate?"true":"false");

		if (can_immediate) {
			if (!SDL_SetGPUSwapchainParameters(gpu_device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_IMMEDIATE)) {
				app_warn("%016"PRIu64" SDL_SetGPUSwapchainParameters(..., SDR, IMMEDIATE) failed", SDL_GetTicksNS());
			}
			present_mode = SDL_GPU_PRESENTMODE_IMMEDIATE;
		}
	}
	app_info("%016"PRIu64" Main window SDL_GPUPresentMode: %s", SDL_GetTicksNS(),
			(present_mode==SDL_GPU_PRESENTMODE_MAILBOX)?"MAILBOX":(
				(present_mode==SDL_GPU_PRESENTMODE_IMMEDIATE)?"IMMEDIATE":(
					(present_mode==SDL_GPU_PRESENTMODE_VSYNC)?"VSYNC":"UNKNOWN")));

	// Create a layered render target texture for stream outputs and preview, allowing alpha-blend in linear color-space
	const SDL_GPUTextureCreateInfo createinfo = {
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = SDL_GPU_TEXTUREFORMAT_R16G16B16A16_FLOAT,
		.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET,
		.width = 1920,
		.height = 1080,
		.layer_count_or_depth = 4,
		.num_levels = 1,
		.sample_count = SDL_GPU_SAMPLECOUNT_1,
		.props = 0
	};
	SDL_GPUTexture *render_targets = SDL_CreateGPUTexture(gpu_device, &createinfo);
	if (!render_targets) {
		app_failure("SDL_CreateGPUTexture() for render_targets: %s", SDL_GetError());
	}

	// Setup Dear ImGui context
	CIMGUI_CHECKVERSION(); // This macro calls ImGui::DebugCheckVersionAndDataLayout() and try to detect ABI problems
	ImGui_CreateContext(NULL);
	ImGuiContext* imgui_context = ImGui_GetCurrentContext();
	ImGuiIO *imgui_io = ImGui_GetIO();
	imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#ifdef IMGUI_HAS_DOCK
	imgui_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
#endif
#ifdef IMGUI_HAS_VIEWPORT
	imgui_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
#endif

	// Setup Dear ImGui style
	ImGui_StyleColorsDark(NULL);
	//ImGui_StyleColorsLight(NULL);

	// Setup scaling
	ImGuiStyle* style = ImGui_GetStyle();
	// Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	ImGuiStyle_ScaleAllSizes(style, main_scale);
	// Set initial font scale. (using imgui_io->ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
	style->FontScaleDpi = main_scale;
#ifdef IMGUI_HAS_VIEWPORT
	// [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	imgui_io->ConfigDpiScaleFonts = true;
	// [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
	imgui_io->ConfigDpiScaleViewports = true;
#endif
#ifdef IMGUI_HAS_VIEWPORT
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (imgui_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style->WindowRounding = 0.0f;
		style->Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
#endif

	// Setup Platform/Renderer backends
	cImGui_ImplSDL3_InitForSDLGPU(window);
	ImGui_ImplSDLGPU3_InitInfo init_info;
	SDL_memset(&init_info, 0, sizeof(init_info));
	init_info.Device = gpu_device;
	init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpu_device, window);
	init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;                      // Only used in multi-viewports mode.
	init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;  // Only used in multi-viewports mode.
	init_info.PresentMode = present_mode;
	cImGui_ImplSDLGPU3_Init(&init_info);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font.
	// You can also load multiple fonts and use ImGui_PushFont()/PopFont() to select them.
	// - AddFontFrom*TTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL.
	// Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details. If you like the default font but want it to scale better, consider using the 'ProggyVector' from the same author!
	// - In SDL use case, use SDL file API then:
	// ImFontAtlas_AddFontFromMemoryTTF(ImFontAtlas* self, void* font_data, int font_data_size, float size_pixels = 0.0f, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);
	// Note: Transfer ownership of 'ttf_data' to ImFontAtlas! Will be deleted after destruction of the atlas.
	// Set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed.
	//
	//style.FontSizeBase = 20.0f;
	//ImFontAtlas *atlas = ...
	//ImFontAtlas_AddFontDefault(atlas, NULL);
	//char *font_basepath = ...
	//char *font_path;
	//if (!SDL_asprintf(&font_path, "%s/%s", font_basepath)) { ... }
	//size_t font_data_size;
	//void *font_data = SDL_LoadFile(font_path, &font_data_size);
	//if ( !font_data ) { ... }
	//ImFont *font = ImFontAtlas_AddFontFromMemoryTTF(atlas, font_data, font_data_size, 0.0f, NULL, NULL)
	//if ( !font ) { ... }

	// SDL Async IO initialization
	SDL_AsyncIOQueue *sdl_io_queue = SDL_CreateAsyncIOQueue();

	// ECS initialization
	ecs_world_t *world = ecs_init();
	// Optional, gather statistics for explorer
	ECS_IMPORT(world, FlecsStats);
	// Creates REST server on default port (27750)
	ecs_singleton_set(world, EcsRest, {0});

	// appinternal_t initialisation
	internal->framerate.num = 60;
	internal->framerate.den = 1;

	// appstate_t initialisation
	appstate->running_app_version = APP_VERSION_INT;
	appstate->internal = internal;
	appstate->logpriority_earlyskip = logpriority_earlyskip;

	appstate->sdl_malloc_func = alloc_count_malloc;
	appstate->sdl_calloc_func = alloc_count_calloc;
	appstate->sdl_realloc_func = alloc_count_realloc;
	appstate->sdl_free_func = alloc_count_free;
	appstate->imgui_allocator_functions_user_data = NULL;

	appstate->imgui_malloc_func = alloc_count_malloc_userptr;
	appstate->imgui_free_func = alloc_count_free_userptr;

	appstate->tick0_wallclock = tick0_wallclock;
	appstate->frameid = 0;

	appstate->window = window;
	appstate->gpu_device = gpu_device;
	appstate->render_targets = render_targets;
	appstate->imgui_context = imgui_context;
	appstate->imgui_io = imgui_io;

	appstate->sdl_io_queue = sdl_io_queue;
	appstate->world = world;

	// Extra tasks
	app_dummy_demo_code_init(appstate);
	mod_load_all(appstate);

	// ECS First frame. This runs both the Startup and Update systems
	ecs_progress(world, 0.0f);

	// Memory allocation statistics
	alloc_count_dump_counters(appstate->frameid, "end of SDL_AppInit()");
	alloc_count_set_context(APP_CONTEXT_FIRST_FRAMES);

	//TODO tmp work around throttling
	internal->video_ts_origin = SDL_GetTicksNS();

	return SDL_APP_CONTINUE;
}

// ECS System
void LogDeltaTime(ecs_iter_t *it) {
	SDL_LogPriority logpriority_earlyskip = SDL_LOG_PRIORITY_TRACE; // TODO get it from ECS, ditto for frameid
	// Print delta_time. The same value is passed to all systems.
	app_trace("%016"PRIu64" ecs_iter_t it->delta_time: %f, it->delta_system_time: %f\n",
			SDL_GetTicksNS(), (double)it->delta_time, (double)it->delta_system_time);
}

void app_dummy_demo_code_init(appstate_t *appstate) {
	ecs_world_t *world = appstate->world;

	ecs_entity_t e = ecs_entity(world, { .name = "Bob" });
	app_info("%016"PRIu64" ECS world initialized, first entity name: %s\n", SDL_GetTicksNS(), ecs_get_name(world, e));

	// Create system that prints delta_time. This system doesn't query for any
	// components which means it won't match any entities, but will still be ran
	// once for each call to ecs_progress.
	ECS_SYSTEM(world, LogDeltaTime, EcsOnUpdate, 0);

	appstate->internal->show_demo_window = true;
	appstate->internal->show_another_window = false;
	appstate->internal->clear_color.x = 0.45f;
	appstate->internal->clear_color.y = 0.55f;
	appstate->internal->clear_color.z = 0.60f;
	appstate->internal->clear_color.w = 1.00f;

	// TODO enqueue some demo asset load here
	// (end of app-iterate handle SDL async IO events, to be drawn by ImGui then)
	// https://github.com/TheSpydog/SDL_gpu_examples/blob/main/Examples/TexturedQuad.c
	// https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
}

void SDL_AppQuit(void *_appstate, SDL_AppResult result) {
	appstate_t *appstate = (appstate_t *)_appstate;
	alloc_count_set_context(APP_CONTEXT_STARTUP_SHUTDOWN);

	SDL_WaitForGPUIdle(appstate->gpu_device);
	cImGui_ImplSDL3_Shutdown();
	cImGui_ImplSDLGPU3_Shutdown();
	ImGui_DestroyContext(NULL);

	SDL_ReleaseGPUTexture(appstate->gpu_device, appstate->render_targets);
	SDL_ReleaseWindowFromGPUDevice(appstate->gpu_device, appstate->window);
	SDL_DestroyGPUDevice(appstate->gpu_device);
	SDL_DestroyWindow(appstate->window);

	ecs_fini(appstate->world);
	SDL_free(appstate);

	alloc_count_dump_counters(appstate->frameid, "end of SDL_AppQuit()");
}
