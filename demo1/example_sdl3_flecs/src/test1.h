#pragma once
#include <SDL3/SDL_gpu.h>
#include <flecs.h>
#include "EgGpu.h"

#define GPU_TRANSFER_MAX_CMDS 16




void gpu_transfer_begin(EgGpuTransfer *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb);
void gpu_transfer_add(EgGpuTransfer *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb, const void *src, uint32_t src_size, SDL_GPUBuffer *dst, uint32_t dst_offset);
void gpu_transfer_end(EgGpuTransfer *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb);
void gpu_transfer_submit(EgGpuTransfer *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb);