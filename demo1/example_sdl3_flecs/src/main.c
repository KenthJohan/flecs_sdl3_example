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
#include <SDL3/SDL_log.h>
#include <stdio.h>
#include <flecs.h>

#include <EgBase.h>
#include <EgSpatials.h>
#include <EgCameras.h>
#include <EgShapes.h>
#include <EgKeyboards.h>
#include <EgWindows.h>


#include "EgFs.h"
#include "EgDisplay.h"
#include "EgGpu.h"

#include "main_render.h"




int main(int argc, char *argv[])
{

	if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize video driver: %s\n", SDL_GetError());
		return 1;
	}

	if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
		SDL_Log("Couldn't initialize video driver: %s\n", SDL_GetError());
		return 1;
	}

	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, FlecsDoc);
	ECS_IMPORT(world, EgBase);
	ECS_IMPORT(world, EgSpatials);
	ECS_IMPORT(world, EgShapes);
	ECS_IMPORT(world, EgCameras);
	ECS_IMPORT(world, EgFs);
	ECS_IMPORT(world, EgDisplay);
	ECS_IMPORT(world, EgGpu);
	ECS_IMPORT(world, EgWindows);
	ECS_IMPORT(world, EgKeyboards);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/hello.flecs");
	ecs_log_set_level(-1);

	ecs_entity_t e_gpu = ecs_lookup(world, "hello.default_gpu");
	ecs_entity_t e_pipeline = ecs_lookup(world, "hello.default_gpu.pipeline");
	ecs_entity_t e_vert1 = ecs_lookup(world, "hello.default_gpu.vert1");
	ecs_entity_t e_texd = ecs_lookup(world, "hello.default_gpu.window1.tex_depth");
	ecs_entity_t e_win = ecs_lookup(world, "hello.default_gpu.window1");
	ecs_entity_t e_cam = ecs_lookup(world, "hello.default_gpu.window1.cam");

	EgGpuPipeline const *c_pipeline = NULL;
	EgGpuDevice const *c_gpu = NULL;
	EgGpuBuffer const *c_buf = NULL;
	EgGpuTexture const *c_texd = NULL;
	EgWindowsWindow const *c_win = NULL;
	EgCamerasState const *c_cam = NULL;
	while (1) {
		ecs_progress(world, 0.0f);
		c_pipeline = ecs_get(world, e_pipeline, EgGpuPipeline);
		c_gpu = ecs_get(world, e_gpu, EgGpuDevice);
		c_buf = ecs_get(world, e_vert1, EgGpuBuffer);
		c_texd = ecs_get(world, e_texd, EgGpuTexture);
		c_win = ecs_get(world, e_win, EgWindowsWindow);
		c_cam = ecs_get(world, e_cam, EgCamerasState);
		if (c_pipeline == NULL) {
			continue;
		}
		if (c_gpu == NULL) {
			continue;
		}
		if (c_buf == NULL) {
			continue;
		}
		if (c_texd == NULL) {
			continue;
		}
		if (c_win == NULL) {
			continue;
		}
		if (c_cam == NULL) {
			continue;
		}
		break;
	}

	SDL_ClaimWindowForGPUDevice(c_gpu->device, c_win->object);

	EgKeyboardsState const *board = ecs_singleton_get(world, EgKeyboardsState);

	while (1) {
		if (board->scancode[SDL_SCANCODE_ESCAPE]) {
			break;
		}
		ecs_progress(world, 0.0f);
		c_cam = ecs_get(world, e_cam, EgCamerasState);
		main_render(c_win->object, c_gpu->device, c_pipeline->object, c_buf->object, c_texd->object, (float *)&c_cam->vp);
	}

	return 0;
}
