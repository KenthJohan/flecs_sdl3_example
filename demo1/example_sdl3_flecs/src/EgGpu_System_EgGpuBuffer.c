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
	float x, y, z;                 /* 3D data. Vertex range -0.5..0.5 in all axes. Z -0.5 is near, 0.5 is far. */
	float red, green, blue, alpha; /* intensity 0 to 1 (alpha is always 1). */
} VertexData;

static const VertexData vertex_data[] = {
/* Front face. */
/* Bottom left */
{-0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0},  /* red */
{0.5, -0.5, -0.5, 0.0, 0.0, 1.0, 1.0},  /* blue */
{-0.5, -0.5, -0.5, 0.0, 1.0, 0.0, 1.0}, /* green */

/* Top right */
{-0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0}, /* red */
{0.5, 0.5, -0.5, 1.0, 1.0, 0.0, 1.0},  /* yellow */
{0.5, -0.5, -0.5, 0.0, 0.0, 1.0, 1.0}, /* blue */

/* Left face */
/* Bottom left */
{-0.5, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0},   /* white */
{-0.5, -0.5, -0.5, 0.0, 1.0, 0.0, 1.0}, /* green */
{-0.5, -0.5, 0.5, 0.0, 1.0, 1.0, 1.0},  /* cyan */

/* Top right */
{-0.5, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0},   /* white */
{-0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0},  /* red */
{-0.5, -0.5, -0.5, 0.0, 1.0, 0.0, 1.0}, /* green */

/* Top face */
/* Bottom left */
{-0.5, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0},  /* white */
{0.5, 0.5, -0.5, 1.0, 1.0, 0.0, 1.0},  /* yellow */
{-0.5, 0.5, -0.5, 1.0, 0.0, 0.0, 1.0}, /* red */

/* Top right */
{-0.5, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0}, /* white */
{0.5, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0},  /* black */
{0.5, 0.5, -0.5, 1.0, 1.0, 0.0, 1.0}, /* yellow */

/* Right face */
/* Bottom left */
{0.5, 0.5, -0.5, 1.0, 1.0, 0.0, 1.0},  /* yellow */
{0.5, -0.5, 0.5, 1.0, 0.0, 1.0, 1.0},  /* magenta */
{0.5, -0.5, -0.5, 0.0, 0.0, 1.0, 1.0}, /* blue */

/* Top right */
{0.5, 0.5, -0.5, 1.0, 1.0, 0.0, 1.0}, /* yellow */
{0.5, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0},  /* black */
{0.5, -0.5, 0.5, 1.0, 0.0, 1.0, 1.0}, /* magenta */

/* Back face */
/* Bottom left */
{0.5, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0},   /* black */
{-0.5, -0.5, 0.5, 0.0, 1.0, 1.0, 1.0}, /* cyan */
{0.5, -0.5, 0.5, 1.0, 0.0, 1.0, 1.0},  /* magenta */

/* Top right */
{0.5, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0},   /* black */
{-0.5, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0},  /* white */
{-0.5, -0.5, 0.5, 0.0, 1.0, 1.0, 1.0}, /* cyan */

/* Bottom face */
/* Bottom left */
{-0.5, -0.5, -0.5, 0.0, 1.0, 0.0, 1.0}, /* green */
{0.5, -0.5, 0.5, 1.0, 0.0, 1.0, 1.0},   /* magenta */
{-0.5, -0.5, 0.5, 0.0, 1.0, 1.0, 1.0},  /* cyan */

/* Top right */
{-0.5, -0.5, -0.5, 0.0, 1.0, 0.0, 1.0}, /* green */
{0.5, -0.5, -0.5, 0.0, 0.0, 1.0, 1.0},  /* blue */
{0.5, -0.5, 0.5, 1.0, 0.0, 1.0, 1.0}    /* magenta */
};

/*
#define SDL_GPU_BUFFERUSAGE_VERTEX                                  (1u << 0)
#define SDL_GPU_BUFFERUSAGE_INDEX                                   (1u << 1)
#define SDL_GPU_BUFFERUSAGE_INDIRECT                                (1u << 2)
#define SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ                   (1u << 3)
#define SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_READ                    (1u << 4)
#define SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE                   (1u << 5)

*/
void System_EgGpuBuffer_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgGpuDevice *c_gpu = ecs_field(it, EgGpuDevice, 0);                      // shared, parent
	EgGpuBufferCreateInfo *create = ecs_field(it, EgGpuBufferCreateInfo, 1); // self
	ecs_log_set_level(1);
	ecs_dbg("System_EgGpuBuffer_Create() count:%i", it->count);
	ecs_log_push_1();

	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		// Entities can be annotated with the Final trait, which prevents using them with IsA relationship.
		ecs_add_id(world, e, EcsFinal);
	}

	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		ecs_dbg("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_1();
		if (create[i].is_vertex) {
			SDL_GPUBufferCreateInfo desc = {0};
			desc.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
			desc.size = create[i].size;
			desc.props = 0;
			SDL_GPUBuffer *b = SDL_CreateGPUBuffer(c_gpu->device, &desc);
			if (b == NULL) {
				ecs_add(world, e, EgBaseError);
				continue;
			}
			char const *name = ecs_get_name(world, e);
			SDL_SetGPUBufferName(c_gpu->device, b, name);
			ecs_set(world, e, EgGpuBufferVertex, {.object = b, .size = desc.size});
		} else if (create[i].is_index) {
			SDL_GPUBufferCreateInfo desc = {0};
			desc.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
			desc.size = create[i].size;
			desc.props = 0;
			SDL_GPUBuffer *b = SDL_CreateGPUBuffer(c_gpu->device, &desc);
			if (b == NULL) {
				ecs_add(world, e, EgBaseError);
				continue;
			}
			char const *name = ecs_get_name(world, e);
			SDL_SetGPUBufferName(c_gpu->device, b, name);
			ecs_set(world, e, EgGpuBufferIndex, {.object = b, .size = desc.size});
		} else if (create[i].is_transfer) {
			SDL_GPUTransferBufferCreateInfo desc = {0};
			desc.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
			SDL_GPUTransferBuffer *b = SDL_CreateGPUTransferBuffer(c_gpu->device, &desc);
			if (b == NULL) {
				ecs_add(world, e, EgBaseError);
				continue;
			}
			ecs_set(world, e, EgGpuBufferTransfer, {.object = b, .size = desc.size});
		} else {
			ecs_err("Unknown buffer type");
			ecs_add(world, e, EgBaseError);
			continue;
		}
		ecs_log_pop_1();
	}

	ecs_log_pop_1();
	ecs_log_set_level(0);
}

typedef struct {
	float x, y, z, r, g, b, a;
} vertex_xyz_rgba;

/*
This system fills the GPU buffer with only vertex data.
TODO: We need to upload both vertex and index data to the GPU.
*/
void System_EgGpuBuffer_Fill(ecs_iter_t *it)
{
	ecs_log_set_level(1);
	ecs_dbg("System_EgGpuBuffer_Fill() count:%i", it->count);
	ecs_log_push_1();

	for (int i = 0; i < it->count; ++i) {
		ecs_remove(it->world, it->entities[i], EgBaseUpdate);
	}

	EgGpuDevice *d0 = ecs_field(it, EgGpuDevice, 0);                   // shared, parent
	EgGpuBufferVertex *b = ecs_field(it, EgGpuBufferVertex, 1);                    // self
	EgBaseVertexIndexVec *vi = ecs_field(it, EgBaseVertexIndexVec, 2); // self
	for (int i = 0; i < it->count; ++i, ++vi, ++b) {
		ecs_entity_t e = it->entities[i];
		ecs_dbg("Entity: '%s'", ecs_get_name(it->world, e));

		int32_t total = ecs_vec_count(&vi->vertices) * vi->stride_vertices;
		vertex_xyz_rgba const *data = ecs_vec_first(&vi->vertices);

		// int32_t total = sizeof(vertex_data);
		// vertex_xyz_rgba const *data = (vertex_xyz_rgba const *)vertex_data;

		if ((int32_t)b[i].size < total) {
			ecs_err("Buffer size is too small");
			ecs_add(it->world, e, EgBaseError);
			continue;
		}
		sdl_gpu_copy_simple1(d0->device, data, total, b->object);
	} // END FOR LOOP

	ecs_log_pop_1();
	ecs_log_set_level(0);
}
