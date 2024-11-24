#include "main_render.h"
#include "main_types.h"
#include "matrix.h"
#include "SDL_test_common.h"
#include <SDL3/SDL_gpu.h>
#include <stdlib.h>
#include <stdlib.h>

#define CHECK_CREATE(var, thing)                                         \
	{                                                                    \
		if (!(var)) {                                                    \
			SDL_Log("Failed to create %s: %s\n", thing, SDL_GetError()); \
			quit(2);                                                     \
		}                                                                \
	}

SDL_GPUTexture * CreateDepthTexture(SDL_GPUSampleCount sample_count, SDLTest_CommonState *state, SDL_GPUDevice *gpu_device, Uint32 drawablew, Uint32 drawableh)
{
	SDL_GPUTextureCreateInfo createinfo;
	SDL_GPUTexture *result;

	createinfo.type = SDL_GPU_TEXTURETYPE_2D;
	createinfo.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
	createinfo.width = drawablew;
	createinfo.height = drawableh;
	createinfo.layer_count_or_depth = 1;
	createinfo.num_levels = 1;
	createinfo.sample_count = sample_count;
	createinfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
	createinfo.props = 0;

	result = SDL_CreateGPUTexture(gpu_device, &createinfo);

	return result;
}






void draw_function(SDL_GPUCommandBuffer *cmd, SDL_GPURenderPass *pass, const SDL_GPUBufferBinding *bindings, float mat[16])
{
	for (int i = 0; i < 16; i++) {
		mat[15] = i;
		SDL_PushGPUVertexUniformData(cmd, 0, mat, sizeof(float)*16);
		SDL_BindGPUVertexBuffers(pass, 0, bindings, 1);
		SDL_DrawGPUPrimitives(pass, 36, 1, 0, 0);
	}
}






void main_render(
	SDL_Window * window, 
	SDL_GPUDevice *gpu_device, 
	WindowState * window_states, 
	const int windownum, 
	SDL_GPUGraphicsPipeline *pipeline, 
	SDL_GPUBuffer *buf_vertex,
	SDL_GPUTexture *tex_depth
)
{
	WindowState *winstate = &window_states[windownum];
	SDL_GPUTexture *swapchainTexture;
	SDL_GPUColorTargetInfo color_target;
	SDL_GPUDepthStencilTargetInfo depth_target;
	float matrix_rotate[16], matrix_modelview[16], matrix_perspective[16], matrix_final[16];
	SDL_GPUCommandBuffer *cmd;
	SDL_GPURenderPass *pass;
	Uint32 drawablew, drawableh;

	/* Acquire the swapchain texture */

	cmd = SDL_AcquireGPUCommandBuffer(gpu_device);
	if (!cmd) {
		SDL_Log("Failed to acquire command buffer :%s", SDL_GetError());
		return;
	}
	if (!SDL_AcquireGPUSwapchainTexture(cmd, window, &swapchainTexture, &drawablew, &drawableh)) {
		SDL_Log("Failed to acquire swapchain texture: %s", SDL_GetError());
		return;
	}

	if (swapchainTexture == NULL) {
		/* No swapchain was acquired, probably too many frames in flight */
		SDL_SubmitGPUCommandBuffer(cmd);
		return;
	}

	/*
	 * Do some rotation with Euler angles. It is not a fixed axis as
	 * quaterions would be, but the effect is cool.
	 */
	rotate_matrix((float)winstate->angle_x, 1.0f, 0.0f, 0.0f, matrix_modelview);
	rotate_matrix((float)winstate->angle_y, 0.0f, 1.0f, 0.0f, matrix_rotate);

	multiply_matrix(matrix_rotate, matrix_modelview, matrix_modelview);

	rotate_matrix((float)winstate->angle_z, 0.0f, 1.0f, 0.0f, matrix_rotate);

	multiply_matrix(matrix_rotate, matrix_modelview, matrix_modelview);

	/* Pull the camera back from the cube */
	matrix_modelview[14] -= 2.5f;

	perspective_matrix(45.0f, (float)drawablew / drawableh, 0.01f, 100.0f, matrix_perspective);
	multiply_matrix(matrix_perspective, matrix_modelview, (float *)&matrix_final);

	winstate->angle_x += 3;
	winstate->angle_y += 2;
	winstate->angle_z += 1;

	if (winstate->angle_x >= 360)
		winstate->angle_x -= 360;
	if (winstate->angle_x < 0)
		winstate->angle_x += 360;
	if (winstate->angle_y >= 360)
		winstate->angle_y -= 360;
	if (winstate->angle_y < 0)
		winstate->angle_y += 360;
	if (winstate->angle_z >= 360)
		winstate->angle_z -= 360;
	if (winstate->angle_z < 0)
		winstate->angle_z += 360;

	/* Resize the depth buffer if the window size changed */

	/* Set up the pass */
	SDL_zero(color_target);
	color_target.clear_color.a = 1.0f;
	color_target.load_op = SDL_GPU_LOADOP_CLEAR;
	color_target.store_op = SDL_GPU_STOREOP_STORE;
	color_target.texture = swapchainTexture;


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

	/* Draw the cube! */
	pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, &depth_target);
	SDL_BindGPUGraphicsPipeline(pass, pipeline);

	// draw
	draw_function(cmd, pass, &vertex_binding, (float *)&matrix_final);

	SDL_EndGPURenderPass(pass);

	/* Submit the command buffer! */
	SDL_SubmitGPUCommandBuffer(cmd);
}