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
	ecs_dbg("System_EgGpuTexture_Create() count:%i", it->count);
	ecs_log_push_1();

	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		ecs_remove(world, e, EgBaseUpdate);
		// Entities can be annotated with the Final trait, which prevents using them with IsA relationship.
		ecs_add_id(world, e, EcsFinal);
	}

	EgGpuDevice *c_gpu = ecs_field(it, EgGpuDevice, 0);                          // parent
	EgShapesRectangle *c_rect = ecs_field(it, EgShapesRectangle, 1);             // parent
	EgGpuTextureCreateInfo *c_create = ecs_field(it, EgGpuTextureCreateInfo, 2); // self
	for (int i = 0; i < it->count; ++i, ++c_create) {
		ecs_entity_t e = it->entities[i];
		ecs_dbg("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_1();
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
			}
			ecs_dbg("w: '%f', h: '%f'", c_rect->w, c_rect->h);
		}
		ecs_log_pop_1();

	} // END FOR LOOP
	ecs_log_pop_1();
	ecs_log_set_level(0);
}

void System_EgGpuTexture_Release(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgGpuDevice *c_gpu = ecs_field(it, EgGpuDevice, 0);   // parent
	EgGpuTexture *c_tex = ecs_field(it, EgGpuTexture, 1); // self

	ecs_log_set_level(1);
	ecs_dbg("System_EgGpuTexture_Release() count:%i", it->count);
	ecs_log_push_1();
	for (int i = 0; i < it->count; ++i, ++c_tex) {
		ecs_entity_t e = it->entities[i];
		ecs_dbg("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_1();
		if (c_tex->object == NULL) {
			continue;
		}
		SDL_ReleaseGPUTexture(c_gpu->device, c_tex->object);
		c_tex->object = NULL;
		ecs_remove(world, e, EgGpuTexture);
		ecs_log_pop_1();

	} // END FOR LOOP
	ecs_log_pop_1();
	ecs_log_set_level(0);
}
