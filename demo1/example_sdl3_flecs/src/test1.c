#include "test1.h"


void gpu_transfer_begin(gpu_transfer_t *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb)
{
	ecs_assert(transfer != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(device != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb->object != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb->cap > 0, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb->last == 0, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->data == NULL, ECS_INVALID_PARAMETER, NULL);
	tb->last = 0;
	// You must unmap the transfer buffer before encoding upload commands.
	transfer->data = SDL_MapGPUTransferBuffer(device, tb->object, false);
	if (transfer->data == NULL) {
		ecs_err("Failed to map transfer buffer: %s", SDL_GetError());
	}
}


void gpu_transfer_add(gpu_transfer_t *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb, const void *src, uint32_t src_size, SDL_GPUBuffer * dst, uint32_t dst_offset)
{
	ecs_assert(transfer != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(device != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb->object != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb->cap > 0, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->data != NULL, ECS_INVALID_PARAMETER, NULL);
	// Check if the transfer buffer cap is too small:
	if (tb->last + src_size <= tb->cap) {
		ecs_err("Buffer cap is too small");
		return;
	}
	// Check if we have room for another command:
	if (transfer->cmd_last >= GPU_TRANSFER_MAX_CMDS) {
		ecs_err("Too many transfer commands");
		return;
	}
	memcpy(transfer->data + tb->last, src, src_size);
	gpu_transfer_cmd_t * cmd = &transfer->cmd[transfer->cmd_last];
	cmd->dst = dst;
	cmd->src_offset = tb->last;
	cmd->dst_offset = dst_offset;
	cmd->size = src_size;
	++transfer->cmd_last;
	tb->last += src_size;
}

void gpu_transfer_end(gpu_transfer_t *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb)
{
	ecs_assert(transfer != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(device != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb->object != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb->cap > 0, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->data != NULL, ECS_INVALID_PARAMETER, NULL);
	// Unmap the transfer buffer before encoding upload commands.
	SDL_UnmapGPUTransferBuffer(device, tb->object);
	transfer->data = NULL;
}


void gpu_transfer_submit(gpu_transfer_t *transfer, SDL_GPUDevice *device, EgGpuBufferTransfer *tb)
{
	ecs_assert(transfer != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(device != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb->object != NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(tb->cap > 0, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->data == NULL, ECS_INVALID_PARAMETER, NULL);
	ecs_assert(transfer->cmd_last <= GPU_TRANSFER_MAX_CMDS, ECS_INVALID_PARAMETER, NULL);
	SDL_GPUCommandBuffer *cmds = SDL_AcquireGPUCommandBuffer(device);
	if (!cmds) {
		ecs_err("Failed to acquire command buffer :%s", SDL_GetError());
		return;
	}
	SDL_GPUCopyPass *pass = SDL_BeginGPUCopyPass(cmds);
	if (!pass) {
		SDL_SubmitGPUCommandBuffer(cmds);
		ecs_err("Failed to begin copy pass :%s", SDL_GetError());
		return;
	}
	// Encode the upload commands:
	for (uint32_t i = 0; i < transfer->cmd_last; ++i) {
		gpu_transfer_cmd_t * cmd = &transfer->cmd[i];
		SDL_UploadToGPUBuffer(
			pass,
			&(SDL_GPUTransferBufferLocation) {
				.transfer_buffer = tb->object,
				.offset = cmd->src_offset
			},
			&(SDL_GPUBufferRegion) {
				.buffer = cmd->dst,
				.offset = cmd->dst_offset,
				.size = cmd->size
			},
			false
		);
	}
	SDL_EndGPUCopyPass(pass);
	SDL_SubmitGPUCommandBuffer(cmds);
}