#pragma once
#include "mod-host.h"

typedef struct appinternal {
	appmods_t mods;	
	//TODO remove this dummy demo code, will be in the ECS
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;
	//...
} appinternal_t;
