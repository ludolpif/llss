#include "ui.h"

SDL_AppResult UI_Main(appstate_t *appstate) {
	SDL_AppResult then = SDL_APP_CONTINUE;

	// 1. Show the big demo window (Most of the sample code is in ImGui_ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (appstate->show_demo_window)
		ImGui_ShowDemoWindow(&appstate->show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui_Begin("Hello, world!", NULL, 0);                 // Create a window called "Hello, world!" and append into it.

		ImGui_Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui_Checkbox("Demo Window", &appstate->show_demo_window);      // Edit bools storing our window open/close state
		ImGui_Checkbox("Another Window", &appstate->show_another_window);

		ImGui_SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui_ColorEdit3("clear color", (float*)&appstate->clear_color, 0); // Edit 3 floats representing a color

		if (ImGui_Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui_SameLine();
		ImGui_Text("counter = %d", counter);

		ImGui_Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / appstate->io->Framerate, appstate->io->Framerate);
		ImGui_End();
	}

	// 3. Show another simple window.
	if (appstate->show_another_window)
	{
		ImGui_Begin("Another Window", &appstate->show_another_window, 0);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui_Text("Hello from another window!");
		if (ImGui_Button("Close Me"))
			appstate->show_another_window = false;
		ImGui_End();
	}

	return then;
}
