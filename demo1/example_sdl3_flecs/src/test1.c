#include <SDL3/SDL_gpu.h>
#include <flecs.h>
#include "EgGpu.h"

typedef struct {
	SDL_GPUDevice *device;
	EgGpuBufferTransfer *tb;
	SDL_GPUCommandBuffer *cmds;
	SDL_GPUCopyPass *pass;
	uint8_t *data;
} gpu_transfer_t;

void gpu_transfer_begin(gpu_transfer_t *transfer)
{
	ecs_assert(transfer != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->device != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->tb != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->tb->object != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->tb->cap > 0, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->tb->last == 0, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->data == NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->cmds == NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->pass == NULL, ECS_INVALID_PARAMETER, NULL);
	transfer->tb->last = 0;
	transfer->data = SDL_MapGPUTransferBuffer(transfer->device, transfer->tb->object, false);
	if (transfer->data == NULL) {
		ecs_err("Failed to map transfer buffer: %s", SDL_GetError());
	}
}


void gpu_transfer_add(gpu_transfer_t *transfer, const void *src, uint32_t src_size)
{
	ecs_assert(transfer != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->device != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->tb != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->tb->object != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->tb->cap > 0, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->tb->last + src_size <= transfer->tb->cap, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->data != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->cmds == NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->pass == NULL, ECS_INVALID_PARAMETER, NULL);
	memcpy(transfer->data + transfer->tb->last, src, src_size);
	transfer->tb->last += src_size;
}