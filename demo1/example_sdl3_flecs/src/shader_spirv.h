#pragma once

#include <SDL3/SDL_gpu.h>

SDL_GPUShader * shader_spirv_compile(SDL_GPUDevice *device, const char *namepart, SDL_GPUShaderStage stage);