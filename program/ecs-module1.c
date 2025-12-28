#include "app.h" // No "ecs-module1.h", embeded in app.h
#include "ui-main.h"
#include "dcimgui_impl_sdl3.h"
#include "dcimgui_impl_sdlgpu3.h"

// Phases for pipelines
ecs_entity_t RenderingPreImGui, RenderingOnImGui, RenderingPostImGui;

ECS_COMPONENT_DECLARE(AppState);

// ECS Systems forward declarations
void ImGuiPrepareForNewFrame(ecs_iter_t *it);
void ImGuiRenderAndSubmit(ecs_iter_t *it);

void Module1Import(ecs_world_t *world) {
	// https://www.flecs.dev/flecs/md_docs_2EntitiesComponents.html#registration
	// See the "modules" example
	ECS_MODULE(world, Module1);

	// Phases for pipelines
	ecs_entity_t RenderingPreImGui = ecs_entity_init(world, &(ecs_entity_desc_t){
			.name = "RenderingPreImGui",
			.add = ecs_ids(EcsPhase)
			});
	ecs_add_pair(world, RenderingPreImGui, EcsDependsOn, EcsPostUpdate);

	ecs_entity_t RenderingOnImGui = ecs_entity_init(world, &(ecs_entity_desc_t){
			.name = "RenderingOnImGui",
			.add = ecs_ids(EcsPhase)
			});
	ecs_add_pair(world, RenderingOnImGui, EcsDependsOn, RenderingPreImGui);

	ecs_entity_t RenderingPostImGui = ecs_entity_init(world, &(ecs_entity_desc_t){
			.name = "RenderingPostImGui",
			.add = ecs_ids(EcsPhase)
			});
	ecs_add_pair(world, RenderingPostImGui, EcsDependsOn, RenderingOnImGui);

	ECS_COMPONENT_DEFINE(world, AppState);

	ECS_SYSTEM(world, ImGuiPrepareForNewFrame, RenderingPreImGui, [in] AppState);
	ECS_SYSTEM(world, UIMain, RenderingOnImGui, [in] AppState);
	ECS_SYSTEM(world, ImGuiRenderAndSubmit, RenderingPostImGui, [in] AppState);
}

void ImGuiPrepareForNewFrame(ecs_iter_t *it) {
	cImGui_ImplSDLGPU3_NewFrame();
	cImGui_ImplSDL3_NewFrame();
	ImGui_NewFrame();
}

void ImGuiRenderAndSubmit(ecs_iter_t *it) {
	// Get fields from system query
	AppState *_appstate = ecs_field(it, AppState, 0);
	appstate_t *appstate = _appstate[0].appstate;
	SDL_GPUDevice *gpu_device = appstate->gpu_device;
	SDL_Window *main_window = appstate->main_window;
	SDL_FColor sdl_clear_color = { 0.7, 0.7, 0.7, 1.0 };
	ImGuiIO *imgui_io = appstate->imgui_io;

	ImGui_Render();
	ImDrawData* draw_data = ImGui_GetDrawData();
	const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

	SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(gpu_device); // Acquire a GPU command buffer

	SDL_GPUTexture* swapchain_texture;
	SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, main_window, &swapchain_texture, NULL, NULL); // Acquire a swapchain texture

	if (swapchain_texture != NULL && !is_minimized) {
		// This is mandatory: call ImGui_ImplSDLGPU3_PrepareDrawData() to upload the vertex/index buffer!
		cImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

		// Setup and start a render pass
		SDL_GPUColorTargetInfo target_info = {};
		target_info.texture = swapchain_texture;
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
	if (imgui_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		ImGui_UpdatePlatformWindows();
		ImGui_RenderPlatformWindowsDefault();
	}

	SDL_SubmitGPUCommandBuffer(command_buffer);
}

