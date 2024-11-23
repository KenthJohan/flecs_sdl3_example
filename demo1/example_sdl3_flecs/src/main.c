/*
  Copyright (C) 1997-2024 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/

#include <stdlib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <flecs.h>

#include <EgBase.h>
#include <EgSpatials.h>
#include <EgCameras.h>
#include <EgShapes.h>

/* Regenerate the shaders with testgpu/build-shaders.sh */
#include "../shaders/testgpu_spirv.h"

#include "shader_spirv.h"
#include "vertex.h"
#include "matrix.h"

#include "EgFs.h"
#include "EgDisplay.h"
#include "EgGpu.h"
#include "EgWindows.h"
#include "main_render.h"
#include "main_types.h"
#include "SDL_test_common.h"

#define TESTGPU_SUPPORTED_FORMATS (SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXBC | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB)

static SDLTest_CommonState *state = NULL;
static WindowState *window_states = NULL;




int main(int argc, char *argv[])
{

	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, FlecsDoc);
	ECS_IMPORT(world, EgBase);
	ECS_IMPORT(world, EgSpatials);
	ECS_IMPORT(world, EgShapes);
	ECS_IMPORT(world, EgCameras);
	
	
	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	/* Initialize test framework */
	state = SDLTest_CommonCreateState(argv, SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (!state) {
		return 1;
	}

	state->verbose = VERBOSE_VIDEO | VERBOSE_MODES | VERBOSE_RENDER | VERBOSE_EVENT | VERBOSE_AUDIO | VERBOSE_MOTION;
	state->verbose = 0;
	state->skip_renderer = 1;
	state->window_flags |= SDL_WINDOW_RESIZABLE;

	if (!SDLTest_CommonInit(state)) {
		return 0;
	}

	ECS_IMPORT(world, EgFs);
	ECS_IMPORT(world, EgDisplay);
	ECS_IMPORT(world, EgGpu);
	ECS_IMPORT(world, EgWindows);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/hello.flecs");
	ecs_log_set_level(-1);

	const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(state->windows[0]));
	SDL_Log("Screen bpp: %d\n", SDL_BITSPERPIXEL(mode->format));


	ecs_entity_t e_gpu = ecs_lookup(world, "hello.default_gpu");
	ecs_entity_t e_pipeline = ecs_lookup(world, "hello.default_gpu.pipeline");
	ecs_entity_t e_vert1 = ecs_lookup(world, "hello.default_gpu.vert1");

	EgGpuPipeline const *c_pipeline = NULL;
	EgGpuDevice const *c_gpu = NULL;
	EgGpuBuffer const *c_buf = NULL;
	while (1) {
		ecs_progress(world, 0.0f);
		c_pipeline = ecs_get(world, e_pipeline, EgGpuPipeline);
		c_gpu = ecs_get(world, e_gpu, EgGpuDevice);
		c_buf = ecs_get(world, e_vert1, EgGpuBuffer);
		if (c_pipeline == NULL) {
			continue;
		}
		if (c_gpu == NULL) {
			continue;
		}
		if (c_buf == NULL) {
			continue;
		}
		break;
	}



	window_states = (WindowState *)SDL_calloc(state->num_windows, sizeof(WindowState));
	if (!window_states) {
		SDL_Log("Out of memory!\n");
		return 1;
	}

	for (int i = 0; i < state->num_windows; i++) {
		WindowState *winstate = &window_states[i];
		/* create a depth texture for the window */
		Uint32 drawablew, drawableh;
		SDL_GetWindowSizeInPixels(state->windows[i], (int *)&drawablew, (int *)&drawableh);
		winstate->tex_depth = CreateDepthTexture(SDL_GPU_SAMPLECOUNT_1, state, c_gpu->device, drawablew, drawableh);
		winstate->tex_msaa = CreateMSAATexture(SDL_GPU_SAMPLECOUNT_1, state, c_gpu->device, drawablew, drawableh);
		winstate->tex_resolve = CreateResolveTexture(SDL_GPU_SAMPLECOUNT_1, state, c_gpu->device, drawablew, drawableh);
		/* make each window different */
		winstate->angle_x = (i * 10) % 360;
		winstate->angle_y = (i * 20) % 360;
		winstate->angle_z = (i * 30) % 360;
	}

	/* Claim the windows */
	for (int i = 0; i < state->num_windows; i++) {
		SDL_ClaimWindowForGPUDevice(c_gpu->device, state->windows[i]);
	}

	while (1) {
		ecs_progress(world, 0.0f);
		SDL_Event event;
		int i;

		/* Check for events */
		int done = 0;
		while (SDL_PollEvent(&event) && !done) {
			SDLTest_CommonEvent(state, &event, &done);
		}
		if (done) {
			break;
		}
		for (i = 0; i < state->num_windows; ++i) {
			main_render(state, state->windows, c_gpu->device, window_states, i, c_pipeline->object, c_buf->object);
		}
	}

	return 0;
}
