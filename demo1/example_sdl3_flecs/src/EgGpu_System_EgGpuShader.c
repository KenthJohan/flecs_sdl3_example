#include "EgGpu_System_EgGpuShader.h"
#include "EgGpu.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <EgBase.h>

#include "shader_spirv.h"


void System_EgGpuShaderFragment_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgGpuDevice *gpu = ecs_field(it, EgGpuDevice, 0);
	EgGpuShaderFragmentCreateInfo *create = ecs_field(it, EgGpuShaderFragmentCreateInfo, 1);
	ecs_log_set_level(1);
	ecs_dbg("System Sampler_Create() count:%i", it->count);
	ecs_log_push_1();
	for (int i = 0; i < it->count; ++i, ++create) {
		ecs_entity_t e = it->entities[i];
		ecs_remove(world, e, EgBaseUpdate);

		ecs_dbg("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_1();
		{
			//SDL_GPUShaderStage stage = create->stage;
			SDL_GPUShader *s = shader_spirv_compile(gpu->device, "shaders/cube", SDL_GPU_SHADERSTAGE_FRAGMENT);
			if (s == NULL) {
				ecs_add(world, e, EgBaseError);
				continue;
			}
			ecs_set(world, e, EgGpuShaderFragment, {.object = s});
			ecs_dbg("shader_spirv_compile() -> %p", s);
			//ecs_enable(world, e, false);
		}
		ecs_log_pop_1();

	} // END FOR LOOP
	ecs_log_pop_1();
	ecs_log_set_level(0);
}

void System_EgGpuShaderVertex_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgGpuDevice *gpu = ecs_field(it, EgGpuDevice, 0);
	EgGpuShaderVertexCreateInfo *create = ecs_field(it, EgGpuShaderVertexCreateInfo, 1);
	ecs_log_set_level(1);
	ecs_dbg("System Sampler_Create() count:%i", it->count);
	ecs_log_push_1();
	for (int i = 0; i < it->count; ++i, ++create) {
		ecs_entity_t e = it->entities[i];
		ecs_remove(world, e, EgBaseUpdate);

		ecs_dbg("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_1();
		{
			//SDL_GPUShaderStage stage = create->stage;
			SDL_GPUShader *s = shader_spirv_compile(gpu->device, "shaders/cube", SDL_GPU_SHADERSTAGE_VERTEX);
			if (s == NULL) {
				ecs_add(world, e, EgBaseError);
				continue;
			}
			ecs_set(world, e, EgGpuShaderVertex, {.object = s});
			ecs_dbg("shader_spirv_compile() -> %p", s);
			//ecs_enable(world, e, false);
		}
		ecs_log_pop_1();

	} // END FOR LOOP
	ecs_log_pop_1();
	ecs_log_set_level(0);
}