// Dear ImGui: standalone example application for SDL3 + SDL_GPU
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "dcimgui.h"
#include "dcimgui_impl_sdl3.h"
#include "dcimgui_impl_sdlgpu3.h"
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>

typedef struct appstate {
	SDL_Window *window;
	SDL_GPUDevice *gpu_device;
	ImGuiIO *io;
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;
} appstate_t;

SDL_AppResult SDL_AppInit(void **_appstate, int argc, char **argv) {
	*_appstate = calloc(1,sizeof(appstate_t));
	if (!*_appstate)
	{
		printf("Error: malloc(sizeof(appstate_t))\n");
		return SDL_APP_FAILURE;
	}
	appstate_t *appstate = *_appstate;

	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
	{
		printf("Error: SDL_Init(): %s\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	// Create SDL window graphics context
	float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	SDL_Window* window = SDL_CreateWindow("LLSS Mockup", (int)(1280 * main_scale), (int)(800 * main_scale), window_flags);
	if (window == NULL)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window);

	// Create GPU Device
	SDL_GPUDevice* gpu_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB,true,NULL);
	if (gpu_device == NULL)
	{
		printf("Error: SDL_CreateGPUDevice(): %s\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	// Claim window for GPU Device
	if (!SDL_ClaimWindowForGPUDevice(gpu_device, window))
	{
		printf("Error: SDL_ClaimWindowForGPUDevice(): %s\n", SDL_GetError());
		return SDL_APP_FAILURE;
	}
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

	appstate->window = window;
	appstate->gpu_device = gpu_device;
	appstate->io = io;
	appstate->show_demo_window = true;
	appstate->show_another_window = false;
	appstate->clear_color.x = 0.45f;
	appstate->clear_color.y = 0.55f;
	appstate->clear_color.z = 0.60f;
	appstate->clear_color.w = 1.00f;

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *_appstate) {
	appstate_t *appstate = (appstate_t *) _appstate;
	SDL_AppResult then = SDL_APP_CONTINUE;

	SDL_Window *window = ((appstate_t *)appstate)->window;
	SDL_GPUDevice* gpu_device = ((appstate_t *)appstate)->gpu_device;
	ImGuiIO *io = ((appstate_t *)appstate)->io;
	bool show_demo_window = ((appstate_t *)appstate)->show_demo_window;
	bool show_another_window = ((appstate_t *)appstate)->show_another_window;
	ImVec4 clear_color = ((appstate_t *)appstate)->clear_color;

	if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
	{
		SDL_Delay(10);
		return then;
	}

	// Start the Dear ImGui frame
	cImGui_ImplSDLGPU3_NewFrame();
	cImGui_ImplSDL3_NewFrame();
	ImGui_NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui_ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui_ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui_Begin("Hello, world!", NULL, 0);                 // Create a window called "Hello, world!" and append into it.

		ImGui_Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui_Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui_Checkbox("Another Window", &show_another_window);

		ImGui_SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui_ColorEdit3("clear color", (float*)&clear_color, 0); // Edit 3 floats representing a color

		if (ImGui_Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui_SameLine();
		ImGui_Text("counter = %d", counter);

		ImGui_Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
		ImGui_End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui_Begin("Another Window", &show_another_window, 0);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui_Text("Hello from another window!");
		if (ImGui_Button("Close Me"))
			show_another_window = false;
		ImGui_End();
	}

	// Rendering
	ImGui_Render();
	ImDrawData* draw_data = ImGui_GetDrawData();
	const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

	SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(gpu_device); // Acquire a GPU command buffer

	SDL_GPUTexture* swapchain_texture;
	SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, window, &swapchain_texture, NULL, NULL); // Acquire a swapchain texture

	if (swapchain_texture != NULL && !is_minimized)
	{
		// This is mandatory: call ImGui_ImplSDLGPU3_PrepareDrawData() to upload the vertex/index buffer!
		cImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

		// Setup and start a render pass
		SDL_GPUColorTargetInfo target_info = {};
		target_info.texture = swapchain_texture;
		SDL_FColor sdl_clear_color = { clear_color.x, clear_color.y, clear_color.z, clear_color.w };
		target_info.clear_color = sdl_clear_color;
		target_info.load_op = SDL_GPU_LOADOP_CLEAR;
		target_info.store_op = SDL_GPU_STOREOP_STORE;
		target_info.mip_level = 0;
		target_info.layer_or_depth_plane = 0;
		target_info.cycle = false;
		SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(command_buffer, &target_info, 1, NULL);

		// Render ImGui
		cImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer, render_pass);

		SDL_EndGPURenderPass(render_pass);
	}

	// Update and Render additional Platform Windows
	if (io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui_UpdatePlatformWindows();
		ImGui_RenderPlatformWindowsDefault();
	}

	// Submit the command buffer
	SDL_SubmitGPUCommandBuffer(command_buffer);

	return then;
}

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

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
	SDL_Window *window = ((appstate_t *)appstate)->window;
	SDL_GPUDevice* gpu_device = ((appstate_t *)appstate)->gpu_device;

	SDL_WaitForGPUIdle(gpu_device);
	cImGui_ImplSDL3_Shutdown();
	cImGui_ImplSDLGPU3_Shutdown();
	ImGui_DestroyContext(NULL);

	SDL_ReleaseWindowFromGPUDevice(gpu_device, window);
	SDL_DestroyGPUDevice(gpu_device);
	SDL_DestroyWindow(window);
}
