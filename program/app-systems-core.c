#include "app-systems-core.h"
#include "ui-main.h"
#include "dcimgui_impl_sdl3.h"
#include "dcimgui_impl_sdlgpu3.h"

void AppSystemsCoreImport(ecs_world_t *world) {
    // ECS_IMPORT Will call AppComponentsCoreImport(world) from app-components-core.c
    ECS_IMPORT(world, AppComponentsCore);

    ECS_MODULE(world, AppSystemsCore);

    // Tasks declarations, will run once per frame
    ECS_SYSTEM(world, ImGuiPrepareForNewFrame, RenderingPreImGui, 0);
    ECS_SYSTEM(world, UIMain, RenderingOnImGui, 0);
    ECS_SYSTEM(world, ImGuiRenderAndSubmit, RenderingPostImGui, 0);
}

void ImGuiPrepareForNewFrame(ecs_iter_t *it) {
    cImGui_ImplSDLGPU3_NewFrame();
    cImGui_ImplSDL3_NewFrame();
    ImGui_NewFrame();
}

void ImGuiRenderAndSubmit(ecs_iter_t *it) {
    const AppSDLContext *app_sdl_context = ecs_singleton_get(it->world, AppSDLContext);
    const AppImGuiContext *app_imgui_context = ecs_singleton_get(it->world, AppImGuiContext);

    SDL_GPUDevice *gpu_device = app_sdl_context->gpu_device;
    SDL_Window *main_window = app_sdl_context->main_window;
    SDL_FColor sdl_clear_color = { 0.4f, 0.4f, 0.4f, 1.0f };
    ImGuiIO *imgui_io = app_imgui_context->imgui_io;

    ImGui_Render();
    ImDrawData* draw_data = ImGui_GetDrawData();
    const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

    // Acquire a GPU command buffer
    SDL_GPUCommandBuffer* command_buffer = SDL_AcquireGPUCommandBuffer(gpu_device);

    // Acquire a target texture
    SDL_GPUTexture* swapchain_texture;
    SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, main_window, &swapchain_texture, NULL, NULL);

    if (swapchain_texture != NULL && !is_minimized) {
        // Mandatory ImGui_ImplSDLGPU3_PrepareDrawData() to upload the vertex/index buffer!
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
