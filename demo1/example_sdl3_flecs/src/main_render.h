#pragma once

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_video.h>

void main_render(
	SDL_Window * window, 
	SDL_GPUDevice *gpu_device, 
	SDL_GPUGraphicsPipeline *pipeline, 
	SDL_GPUBuffer *buf_vertex,
	SDL_GPUTexture *tex_depth,
	float const * mvp
	);