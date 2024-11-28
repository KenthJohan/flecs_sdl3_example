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

static void ControllerRotate(ecs_iter_t *it)
{
	EgCamerasKeyBindings *controller = ecs_field(it, EgCamerasKeyBindings, 0);
	Rotate3 *rotate = ecs_field(it, Rotate3, 1);
	EgKeyboardsState *ckey = ecs_field(it, EgKeyboardsState, 2); // singleton
	uint8_t *keys = ckey->scancode;
	float k = 0.8f * it->delta_time;
	for (int i = 0; i < it->count; ++i, ++rotate) {
		rotate->dx = keys[controller->key_rotate_dx_plus] - keys[controller->key_rotate_dx_minus];
		rotate->dy = keys[controller->key_rotate_dy_plus] - keys[controller->key_rotate_dy_minus];
		rotate->dz = keys[controller->key_rotate_dz_plus] - keys[controller->key_rotate_dz_minus];
		v3f32_mul((float *)rotate, (float *)rotate, k);
	}
}

static void ControllerMove(ecs_iter_t *it)
{
	EgCamerasKeyBindings *controller = ecs_field(it, EgCamerasKeyBindings, 0);
	Velocity3 *vel = ecs_field(it, Velocity3, 1);
	EgKeyboardsState *ckey = ecs_field(it, EgKeyboardsState, 2);
	uint8_t *keys = ckey->scancode;
	float moving_speed = 1.1f;
	float k = it->delta_time * moving_speed;
	for (int i = 0; i < it->count; i++) {
		vel->x = -(keys[controller->key_move_dx_plus] - keys[controller->key_move_dx_minus]);
		vel->y = -(keys[controller->key_move_dy_plus] - keys[controller->key_move_dy_minus]);
		vel->z = -(keys[controller->key_move_dz_plus] - keys[controller->key_move_dz_minus]);
		v3f32_mul((float *)vel, (float *)vel, k);
	}
}

static void System_Draw(ecs_iter_t *it)
{
	EgGpuDevice *c_gpu = ecs_field(it, EgGpuDevice, 0);          // shared
	EgGpuPipeline *c_pipeline = ecs_field(it, EgGpuPipeline, 1); // shared
	EgGpuBuffer *c_buf = ecs_field(it, EgGpuBuffer, 2);          // shared
	EgGpuTexture *c_texd = ecs_field(it, EgGpuTexture, 3);       // shared
	EgWindowsWindow *c_win = ecs_field(it, EgWindowsWindow, 4);  // shared
	EgGpuWindow *c_gwin = ecs_field(it, EgGpuWindow, 5);         // shared
	EgCamerasState *c_cam = ecs_field(it, EgCamerasState, 6);    // shared
	EgGpuDrawCube *c_cube = ecs_field(it, EgGpuDrawCube, 7);     // self
	Transformation *c_trans = ecs_field(it, Transformation, 8);  // self

	SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(c_gpu->device);
	if (!cmd) {
		SDL_Log("Failed to acquire command buffer :%s", SDL_GetError());
		return;
	}

	SDL_GPUTexture *tex;
	Uint32 drawableh = 0;
	Uint32 drawablew = 0;
	if (!SDL_AcquireGPUSwapchainTexture(cmd, c_win->object, &tex, &drawablew, &drawableh)) {
		SDL_Log("Failed to acquire swapchain texture: %s", SDL_GetError());
		return;
	}

	if (tex == NULL) {
		// No swapchain was acquired, probably too many frames in flight
		SDL_SubmitGPUCommandBuffer(cmd);
		return;
	}

	SDL_GPUColorTargetInfo color_target;
	SDL_zero(color_target);
	color_target.clear_color.a = 1.0f;
	color_target.load_op = SDL_GPU_LOADOP_CLEAR;
	color_target.store_op = SDL_GPU_STOREOP_STORE;
	color_target.texture = tex;

	SDL_GPUDepthStencilTargetInfo depth_target;
	SDL_zero(depth_target);
	depth_target.clear_depth = 1.0f;
	depth_target.load_op = SDL_GPU_LOADOP_CLEAR;
	depth_target.store_op = SDL_GPU_STOREOP_DONT_CARE;
	depth_target.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
	depth_target.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
	depth_target.texture = c_texd->object;
	depth_target.cycle = true;

	SDL_GPUBufferBinding vertex_binding;
	vertex_binding.buffer = c_buf->object;
	vertex_binding.offset = 0;

	SDL_GPURenderPass *pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, &depth_target);
	SDL_BindGPUGraphicsPipeline(pass, c_pipeline->object);

	for (int i = 0; i < it->count; ++i, ++c_trans) {
		m4f32 mvp;
		m4f32_mul(&mvp, &c_cam->vp, &c_trans->matrix);
		SDL_PushGPUVertexUniformData(cmd, 0, &mvp, sizeof(float) * 16);
		SDL_BindGPUVertexBuffers(pass, 0, &vertex_binding, 1);
		SDL_DrawGPUPrimitives(pass, 36, 1, 0, 0);
	}

	SDL_EndGPURenderPass(pass);
	SDL_SubmitGPUCommandBuffer(cmd);
}

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

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/app.flecs");
	ecs_log_set_level(-1);

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "ControllerRotate", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = ControllerRotate,
	.query.terms = {
	{.id = ecs_id(EgCamerasKeyBindings), .src.id = EcsSelf},
	{.id = ecs_id(Rotate3), .src.id = EcsSelf},
	{.id = ecs_id(EgKeyboardsState), .src.id = ecs_id(EgKeyboardsState)},
	}});

	// ECS_SYSTEM(world, ControllerMove, EcsOnUpdate, KeyboardController, Velocity3, Window($));
	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "ControllerMove", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = ControllerMove,
	.query.terms = {
	{.id = ecs_id(EgCamerasKeyBindings), .src.id = EcsSelf},
	{.id = ecs_id(Velocity3), .src.id = EcsSelf},
	{.id = ecs_id(EgKeyboardsState), .src.id = ecs_id(EgKeyboardsState)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_Draw", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Draw,
	.query.terms = {
	{.id = ecs_id(EgGpuDevice), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuPipeline), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuBuffer), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuTexture), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgWindowsWindow), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuWindow), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgCamerasState), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuDrawCube), .src.id = EcsSelf},
	{.id = ecs_id(Transformation), .src.id = EcsSelf},
	}});

	EgKeyboardsState const *board = ecs_singleton_get(world, EgKeyboardsState);

	while (1) {
		if (board->scancode[SDL_SCANCODE_ESCAPE]) {
			break;
		}
		ecs_progress(world, 0.0f);
		// c_cam = ecs_get(world, e_cam, EgCamerasState);
		// main_render(c_win->object, c_gpu->device, c_pipeline->object, c_buf->object, c_texd->object, (float *)&c_cam->vp);
	}

	return 0;
}
