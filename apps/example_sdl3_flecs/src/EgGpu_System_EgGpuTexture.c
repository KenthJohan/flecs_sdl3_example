#include "EgGpu_System_EgGpuBuffer.h"
#include "EgGpu.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <EgBase.h>
#include <EgShapes.h>

void System_EgGpuTexture_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	ecs_log_set_level(1);
	ecs_trace("System_EgGpuTexture_Create() count:%i", it->count);
	ecs_log_push_(0);

	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		// Entities can be annotated with the Final trait, which prevents using them with IsA relationship.
		ecs_add_id(world, e, EcsFinal);
	}

	EgGpuDevice *c_gpu = ecs_field(it, EgGpuDevice, 0);                          // parent
	EgShapesRectangle *c_rect = ecs_field(it, EgShapesRectangle, 1);             // parent
	EgGpuTextureCreateInfo *c_create = ecs_field(it, EgGpuTextureCreateInfo, 2); // self
	for (int i = 0; i < it->count; ++i, ++c_create) {
		ecs_entity_t e = it->entities[i];
		ecs_trace("Entity: '%s'", ecs_get_name(world, e));

		if (c_rect->w <= 0) {
			ecs_err("EgShapesRectangle width is zero or negative");
			ecs_add(world, e, EgBaseError);
			ecs_enable(world, e, false);
			continue;
		}

		if (c_rect->h <= 0) {
			ecs_err("EgShapesRectangle height is zero or negative");
			ecs_add(world, e, EgBaseError);
			ecs_enable(world, e, false);
			continue;
		}

		ecs_log_push_(0);
		{
			SDL_GPUTextureCreateInfo createinfo = {0};
			createinfo.type = SDL_GPU_TEXTURETYPE_2D;
			createinfo.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
			createinfo.width = c_rect->w;
			createinfo.height = c_rect->h;
			createinfo.layer_count_or_depth = 1;
			createinfo.num_levels = 1;
			createinfo.sample_count = c_create->sample_count;
			createinfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
			createinfo.props = 0;
			SDL_GPUTexture *tex = SDL_CreateGPUTexture(c_gpu->device, &createinfo);
			if (tex) {
				ecs_set(world, e, EgGpuTexture, {.object = tex});
			} else {
				ecs_err("Failed to create texture: %s", SDL_GetError());
				ecs_add(world, e, EgBaseError);
				ecs_enable(world, e, false);
			}
			ecs_trace("w: '%f', h: '%f'", c_rect->w, c_rect->h);
		}
		ecs_log_pop_(0);

	} // END FOR LOOP
	ecs_log_pop_(0);
	ecs_log_set_level(0);
}




void Observer_EgGpuTexture(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgShapesRectangle *r = ecs_field(it, EgShapesRectangle, 0); // self
	EgGpuTexture *t = ecs_field(it, EgGpuTexture, 1); // self
	EgGpuDevice *g = ecs_field(it, EgGpuDevice, 2); // shared
	for (int i = 0; i < it->count; ++i, ++r, ++t) {
		printf("Changing texture (%s) to size (%f %f)\n", ecs_get_name(world, it->entities[i]), r->w, r->h);
		SDL_ReleaseGPUTexture(g->device, t->object);
		SDL_GPUTextureCreateInfo createinfo = {0};
		createinfo.type = SDL_GPU_TEXTURETYPE_2D;
		createinfo.format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
		createinfo.width = r->w;
		createinfo.height = r->h;
		createinfo.layer_count_or_depth = 1;
		createinfo.num_levels = 1;
		createinfo.sample_count = 1;
		createinfo.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET;
		createinfo.props = 0;
		t->object = SDL_CreateGPUTexture(g->device, &createinfo);
	} // END FOR LOOP

}

