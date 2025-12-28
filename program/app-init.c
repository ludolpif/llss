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
#include "alloc.h"
#include "app.h"
#include "app-internal.h"
#include "dcimgui_impl_sdl3.h"
#include "dcimgui_impl_sdlgpu3.h"

SDL_DECLSPEC SDL_LogPriority logpriority_earlyskip;
#define app_failure(...) do { SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__); return SDL_APP_FAILURE; } while(SDL_NULL_WHILE_LOOP_CONDITION)

#define APP_UI_DEFAULT_WIDTH 1280
#define APP_UI_DEFAULT_HEIGHT 720

//typedef void(* ecs_os_api_log_t) (int32_t level, const char *file, int32_t line, const char *msg)
void flecs_to_sdl_log_adapter(int32_t level, const char *file, int32_t line, const char *msg) {
	/* The level should be interpreted as:
	 * >0: Debug tracing. Only enabled in debug builds.
	 *  0: Tracing. Enabled in debug/release builds.
	 * -2: Warning. An issue occurred, but operation was successful.
	 * -3: Error. An issue occurred, and operation was unsuccessful.
	 * -4: Fatal. An issue occurred, and application must quit.
	 */
#define FLECS_LOG_VAARGS "%016"PRIu64" %s:%"PRIi32" %s", SDL_GetTicksNS(), file, line, msg
	switch (level) {
		case -4: app_critical(FLECS_LOG_VAARGS); break;
		case -3: app_error(FLECS_LOG_VAARGS); break;
		case -2: app_warn(FLECS_LOG_VAARGS); break;
		case  0: app_verbose(FLECS_LOG_VAARGS); break;
		default: app_trace(FLECS_LOG_VAARGS); break;
	}
#undef FLECS_LOG_VAARGS
}

// Implementations
SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
	// Configure memory functions before the first dynamic allocation
	alloc_count_install_hooks();

	// Configure logging
	logpriority_earlyskip = SDL_GetLogPriority(SDL_LOG_CATEGORY_APPLICATION);
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_TRACE,    "TRACE ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_VERBOSE,  "VERB  ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_DEBUG,    "DEBUG ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INFO,     "INFO  ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_WARN,     "WARN  ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_ERROR,    "ERROR ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_CRITICAL, "CRIT  ");

	// Early log message to help troubleshoot application init and allow human readable timestamps later conversion
	// Note SDL_Ticks should be a CLOCK_MONOTONIC source, but some platforms may not provide it
	// SDL_Time is real time, subject to system clock adjustment
	SDL_Time tick0_wallclock = 0;
	(void) SDL_GetCurrentTime(&tick0_wallclock);

	Uint64 tick = SDL_GetTicksNS();
	tick0_wallclock -= tick;
	app_warn("Starting %s %s with log priority %d, tick0_wallclock==%"PRId64,
			APP_METADATA_NAME_STRING, APP_VERSION_STR, logpriority_earlyskip, tick0_wallclock);

	// Set metadata before SDL_Init because it will print it if loglevel is high enough
	// We don't check return value, we don't want to abort the app startup if this fails anyway.
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, APP_METADATA_NAME_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, APP_VERSION_STR);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, APP_METADATA_IDENTIFIER_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, APP_METADATA_CREATOR_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, APP_METADATA_COPYRIGHT_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, APP_METADATA_URL_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, APP_METADATA_TYPE_STRING);

	if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_CAMERA))
		app_failure("SDL_Init(): %s", SDL_GetError());

	// Create SDL main_window graphics context
	float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	SDL_Window* main_window = SDL_CreateWindow(APP_METADATA_NAME_STRING,
			(int)(APP_UI_DEFAULT_WIDTH * main_scale),
			(int)(APP_UI_DEFAULT_HEIGHT * main_scale),
			window_flags);
	if (main_window == NULL)
		app_failure("SDL_CreateWindow(): %s", SDL_GetError());
	SDL_SetWindowPosition(main_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	// Assume this app will run on a single GPU and need to be restarted to change it
	bool gpu_device_debug_mode = true;
	SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(
			SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB,
			gpu_device_debug_mode, NULL);
	if (gpu_device == NULL)
		app_failure("SDL_CreateGPUDevice(): %s", SDL_GetError());

	// Init other SDL features that aren't foreseen to may be multi-instantiated one day
	SDL_AsyncIOQueue *sdl_io_queue = SDL_CreateAsyncIOQueue();

	// Claim main_window for GPU Device
	if (!SDL_ClaimWindowForGPUDevice(gpu_device, main_window))
		app_failure("SDL_ClaimWindowForGPUDevice(): %s", SDL_GetError());

	// Try to apply a new SDL_GPUPresentMode, respecting this preference order : MAILBOX > IMMEDIATE > VSYNC
	// VSYNC mode will always be supported. IMMEDIATE and MAILBOX modes may not be supported on certain systems.
	// IMMEDIATE: Immediately presents. Lowest latency option, but tearing may occur.
	// MAILBOX: Waits for vblank before presenting. No tearing is possible.
	// If there is a pending image to present, the pending image is replaced by the new image.
	// Similar to VSYNC, but with reduced visual latency.

	SDL_GPUPresentMode present_mode = SDL_GPU_PRESENTMODE_VSYNC;
	// API doc says: The swapchain will be created with SDL_GPU_SWAPCHAINCOMPOSITION_SDR and SDL_GPU_PRESENTMODE_VSYNC.

	bool can_mailbox = SDL_WindowSupportsGPUPresentMode(gpu_device, main_window, SDL_GPU_PRESENTMODE_MAILBOX);
	app_info("%016"PRIu64" SDL_WindowSupportsGPUPresentMode(..., MAILBOX): %s", SDL_GetTicksNS(), can_mailbox?"true":"false");

	if ( present_mode != SDL_GPU_PRESENTMODE_MAILBOX ) {
		// Switch to MAILBOX mode if avaiable
		if (can_mailbox) {
			if (!SDL_SetGPUSwapchainParameters(gpu_device, main_window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_MAILBOX)) {
				app_warn("%016"PRIu64" SDL_SetGPUSwapchainParameters(..., SDR, MAILBOX) failed", SDL_GetTicksNS());
			}
			present_mode = SDL_GPU_PRESENTMODE_MAILBOX;
		}
	}
	if ( present_mode != SDL_GPU_PRESENTMODE_MAILBOX ) {
		// Fallback to IMMEDIATE mode if avaiable
		bool can_immediate = SDL_WindowSupportsGPUPresentMode(gpu_device, main_window, SDL_GPU_PRESENTMODE_IMMEDIATE);
		app_info("%016"PRIu64" SDL_WindowSupportsGPUPresentMode(..., IMMEDIATE): %s", SDL_GetTicksNS(), can_immediate?"true":"false");

		if (can_immediate) {
			if (!SDL_SetGPUSwapchainParameters(gpu_device, main_window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_IMMEDIATE)) {
				app_warn("%016"PRIu64" SDL_SetGPUSwapchainParameters(..., SDR, IMMEDIATE) failed", SDL_GetTicksNS());
			}
			present_mode = SDL_GPU_PRESENTMODE_IMMEDIATE;
		}
	}
	app_info("%016"PRIu64" SDL_GPUPresentMode for main window: %s", SDL_GetTicksNS(),
			(present_mode==SDL_GPU_PRESENTMODE_MAILBOX)?"MAILBOX":(
				(present_mode==SDL_GPU_PRESENTMODE_IMMEDIATE)?"IMMEDIATE":(
					(present_mode==SDL_GPU_PRESENTMODE_VSYNC)?"VSYNC":"UNKNOWN")));

	// Setup Dear ImGui context
	CIMGUI_CHECKVERSION(); // This macro calls ImGui::DebugCheckVersionAndDataLayout() and try to detect ABI problems
	ImGui_CreateContext(NULL);
	ImGuiContext* imgui_context = ImGui_GetCurrentContext();
	ImGuiIO *imgui_io = ImGui_GetIO();
	imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	// imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //TODO need more code in app-events.c

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
	cImGui_ImplSDL3_InitForSDLGPU(main_window);
	ImGui_ImplSDLGPU3_InitInfo init_info = {
		.Device = gpu_device,
		.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpu_device, main_window),
		.MSAASamples = SDL_GPU_SAMPLECOUNT_1,                      // Only used in multi-viewports mode.
		.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR,  // Only used in multi-viewports mode.
		.PresentMode = present_mode
		};
	cImGui_ImplSDLGPU3_Init(&init_info);

	//TODO load from config
	Sint32 fr_num = 60;
	Sint32 fr_den = 1;
	int32_t ecs_worker_threads_count = 4;

	// ECS early minimal init
	ecs_os_set_api_defaults();
	ecs_os_api_t os_api = ecs_os_get_api();
	os_api.malloc_  = alloc_count_malloc_ecs;
	os_api.free_    = alloc_count_free_ecs;
	os_api.realloc_ = alloc_count_realloc_ecs;
	os_api.log_     = flecs_to_sdl_log_adapter;
	//os_api.strdup_  = SDL_strdup; // FIXME already wired to alloc_count_malloc_ecs but crashes app
	os_api.now_     = SDL_GetTicksNS; // Hopefully less confusing to use the same everywhere
	ecs_os_set_api(&os_api);

	ecs_world_t *world = ecs_init();
	ecs_set_threads(world, ecs_worker_threads_count);
	ECS_IMPORT(world, FlecsStats); // Optional, gather statistics for explorer

	ecs_log_set_level(0); // Increase verbosity level
	ecs_singleton_set(world, EcsRest, {0}); // Creates REST server on default port (27750)

	ECS_IMPORT(world, Module1); // Will call Module1Import(world) from ecs-module1.c
	ecs_singleton_set(world, AppVersion, {
			.running_app_version = APP_VERSION_INT
			});
	ecs_singleton_set(world, AppMemoryFuncs, {
		.sdl_malloc_func = alloc_count_malloc,
		.sdl_calloc_func = alloc_count_calloc,
		.sdl_realloc_func = alloc_count_realloc,
		.sdl_free_func = alloc_count_free,
		.imgui_malloc_func = alloc_count_malloc_userptr,
		.imgui_free_func = alloc_count_free_userptr,
		.imgui_allocator_functions_user_data = NULL,
		}),
	ecs_singleton_set(world, AppSDLContext, {
		.main_window = main_window,
		.gpu_device = gpu_device,
		.sdl_io_queue = sdl_io_queue,
		}),
	ecs_singleton_set(world, AppImGuiContext, {
		.imgui_context = imgui_context,
		.imgui_io = imgui_io,
		});
	ecs_singleton_set(world, AppMainTimingContext, {
		.app_iterate_count = 0,
		.total_skipped = 0,
		.main_framerate_num = fr_num,
		.main_framerate_den = fr_den,
		.main_frame_start_ns = 0,
		.main_frameid = 0,
		});

	// ECS First frame. This runs both the Startup, Update and user-defined systems
	ecs_progress(world, 0.0f);

	// Show window now we have a valid first image to display
	SDL_ShowWindow(main_window);

	// Memory allocation statistics
	alloc_count_dump_counters(0, "end of SDL_AppInit()");
	alloc_count_set_context(APP_CONTEXT_FIRST_FRAMES);

	// SDL main give us the opportunity to have a *userdata-like pointer for futher callbacks
	*appstate = world;

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
	ecs_world_t *world = (ecs_world_t *)appstate;
	alloc_count_set_context(APP_CONTEXT_STARTUP_SHUTDOWN);

	const AppSDLContext *app_sdl_context = ecs_singleton_get(world, AppSDLContext);
	const AppMainTimingContext *app_main_timing_context = ecs_singleton_get(world, AppMainTimingContext);
	Uint32 app_iterate_count = app_main_timing_context->app_iterate_count;
	
	SDL_WaitForGPUIdle(app_sdl_context->gpu_device);
	cImGui_ImplSDL3_Shutdown();
	cImGui_ImplSDLGPU3_Shutdown();
	ImGui_DestroyContext(NULL);

	SDL_ReleaseWindowFromGPUDevice(app_sdl_context->gpu_device, app_sdl_context->main_window);
	SDL_DestroyGPUDevice(app_sdl_context->gpu_device);
	SDL_DestroyWindow(app_sdl_context->main_window);

	ecs_log_set_level(-1);
	ecs_fini(world);

	alloc_count_dump_counters(app_iterate_count, "end of SDL_AppQuit()");
}
