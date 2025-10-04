#include <stdlib.h>
#include <stdio.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_version.h>

#include <flecs.h>

#include <EgBase.h>
#include <EgSpatials.h>
#include <EgCameras.h>
#include <EgCamcontrols.h>
#include <EgShapes.h>
#include <EgKeyboards.h>
#include <EgWindows.h>
#include <EgGlslang.h>
#include <EgFs.h>

#include "EgDisplay.h"
#include "EgGpu.h"
#include "EgMeshes.h"
#include "EgRenderers.h"

// include header for getcwd
#include <unistd.h>
#include <limits.h> // For PATH_MAX

static void System_Draw1(ecs_iter_t *it, SDL_GPUCommandBuffer *cmd, SDL_GPURenderPass *pass)
{
	while (ecs_query_next(it)) {
		EgBaseOffsetCount *d0 = ecs_field(it, EgBaseOffsetCount, 0); // shared
		Transformation *x = ecs_field(it, Transformation, 1);        // self
		EgCamerasState *c0 = ecs_field(it, EgCamerasState, 2);       // shared
		for (int i = 0; i < it->count; ++i, ++x) {
			m4f32 mvp;
			m4f32_mul(&mvp, &c0->vp, &x->matrix);
			SDL_PushGPUVertexUniformData(cmd, 0, &mvp, sizeof(float) * 16);
			// SDL_DrawGPUPrimitives(pass, 36, 1, 0, 0);
			SDL_DrawGPUPrimitives(pass, d0->count, 1, d0->offset, 0);
		}
	}
}

static void System_Draw(ecs_iter_t *it)
{
	EgGpuDraw1 *c_draw1 = ecs_field(it, EgGpuDraw1, 0);             // self
	EgShapesRectangle *r = ecs_field(it, EgShapesRectangle, 1);     // self
	EgGpuDevice *c_gpu = ecs_field(it, EgGpuDevice, 2);             // shared
	EgGpuPipeline *c_pipeline = ecs_field(it, EgGpuPipeline, 3);    // shared
	EgGpuBufferVertex *c_buf = ecs_field(it, EgGpuBufferVertex, 4); // shared
	EgGpuTexture *c_texd = ecs_field(it, EgGpuTexture, 5);          // shared
	EgWindowsWindow *c_win = ecs_field(it, EgWindowsWindow, 6);     // shared
	EgGpuWindow *c_gwin = ecs_field(it, EgGpuWindow, 7);            // shared
	(void)c_gwin;

	ecs_world_t *world = it->world;
	SDL_GPUDevice *device = c_gpu->device;
	SDL_Window *window = c_win->object;
	SDL_GPUTexture *texture_depth = c_texd->object;
	SDL_GPUGraphicsPipeline *pipeline = c_pipeline->object;
	SDL_GPUBuffer *buffer = c_buf->object;
	ecs_query_t *query = c_draw1->query;

	for (int i = 0; i < it->count; ++i, ++r) {
		ecs_entity_t e = it->entities[i];

		SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(device);
		if (cmd == NULL) {
			ecs_err("Failed to acquire command buffer :%s", SDL_GetError());
			ecs_add(world, e, EgBaseError);
			ecs_enable(world, e, false);
			continue;
		}

		SDL_GPUTexture *texture_swapchain = NULL;
		Uint32 w = 0;
		Uint32 h = 0;
		if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd, window, &texture_swapchain, &w, &h)) {
			ecs_err("Failed to acquire swapchain texture: %s", SDL_GetError());
			ecs_add(world, e, EgBaseError);
			ecs_enable(world, e, false);
			continue;
		}

		if (texture_swapchain == NULL) {
			// No swapchain was acquired, probably too many frames in flight.
			// you must always submit the command buffer.
			SDL_SubmitGPUCommandBuffer(cmd);
			continue;
		}

		// Update the depth texture size if window has been resized:
		if (r->w != w || r->h != h) {
			r->w = w;
			r->h = h;
			ecs_entity_t depth = ecs_field_src(it, 5);
			ecs_set(it->world, depth, EgShapesRectangle, {r->w, r->h});
			//ecs_modified(it->world, depth, EgShapesRectangle);
		}

		if (texture_depth) {
			SDL_GPUColorTargetInfo color_target = {0};
			SDL_zero(color_target);
			color_target.clear_color.a = 1.0f;
			color_target.load_op = SDL_GPU_LOADOP_CLEAR;
			color_target.store_op = SDL_GPU_STOREOP_STORE;
			color_target.texture = texture_swapchain;
			SDL_GPUDepthStencilTargetInfo depth_target;
			SDL_zero(depth_target);
			depth_target.clear_depth = 1.0f;
			depth_target.load_op = SDL_GPU_LOADOP_CLEAR;
			depth_target.store_op = SDL_GPU_STOREOP_DONT_CARE;
			depth_target.stencil_load_op = SDL_GPU_LOADOP_DONT_CARE;
			depth_target.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;
			depth_target.texture = texture_depth;
			depth_target.cycle = true;
			SDL_GPUBufferBinding vertex_binding;
			vertex_binding.buffer = buffer;
			vertex_binding.offset = 0;
			SDL_GPURenderPass *pass = SDL_BeginGPURenderPass(cmd, &color_target, 1, &depth_target);
			SDL_BindGPUGraphicsPipeline(pass, pipeline);
			SDL_BindGPUVertexBuffers(pass, 0, &vertex_binding, 1);
			// call the inner system:
			ecs_iter_t it2 = ecs_query_iter(world, query);
			System_Draw1(&it2, cmd, pass);
			SDL_EndGPURenderPass(pass);
		}

		SDL_SubmitGPUCommandBuffer(cmd);
	}
}

int main(int argc, char *argv[])
{
	ecs_os_set_api_defaults();
	ecs_os_api_t os_api = ecs_os_get_api();
	ecs_os_set_api(&os_api);

	ecs_log_set_level(0);
	ecs_trace("SDL_GetRevision %s\n", SDL_GetRevision());

	// print current directory
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("Current working dir: %s\n", cwd);
	} else {
		perror("getcwd() error");
	}

	if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
		ecs_err("Couldn't initialize video driver: %s\n", SDL_GetError());
		return 1;
	}

	if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
		ecs_err("Couldn't initialize video driver: %s\n", SDL_GetError());
		return 1;
	}
	ecs_log_set_level(-1);



	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, FlecsDoc);
	ECS_IMPORT(world, FlecsStats);
	ECS_IMPORT(world, EgBase);
	ECS_IMPORT(world, EgSpatials);
	ECS_IMPORT(world, EgShapes);
	ECS_IMPORT(world, EgCameras);
	ECS_IMPORT(world, EgFs);
	ECS_IMPORT(world, EgFsEpoll);
	ECS_IMPORT(world, EgFsInotify);
	ECS_IMPORT(world, EgFsSocket);
	ECS_IMPORT(world, EgDisplay);
	ECS_IMPORT(world, EgGpu);
	ECS_IMPORT(world, EgWindows);
	ECS_IMPORT(world, EgKeyboards);
	ECS_IMPORT(world, EgMeshes);
	ECS_IMPORT(world, EgCamcontrols);
	ECS_IMPORT(world, EgRenderers);
	ECS_IMPORT(world, EgGlslang);

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

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/xfiles.flecs");
	ecs_log_set_level(-1);

	{
		// Create a query for the draw system
		ecs_entity_t e_draw1 = ecs_lookup(world, "xgpu.gpu0.renderer1");
		if (e_draw1 == 0) {
			ecs_os_abort();
		}
		ecs_query_t *q = ecs_query(world,
		{.terms = {
		 {.id = ecs_id(EgBaseOffsetCount), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
		 {.id = ecs_id(Transformation), .src.id = EcsSelf, .inout = EcsIn},
		 {.id = ecs_id(EgCamerasState), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn}}});
		ecs_set(world, e_draw1, EgGpuDraw1, {.query = q});
	}

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_Draw", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Draw,
	.query.terms = {
	{.id = ecs_id(EgGpuDraw1), .src.id = EcsSelf, .inout = EcsIn},
	{.id = ecs_id(EgShapesRectangle), .src.id = EcsSelf, .inout = EcsInOut},
	{.id = ecs_id(EgGpuDevice), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuPipeline), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuBufferVertex), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuTexture), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsInOut}, // Depth texture
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
