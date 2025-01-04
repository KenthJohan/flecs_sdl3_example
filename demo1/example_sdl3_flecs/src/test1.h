#pragma once
#include <SDL3/SDL_gpu.h>
#include <flecs.h>
#include "EgGpu.h"

#define GPU_TRANSFER_MAX_CMDS 16

typedef struct {
	SDL_GPUBuffer *dst;
	uint32_t src_offset;
	uint32_t dst_offset;
	uint32_t size;
} gpu_transfer_cmd_t;

typedef struct {
	uint8_t *data;
	uint32_t cmd_last;
	gpu_transfer_cmd_t cmd[GPU_TRANSFER_MAX_CMDS];
} gpu_transfer_t;

void gpu_transfer_begin(gpu_transfer_t *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb);
void gpu_transfer_add(gpu_transfer_t *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb, const void *src, uint32_t src_size, SDL_GPUBuffer *dst, uint32_t dst_offset);
void gpu_transfer_end(gpu_transfer_t *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb);
void gpu_transfer_submit(gpu_transfer_t *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb);