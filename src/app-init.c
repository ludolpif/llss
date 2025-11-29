#include "app.h"
#include "dcimgui_impl_sdl3.h"
#include "dcimgui_impl_sdlgpu3.h"
#include "alloc.h"
#include "mod-host.h"

#define app_failure(...) do { SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__); return SDL_APP_FAILURE; } while(SDL_NULL_WHILE_LOOP_CONDITION)

SDL_AppResult SDL_AppInit(void **_appstate, int argc, char **argv) {
	// Configure memory functions before the first dynamic allocation
	alloc_count_install_hooks();

	// Allocate on heap the main appstate
	*_appstate = SDL_calloc(1,sizeof(appstate_t));
	if (!*_appstate)
		app_failure("SDL_calloc(1,sizeof(appstate_t))");
	appstate_t *appstate = *_appstate;
	appstate->mods = SDL_calloc(1,sizeof(appmods_t));
	if (!appstate->mods)
		app_failure("SDL_calloc(1,sizeof(appmods_t))");

	// Configure logging
	SDL_LogPriority pri = SDL_GetLogPriority(SDL_LOG_CATEGORY_APPLICATION);
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_TRACE,    "TRACE ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_VERBOSE,  "VERB  ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_DEBUG,    "DEBUG ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_INFO,     "INFO  ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_WARN,     "WARN  ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_ERROR,    "ERROR ");
	SDL_SetLogPriorityPrefix(SDL_LOG_PRIORITY_CRITICAL, "CRIT  ");
	bool skip_debug = ( pri > SDL_LOG_PRIORITY_DEBUG );

	// Set metadata before SDL_Init because it will print it if loglevel is high enough
	// We don't check return value, we don't want to abort the app startup if this fails anyway.
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, APP_METADATA_NAME_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, APP_METADATA_VERSION_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, APP_METADATA_IDENTIFIER_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, APP_METADATA_CREATOR_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, APP_METADATA_COPYRIGHT_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, APP_METADATA_URL_STRING);
	(void) SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, APP_METADATA_TYPE_STRING);

	// Early first log message to help troubleshoot application init and allow human readable timestamps later conversion
	// Note SDL_Ticks should be a CLOCK_MONOTIC source, but some platforms may not provide it
	// SDL_Time is real time, subject to system clock adjustment
	SDL_Time tick0_wallclock;
	if (!SDL_GetCurrentTime(&tick0_wallclock))
		app_failure("SDL_GetCurrentTime(): %s", SDL_GetError());

	Uint64 tick = SDL_GetTicksNS();
	tick0_wallclock -= tick;
	app_info("%016lu SDL_AppInit(): tick0_wallclock==%ld", tick, tick0_wallclock);

	if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_CAMERA))
		app_failure("SDL_Init(): %s", SDL_GetError());

	// Create SDL window graphics context
	float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	SDL_Window* window = SDL_CreateWindow("LLSS Mockup", (int)(1280 * main_scale), (int)(800 * main_scale), window_flags);
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
	SDL_SetGPUSwapchainParameters(gpu_device, window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, SDL_GPU_PRESENTMODE_VSYNC);

	// Setup Dear ImGui context
	CIMGUI_CHECKVERSION();
	ImGui_CreateContext(NULL);
	ImGuiIO *io = ImGui_GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	// Setup Dear ImGui style
	ImGui_StyleColorsDark(NULL);
	//ImGui_StyleColorsLight(NULL);

	// Setup scaling
	ImGuiStyle* style = ImGui_GetStyle();
	ImGuiStyle_ScaleAllSizes(style, main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style->FontScaleDpi = main_scale;        // Set initial font scale. (using io->ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
	io->ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
	io->ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style->WindowRounding = 0.0f;
		style->Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	cImGui_ImplSDL3_InitForSDLGPU(window);
	ImGui_ImplSDLGPU3_InitInfo init_info = {};
	init_info.Device = gpu_device;
	init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(gpu_device, window);
	init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;                      // Only used in multi-viewports mode.
	init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;  // Only used in multi-viewports mode.
	init_info.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
	cImGui_ImplSDLGPU3_Init(&init_info);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui_PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
	// - Read 'docs/FONTS.md' for more instructions and details. If you like the default font but want it to scale better, consider using the 'ProggyVector' from the same author!
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//style.FontSizeBase = 20.0f;
	//io->Fonts->AddFontDefault();
	//io->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
	//io->Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
	//io->Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
	//io->Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
	//ImFont* font = io->Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
	//IM_ASSERT(font != NULL);

	ecs_world_t *world = ecs_init();
	ecs_entity_t e = ecs_entity(world, { .name = "Bob" });
	app_info("%016lu ECS world initialized, first entity name: %s\n", SDL_GetTicksNS(), ecs_get_name(world, e));
	//TODO this is dummy code for first sanity checks

	appstate->world = world;
	appstate->tick0_wallclock = tick0_wallclock;
	appstate->skip_debug = skip_debug;
	appstate->window = window;
	appstate->gpu_device = gpu_device;
	appstate->io = io;
	appstate->show_demo_window = true;
	appstate->show_another_window = false;
	appstate->frame_count = -1;
	appstate->clear_color.x = 0.45f;
	appstate->clear_color.y = 0.55f;
	appstate->clear_color.z = 0.60f;
	appstate->clear_color.w = 1.00f;

	app_info("%016lu heap allocation at end of SDL_AppInit:", SDL_GetTicksNS());
	alloc_count_dump_counters();
	alloc_count_set_context(APP_CONTEXT_FIRST_FRAMES);

	return mod_host_init(appstate);
}

void SDL_AppQuit(void *_appstate, SDL_AppResult result) {
	appstate_t *appstate = (appstate_t *)_appstate;
	alloc_count_set_context(APP_CONTEXT_STARTUP_SHUTDOWN);

	SDL_WaitForGPUIdle(appstate->gpu_device);
	cImGui_ImplSDL3_Shutdown();
	cImGui_ImplSDLGPU3_Shutdown();
	ImGui_DestroyContext(NULL);

	SDL_ReleaseWindowFromGPUDevice(appstate->gpu_device, appstate->window);
	SDL_DestroyGPUDevice(appstate->gpu_device);
	SDL_DestroyWindow(appstate->window);

	ecs_fini(appstate->world);
	SDL_free(appstate);

	app_info("%016lu heap allocation at end of SDL_AppQuit:", SDL_GetTicksNS());
	alloc_count_dump_counters();
}
