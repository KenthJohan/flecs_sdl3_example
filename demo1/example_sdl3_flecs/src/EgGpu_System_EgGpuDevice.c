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
	ecs_dbg("System Sampler_Create() count:%i", it->count);
	ecs_log_push_1();
	for (int i = 0; i < it->count; ++i, ++create) {
		ecs_entity_t e = it->entities[i];
		ecs_dbg("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_1();
		{
			SDL_GPUDevice *device = SDL_CreateGPUDevice(TESTGPU_SUPPORTED_FORMATS, true, NULL);
			if (device == NULL) {
				ecs_enable(world, e, false);
				continue;
			}
			ecs_set(world, e, EgGpuDevice, {.device = device});
			ecs_dbg("SDL_CreateGPUDevice() -> %p", device);
			char buf[128];
			snprintf(buf, 128, "%s", ecs_get_name(world, e));
			ecs_doc_set_name(world, e, buf);
			char const *driver = SDL_GetGPUDeviceDriver(device);
			SDL_GPUShaderFormat formats = SDL_GetGPUShaderFormats(device);
			snprintf(buf, 128, "Driver: %s, %08X", driver, formats);
			ecs_doc_set_brief(world, e, buf);
			//ecs_doc_set_color(world, e, color);
		}
		ecs_log_pop_1();

	} // END FOR LOOP
	ecs_log_pop_1();
	ecs_log_set_level(0);
}