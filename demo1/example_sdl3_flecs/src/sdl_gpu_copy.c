#include "sdl_gpu_copy.h"

#include <flecs.h>

int sdl_gpu_copy_simple1(SDL_GPUDevice *device, void const *data, size_t size, SDL_GPUBuffer *gpubuf)
{
	ecs_assert(device != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(gpubuf != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(data != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(size != 0, ECS_INVALID_PARAMETER, NULL);

	SDL_GPUTransferBufferCreateInfo desc = {0};
	desc.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
	desc.size = size;
	desc.props = 0;

	SDL_GPUTransferBuffer *transfer = SDL_CreateGPUTransferBuffer(device, &desc);
	if (transfer == NULL) {
		ecs_err("Failed to create transfer buffer: %s", SDL_GetError());
		goto on_error;
	}

	void *map = SDL_MapGPUTransferBuffer(device, transfer, false);
	if (map == NULL) {
		ecs_err("Failed to map transfer buffer: %s", SDL_GetError());
		goto on_error;
	}
	SDL_memcpy(map, data, size);
	SDL_UnmapGPUTransferBuffer(device, transfer);

	SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);
	if (cmd == NULL) {
		ecs_err("Failed to acquire command buffer: %s", SDL_GetError());
		goto on_error;
	}
	
	SDL_GPUCopyPass *pass = SDL_BeginGPUCopyPass(cmd);
	if (pass == NULL) {
		ecs_err("Failed to begin copy pass: %s", SDL_GetError());
		goto on_error;
	}

	SDL_GPUTransferBufferLocation src = {0};
	src.transfer_buffer = transfer;
	src.offset = 0;
	SDL_GPUBufferRegion dst = {0};
	dst.buffer = gpubuf;
	dst.offset = 0;
	dst.size = size;
	SDL_UploadToGPUBuffer(pass, &src, &dst, false);
	SDL_EndGPUCopyPass(pass);
	SDL_SubmitGPUCommandBuffer(cmd);
	SDL_ReleaseGPUTransferBuffer(device, transfer);

	return 0;

on_error:
	if (cmd) {
		SDL_SubmitGPUCommandBuffer(cmd);
	}
	if (transfer) {
		SDL_ReleaseGPUTransferBuffer(device, transfer);
	}
	return -1;
}