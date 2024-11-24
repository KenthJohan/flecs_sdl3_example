#pragma once
#include "main_types.h"
#include "SDL_test_common.h"

void main_render(
	SDL_Window * window, 
	SDL_GPUDevice *gpu_device, 
	WindowState * window_states, 
	const int windownum, 
	SDL_GPUGraphicsPipeline *pipeline, 
	SDL_GPUBuffer *buf_vertex,
	SDL_GPUTexture *tex_depth
	);


SDL_GPUTexture * CreateDepthTexture(SDL_GPUSampleCount sample_count, SDLTest_CommonState *state, SDL_GPUDevice *gpu_device, Uint32 drawablew, Uint32 drawableh);

