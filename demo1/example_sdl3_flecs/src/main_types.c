#include "main_types.h"
#include <stdlib.h>

/*
static void shutdownGPU(RenderState * render_state, WindowState *window_states, SDL_GPUDevice *gpu_device, SDLTest_CommonState *state)
{
	if (window_states) {
		int i;
		for (i = 0; i < state->num_windows; i++) {
			WindowState *winstate = &window_states[i];
			SDL_ReleaseGPUTexture(gpu_device, winstate->tex_depth);
			SDL_ReleaseGPUTexture(gpu_device, winstate->tex_msaa);
			SDL_ReleaseGPUTexture(gpu_device, winstate->tex_resolve);
			SDL_ReleaseWindowFromGPUDevice(gpu_device, state->windows[i]);
		}
		SDL_free(window_states);
		window_states = NULL;
	}

	SDL_ReleaseGPUBuffer(gpu_device, render_state->buf_vertex);
	SDL_ReleaseGPUGraphicsPipeline(gpu_device, render_state->pipeline);
	SDL_DestroyGPUDevice(gpu_device);

	SDL_zero(render_state);
	gpu_device = NULL;
}




void quit(int rc, RenderState * render_state, WindowState *window_states, SDLTest_CommonState *state, SDL_GPUDevice *gpu_device)
{
	shutdownGPU(render_state, window_states, gpu_device, state);
	SDLTest_CommonQuit(state);
	exit(rc);
}
*/