#include "app.h"
#include "dcimgui_impl_sdl3.h"
#include "dcimgui_impl_sdlgpu3.h"
#include "alloc.h"
#include "ui.h"

SDL_AppResult SDL_AppIterate(void *_appstate) {
	appstate_t *appstate = (appstate_t *) _appstate;
	SDL_AppResult then = SDL_APP_CONTINUE;

	SDL_LogPriority logpriority_earlyskip = appstate->logpriority_earlyskip;
	SDL_Window *window = appstate->window;
	SDL_GPUDevice* gpu_device = appstate->gpu_device;
	ImGuiIO *imgui_io = appstate->imgui_io;
	Sint32 frameid = appstate->frameid;
	ImVec4 clear_color = appstate->clear_color;

	app_trace("%016"PRIu64" SDL_AppIterate(%d)", SDL_GetTicksNS(), frameid);

	if ( frameid == 5 ) alloc_count_set_context(APP_CONTEXT_RENDERING);

	if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
	{
		SDL_Delay(10);
		return then;
	}

	// Start the Dear ImGui frame
	cImGui_ImplSDLGPU3_NewFrame();
	cImGui_ImplSDL3_NewFrame();
	ImGui_NewFrame();

	// Prepare UI content (it produces command buffers, not pixels)
	then = ui_main(appstate);

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
	if (imgui_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui_UpdatePlatformWindows();
		ImGui_RenderPlatformWindowsDefault();
	}

	// Submit the command buffer
	SDL_SubmitGPUCommandBuffer(command_buffer);

	appstate->frameid++;

	return then;
}
