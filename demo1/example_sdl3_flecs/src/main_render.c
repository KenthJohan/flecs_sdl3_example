#include "main_render.h"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>
#include <stdlib.h>
#include <stdlib.h>



void draw_function(SDL_GPUCommandBuffer *cmd, SDL_GPURenderPass *pass, const SDL_GPUBufferBinding *bindings, float const mat[16])
{
	// for (int i = 0; i < 16; i++) {
	// mat[15] = i;
	SDL_PushGPUVertexUniformData(cmd, 0, mat, sizeof(float) * 16);
	SDL_BindGPUVertexBuffers(pass, 0, bindings, 1);
	SDL_DrawGPUPrimitives(pass, 36, 1, 0, 0);
	//}
}

void main_render(
SDL_Window *window,
SDL_GPUDevice *gpu_device,
SDL_GPUGraphicsPipeline *pipeline,
SDL_GPUBuffer *buf_vertex,
SDL_GPUTexture *tex_depth,
float const *mvp)
{

	Uint32 drawablew, drawableh;

	/* Acquire the swapchain texture */

	SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(gpu_device);
	if (!cmd) {
		SDL_Log("Failed to acquire command buffer :%s", SDL_GetError());
		return;
	}

	SDL_GPUTexture *swapchainTexture;
	if (!SDL_AcquireGPUSwapchainTexture(cmd, window, &swapchainTexture, &drawablew, &drawableh)) {
		SDL_Log("Failed to acquire swapchain texture: %s", SDL_GetError());
		return;
	}

	if (swapchainTexture == NULL) {
		/* No swapchain was acquired, probably too many frames in flight */
		SDL_SubmitGPUCommandBuffer(cmd);
		return;
	}

	/* Set up the pass */
	SDL_GPUColorTargetInfo color_target;
	SDL_zero(color_target);
	color_target.clear_color.a = 1.0f;
	color_target.load_op = SDL_GPU_LOADOP_CLEAR;
	color_target.store_op = SDL_GPU_STOREOP_STORE;
	color_target.texture = swapchainTexture;

	SDL_GPUDepthStencilTargetInfo depth_target;
	SDL_zero(depth_target);
	depth_target.clear_depth = 1.0f;
	depth_target.load_op = SDL_GPU_LOADOP_CLEAR;
	depth_target.store_op = SDL_GPU_STOREOP_DONT_CARE;
	depth_target.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
	depth_target.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
	depth_target.texture = tex_depth;
	depth_target.cycle = true;

	/* Set up the bindings */

	SDL_GPUBufferBinding vertex_binding;
	vertex_binding.buffer = buf_vertex;
	vertex_binding.offset = 0;

	SDL_GPURenderPass *pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, &depth_target);
	SDL_BindGPUGraphicsPipeline(pass, pipeline);

	// draw
	// draw_function(cmd, pass, &vertex_binding, (float *)&matrix_final);
	draw_function(cmd, pass, &vertex_binding, mvp);

	SDL_EndGPURenderPass(pass);

	/* Submit the command buffer! */
	SDL_SubmitGPUCommandBuffer(cmd);
}