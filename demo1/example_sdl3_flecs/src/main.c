#include <stdlib.h>
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_log.h>

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
#include "EgMeshes.h"

static void ControllerRotate(ecs_iter_t *it)
{
	EgCamerasKeyBindings *controller = ecs_field(it, EgCamerasKeyBindings, 0);
	Rotate3 *rotate = ecs_field(it, Rotate3, 1);
	EgKeyboardsState *ckey = ecs_field(it, EgKeyboardsState, 2); // singleton
	uint8_t *keys = ckey->scancode;
	float k = 0.8f * it->delta_time;
	for (int i = 0; i < it->count; ++i, ++rotate) {
		rotate->dx = !!keys[controller->key_rotate_dx_plus] - !!keys[controller->key_rotate_dx_minus];
		rotate->dy = !!keys[controller->key_rotate_dy_plus] - !!keys[controller->key_rotate_dy_minus];
		rotate->dz = !!keys[controller->key_rotate_dz_plus] - !!keys[controller->key_rotate_dz_minus];
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
		vel->x = -(!!keys[controller->key_move_dx_plus] - !!keys[controller->key_move_dx_minus]);
		vel->y = -(!!keys[controller->key_move_dy_plus] - !!keys[controller->key_move_dy_minus]);
		vel->z = -(!!keys[controller->key_move_dz_plus] - !!keys[controller->key_move_dz_minus]);
		v3f32_mul((float *)vel, (float *)vel, k);
	}
}

static void System_Draw1(ecs_iter_t *it, SDL_GPUCommandBuffer *cmd, SDL_GPURenderPass *pass)
{
	while (ecs_query_next(it)) {
		EgGpuDrawPrimitive *field_prim = ecs_field(it, EgGpuDrawPrimitive, 0); // self
		Transformation *field_trans = ecs_field(it, Transformation, 1);        // self
		EgCamerasState *field_cam = ecs_field(it, EgCamerasState, 2);          // shared
		(void)field_prim;
		for (int i = 0; i < it->count; ++i, ++field_trans, ++field_prim) {
			m4f32 mvp;
			m4f32_mul(&mvp, &field_cam->vp, &field_trans->matrix);
			SDL_PushGPUVertexUniformData(cmd, 0, &mvp, sizeof(float) * 16);
			//SDL_DrawGPUPrimitives(pass, 36, 1, 0, 0);
			SDL_DrawGPUPrimitives(pass, field_prim->num_vertices, field_prim->num_instances, field_prim->first_vertex, field_prim->first_instance);
		}
	}
}

static void System_Draw(ecs_iter_t *it)
{
	EgGpuDraw1 *c_draw1 = ecs_field(it, EgGpuDraw1, 0);             // self
	EgShapesRectangle *c_rec = ecs_field(it, EgShapesRectangle, 1); // self
	EgGpuDevice *c_gpu = ecs_field(it, EgGpuDevice, 2);             // shared
	EgGpuPipeline *c_pipeline = ecs_field(it, EgGpuPipeline, 3);    // shared
	EgGpuBuffer *c_buf = ecs_field(it, EgGpuBuffer, 4);             // shared
	EgGpuTexture *c_texd = ecs_field(it, EgGpuTexture, 5);          // shared
	EgWindowsWindow *c_win = ecs_field(it, EgWindowsWindow, 6);     // shared
	EgGpuWindow *c_gwin = ecs_field(it, EgGpuWindow, 7);            // shared
	(void)c_gwin;

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

	if (c_rec->w != drawablew || c_rec->h != drawableh) {
		SDL_ReleaseGPUTexture(c_gpu->device, c_texd->object);
		SDL_GPUTextureCreateInfo createinfo = {0};
		createinfo.type = SDL_GPU_TEXTURETYPE_2D;
		createinfo.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
		createinfo.width = drawablew;
		createinfo.height = drawableh;
		createinfo.layer_count_or_depth = 1;
		createinfo.num_levels = 1;
		createinfo.sample_count = 1;
		createinfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
		createinfo.props = 0;
		c_texd->object = SDL_CreateGPUTexture(c_gpu->device, &createinfo);
		c_rec->w = drawablew;
		c_rec->h = drawableh;
	}

	if (tex == NULL) {
		// No swapchain was acquired, probably too many frames in flight
		SDL_SubmitGPUCommandBuffer(cmd);
		return;
	}

	if (c_texd->object) {
		SDL_GPUColorTargetInfo color_target = {0};
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
		SDL_BindGPUVertexBuffers(pass, 0, &vertex_binding, 1);
		ecs_iter_t it2 = ecs_query_iter(it->world, c_draw1->query);
		System_Draw1(&it2, cmd, pass); // call the inner system
		SDL_EndGPURenderPass(pass);
	}

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

	ecs_os_set_api_defaults();
	ecs_os_api_t os_api = ecs_os_get_api();
	ecs_os_set_api(&os_api);

	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, FlecsDoc);
	ECS_IMPORT(world, FlecsStats);
	ECS_IMPORT(world, EgBase);
	ECS_IMPORT(world, EgSpatials);
	ECS_IMPORT(world, EgShapes);
	ECS_IMPORT(world, EgCameras);
	ECS_IMPORT(world, EgFs);
	ECS_IMPORT(world, EgDisplay);
	ECS_IMPORT(world, EgGpu);
	ECS_IMPORT(world, EgWindows);
	ECS_IMPORT(world, EgKeyboards);
	ECS_IMPORT(world, EgMeshes);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/xvertex.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/xmeshes.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/xgpu.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/xcam.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/xapp.flecs");
	ecs_log_set_level(-1);

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/xkeybinds.flecs");
	ecs_log_set_level(-1);

	{
		ecs_entity_t e_draw1 = ecs_lookup(world, "xapp.renderer");
		ecs_query_t *q = ecs_query(world,
		{.terms = {
		 {.id = ecs_id(EgGpuDrawPrimitive), .src.id = EcsSelf},
		 {.id = ecs_id(Transformation), .src.id = EcsSelf},
		 {.id = ecs_id(EgCamerasState), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn}}});
		ecs_set(world, e_draw1, EgGpuDraw1, {.query = q});
	}

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "ControllerRotate", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = ControllerRotate,
	.query.terms = {
	{.id = ecs_id(EgCamerasKeyBindings), .src.id = EcsSelf},
	{.id = ecs_id(Rotate3), .src.id = EcsSelf},
	{.id = ecs_id(EgKeyboardsState), .src.id = ecs_id(EgKeyboardsState)},
	}});

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
	{.id = ecs_id(EgGpuDraw1), .src.id = EcsSelf, .inout = EcsIn},
	{.id = ecs_id(EgShapesRectangle), .src.id = EcsSelf, .inout = EcsIn},
	{.id = ecs_id(EgGpuDevice), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuPipeline), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuBuffer), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuTexture), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgWindowsWindow), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuWindow), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	}});

	EgKeyboardsState const *board = ecs_singleton_get(world, EgKeyboardsState);

	while (1) {
		if (board->scancode[SDL_SCANCODE_ESCAPE]) {
			break;
		}
		ecs_progress(world, 0.0f);
	}

	return 0;
}
