#include "EgGpu_System_EgGpuBuffer.h"
#include "EgGpu.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <EgBase.h>

#include "sdl_gpu_copy.h"
#include "EgMeshes.h"

typedef struct VertexData {
	float x, y, z;          /* 3D data. Vertex range -0.5..0.5 in all axes. Z -0.5 is near, 0.5 is far. */
	float red, green, blue; /* intensity 0 to 1 (alpha is always 1). */
} VertexData;

static const VertexData vertex_data[] = {
/* Front face. */
/* Bottom left */
{-0.5, 0.5, -0.5, 1.0, 0.0, 0.0},  /* red */
{0.5, -0.5, -0.5, 0.0, 0.0, 1.0},  /* blue */
{-0.5, -0.5, -0.5, 0.0, 1.0, 0.0}, /* green */

/* Top right */
{-0.5, 0.5, -0.5, 1.0, 0.0, 0.0}, /* red */
{0.5, 0.5, -0.5, 1.0, 1.0, 0.0},  /* yellow */
{0.5, -0.5, -0.5, 0.0, 0.0, 1.0}, /* blue */

/* Left face */
/* Bottom left */
{-0.5, 0.5, 0.5, 1.0, 1.0, 1.0},   /* white */
{-0.5, -0.5, -0.5, 0.0, 1.0, 0.0}, /* green */
{-0.5, -0.5, 0.5, 0.0, 1.0, 1.0},  /* cyan */

/* Top right */
{-0.5, 0.5, 0.5, 1.0, 1.0, 1.0},   /* white */
{-0.5, 0.5, -0.5, 1.0, 0.0, 0.0},  /* red */
{-0.5, -0.5, -0.5, 0.0, 1.0, 0.0}, /* green */

/* Top face */
/* Bottom left */
{-0.5, 0.5, 0.5, 1.0, 1.0, 1.0},  /* white */
{0.5, 0.5, -0.5, 1.0, 1.0, 0.0},  /* yellow */
{-0.5, 0.5, -0.5, 1.0, 0.0, 0.0}, /* red */

/* Top right */
{-0.5, 0.5, 0.5, 1.0, 1.0, 1.0}, /* white */
{0.5, 0.5, 0.5, 0.0, 0.0, 0.0},  /* black */
{0.5, 0.5, -0.5, 1.0, 1.0, 0.0}, /* yellow */

/* Right face */
/* Bottom left */
{0.5, 0.5, -0.5, 1.0, 1.0, 0.0},  /* yellow */
{0.5, -0.5, 0.5, 1.0, 0.0, 1.0},  /* magenta */
{0.5, -0.5, -0.5, 0.0, 0.0, 1.0}, /* blue */

/* Top right */
{0.5, 0.5, -0.5, 1.0, 1.0, 0.0}, /* yellow */
{0.5, 0.5, 0.5, 0.0, 0.0, 0.0},  /* black */
{0.5, -0.5, 0.5, 1.0, 0.0, 1.0}, /* magenta */

/* Back face */
/* Bottom left */
{0.5, 0.5, 0.5, 0.0, 0.0, 0.0},   /* black */
{-0.5, -0.5, 0.5, 0.0, 1.0, 1.0}, /* cyan */
{0.5, -0.5, 0.5, 1.0, 0.0, 1.0},  /* magenta */

/* Top right */
{0.5, 0.5, 0.5, 0.0, 0.0, 0.0},   /* black */
{-0.5, 0.5, 0.5, 1.0, 1.0, 1.0},  /* white */
{-0.5, -0.5, 0.5, 0.0, 1.0, 1.0}, /* cyan */

/* Bottom face */
/* Bottom left */
{-0.5, -0.5, -0.5, 0.0, 1.0, 0.0}, /* green */
{0.5, -0.5, 0.5, 1.0, 0.0, 1.0},   /* magenta */
{-0.5, -0.5, 0.5, 0.0, 1.0, 1.0},  /* cyan */

/* Top right */
{-0.5, -0.5, -0.5, 0.0, 1.0, 0.0}, /* green */
{0.5, -0.5, -0.5, 0.0, 0.0, 1.0},  /* blue */
{0.5, -0.5, 0.5, 1.0, 0.0, 1.0}    /* magenta */
};

void System_EgGpuBuffer_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgGpuDevice *c_gpu = ecs_field(it, EgGpuDevice, 0);                      // shared
	EgGpuBufferCreateInfo *create = ecs_field(it, EgGpuBufferCreateInfo, 1); // self
	ecs_log_set_level(1);
	ecs_dbg("System_EgGpuBuffer_Create() count:%i", it->count);
	ecs_log_push_1();
	for (int i = 0; i < it->count; ++i, ++create) {
		ecs_entity_t e = it->entities[i];
		ecs_remove(world, e, EgBaseUpdate);
		ecs_dbg("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_1();
		{
			SDL_GPUBufferCreateInfo desc = {0};
			desc.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
			desc.size = sizeof(vertex_data);
			desc.props = 0;
			SDL_GPUBuffer *b = SDL_CreateGPUBuffer(c_gpu->device, &desc);
			if (b == NULL) {
				ecs_add(world, e, EgBaseError);
				continue;
			}
			sdl_gpu_copy_simple1(c_gpu->device, vertex_data, sizeof(vertex_data), b);

			char const *name = ecs_get_name(world, e);
			SDL_SetGPUBufferName(c_gpu->device, b, name);
			ecs_set(world, e, EgGpuBuffer, {.object = b, .usage = desc.usage, .size = desc.size});
		}
		ecs_log_pop_1();

	} // END FOR LOOP
	ecs_log_pop_1();
	ecs_log_set_level(0);
}

/*
void System_EgGpuBuffer_Append(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgGpuDevice *d = ecs_field(it, EgGpuDevice, 0); // shared, parent
	EgGpuBuffer *b = ecs_field(it, EgGpuBuffer, 1); // shared, parent
	EgMeshesMesh *m = ecs_field(it, EgMeshesMesh, 2); // self
	for (int i = 0; i < it->count; ++i) {

	} // END FOR LOOP
}
*/