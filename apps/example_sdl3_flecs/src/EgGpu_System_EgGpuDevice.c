#include "EgGpu_System_EgGpuPipeline.h"
#include "EgGpu.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <EgBase.h>

#include "shader_spirv.h"

#define TESTGPU_SUPPORTED_FORMATS (SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXBC | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_METALLIB)

void System_EgGpuDevice_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgGpuDeviceCreateInfo *create = ecs_field(it, EgGpuDeviceCreateInfo, 0);

	ecs_log_set_level(1);
	ecs_trace("System Sampler_Create() count:%i", it->count);
	ecs_log_push_(0);
	for (int i = 0; i < it->count; ++i, ++create) {
		ecs_entity_t e = it->entities[i];
		ecs_trace("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_(0);
		{
			SDL_GPUDevice *device = SDL_CreateGPUDevice(TESTGPU_SUPPORTED_FORMATS, true, NULL);
			if (device == NULL) {
				ecs_enable(world, e, false);
				continue;
			}
			ecs_set(world, e, EgGpuDevice,
			{
			.device = device,
			});
			ecs_trace("SDL_CreateGPUDevice() -> %p", device);
			char buf[128];
			snprintf(buf, 128, "%s", ecs_get_name(world, e));
			ecs_doc_set_name(world, e, buf);
			char const *driver = SDL_GetGPUDeviceDriver(device);
			SDL_GPUShaderFormat formats = SDL_GetGPUShaderFormats(device);
			// ecs_doc_set_color(world, e, color);
			snprintf(buf, 128, "Driver: %s, %08X", driver, formats);
			ecs_doc_set_brief(world, e, buf);
			// Entities can be annotated with the Final trait, which prevents using them with IsA relationship.
			ecs_add_id(world, e, EcsFinal);
		}
		ecs_log_pop_(0);

	} // END FOR LOOP
	ecs_log_pop_(0);
	ecs_log_set_level(0);
}