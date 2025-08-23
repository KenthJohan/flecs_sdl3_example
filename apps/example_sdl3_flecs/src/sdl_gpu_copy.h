#pragma once

#include <SDL3/SDL_gpu.h>

int sdl_gpu_copy_simple1(SDL_GPUDevice *device, void const *data, size_t size, SDL_GPUBuffer *gpubuf);