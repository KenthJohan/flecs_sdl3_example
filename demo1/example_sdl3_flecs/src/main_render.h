#pragma once
#include "main_types.h"
#include "SDL_test_common.h"

void main_render(SDLTest_CommonState *state, SDL_Window ** windows, SDL_GPUDevice *gpu_device, WindowState * window_states, const int windownum, SDL_GPUGraphicsPipeline *pipeline, SDL_GPUBuffer *buf_vertex);


SDL_GPUTexture * CreateDepthTexture(SDL_GPUSampleCount sample_count, SDLTest_CommonState *state, SDL_GPUDevice *gpu_device, Uint32 drawablew, Uint32 drawableh);

SDL_GPUTexture * CreateMSAATexture(SDL_GPUSampleCount sample_count, SDLTest_CommonState *state, SDL_GPUDevice *gpu_device, Uint32 drawablew, Uint32 drawableh);

SDL_GPUTexture * CreateResolveTexture(SDL_GPUSampleCount sample_count, SDLTest_CommonState *state, SDL_GPUDevice *gpu_device, Uint32 drawablew, Uint32 drawableh);