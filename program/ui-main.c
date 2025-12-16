#include "ui.h"
#include "app-internal.h"

SDL_AppResult ui_main(appstate_t *appstate) {
	SDL_AppResult then = SDL_APP_CONTINUE;

	// 1. Show the big demo window (Most of the sample code is in ImGui_ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (appstate->internal->show_demo_window)
		ImGui_ShowDemoWindow(&appstate->internal->show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui_Begin("Hello, world!", NULL, 0);                 // Create a window called "Hello, world!" and append into it.

		ImGui_Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui_Checkbox("Demo Window", &appstate->internal->show_demo_window);      // Edit bools storing our window open/close state
		ImGui_Checkbox("mod-template Window", &appstate->internal->show_another_window);

		ImGui_SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui_ColorEdit3("clear color", (float*)&appstate->internal->clear_color, 0); // Edit 3 floats representing a color

		if (ImGui_Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui_SameLine();
		ImGui_Text("counter = %d", counter);

		ImGui_Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / appstate->imgui_io->Framerate, appstate->imgui_io->Framerate);
		ImGui_End();
	}

	//TODO hooks that return persistently a non MOD_RESULT_CONTINUE should be noticed,
	// but if logged, will flood the logs at 60 lines per second, prevent it
	// by using counters + last frameid of each status instead
	appmods_t *mods = &appstate->internal->mods;
	if ( appstate->internal->show_another_window ) {
		Sint32 count = mods->hook_ui_config_v1_count;
		for (int j=0; j<count; j++) {
			void *userptr = mods->userptr[j];
			mod_result_t res = mods->hook_ui_config_v1[j](userptr);
			if ( res == MOD_RESULT_SUCCESS ) {
				appstate->internal->show_another_window = false;
			}
		}
	}

	return then;
}
