#pragma once

#include <SDL3/SDL_gpu.h>
#include "SDL_test_common.h"

typedef struct RenderState {
	SDL_GPUSampleCount sample_count;
} RenderState;

typedef struct WindowState {
	int angle_x, angle_y, angle_z;
	SDL_GPUTexture *tex_depth, *tex_msaa, *tex_resolve;
	Uint32 prev_drawablew, prev_drawableh;
} WindowState;


