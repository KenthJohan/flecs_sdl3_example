#include "EgGpu_System_EgGpuTransfer.h"
#include "EgGpu.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <EgBase.h>

#include "EgMeshes.h"

#include "test1.h"

void System_EgGpuTransferCreateInfo(ecs_iter_t *it)
{
	EgGpuTransferCreateInfo *c = ecs_field(it, EgGpuTransferCreateInfo, 0);
	for (int i = 0; i < it->count; ++i) {
		ecs_add_id(it->world, it->entities[i], it->entities[i]);
		ecs_query_t *q = ecs_query(it->world,
		{.cache_kind = EcsQueryCacheAll,
			.terms = {

		 {.id = ecs_id(EgGpuDevice), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsInOut},
		 //{.id = ecs_id(EgGpuTransfer), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
		 //{.id = ecs_id(EgGpuBufferTransfer), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
		 //{.id = ecs_id(EgGpuBufferVertex), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
		 //{.id = ecs_id(EgGpuBufferIndex), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
		 //{.id = ecs_id(EgBaseVertexIndexVec), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsInOut}, // This causes segfault
		 //{.id = it->entities[i], .trav = EcsChildOf, .src.id = EcsUp},
		 }});
		ecs_set(it->world, it->entities[i], EgGpuTransfer, {.query1 = q});
	} // END FOR LOOP
}

void System_EgGpuTransfer2(ecs_iter_t *it)
{
	EgGpuDevice *d0 = ecs_field(it, EgGpuDevice, 0);                    // shared, parent
	EgGpuTransfer *t0 = ecs_field(it, EgGpuTransfer, 1);                // shared, parent
	EgGpuBufferTransfer *bt0 = ecs_field(it, EgGpuBufferTransfer, 2);   // shared, parent
	EgGpuBufferVertex *bv0 = ecs_field(it, EgGpuBufferVertex, 3);       // shared, dependent
	EgGpuBufferIndex *bi0 = ecs_field(it, EgGpuBufferIndex, 4);         // shared, dependent
	EgBaseVertexIndexVec *vi0 = ecs_field(it, EgBaseVertexIndexVec, 5); // shared, dependent

	void *src = ecs_vec_first(&vi0->vertices);
	int src_size = ecs_vec_count(&vi0->vertices) * vi0->stride_vertices;
	// gpu_transfer_add(t0, d0->device, bt0, src, src_size, bt0->object, bv0->last);
	bv0->last += src_size;

	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		ecs_dbg("System_EgGpuTransfer2: '%s'", ecs_get_name(it->world, e));
		// ecs_enable(it->world, e, false);
	}
}

void System_EgGpuTransfer(ecs_iter_t *it)
{
	ecs_log_set_level(1);
	EgGpuDevice *d0 = ecs_field(it, EgGpuDevice, 0);                  // shared, parent
	EgGpuBufferTransfer *bt0 = ecs_field(it, EgGpuBufferTransfer, 1); // shared, parent
	EgGpuTransfer *c = ecs_field(it, EgGpuTransfer, 2);               // self
	for (int i = 0; i < it->count; ++i, ++c) {
		/*
		bool exist = ecs_query_is_true(c->query1);
		if (!exist) {
		    // ecs_dbg("System_EgGpuTransfer: query1 is false");
		    continue;
		}
		*/
		// gpu_transfer_begin(c, d0->device, bt0);
		ecs_iter_t it2 = ecs_query_iter(it->world, c->query1);
		while (ecs_iter_next(&it2)) {
			// System_EgGpuTransfer2(&it2);
		}
		// gpu_transfer_end(c, d0->device, bt0);
		ecs_dbg("gpu_transfer_end");
	} // END FOR LOOP
	ecs_log_set_level(0);
}

void System_EgGpuTransfer_Append(ecs_iter_t *it)
{
	ecs_log_set_level(1);
	ecs_dbg("System_EgGpuTransfer_Append() count:%i", it->count);
	ecs_log_push_1();

	EgGpuDevice *d0 = ecs_field(it, EgGpuDevice, 0);                    // shared, parent
	EgGpuBufferTransfer *bt0 = ecs_field(it, EgGpuBufferTransfer, 0);   // shared, parent
	EgGpuBufferVertex *bv0 = ecs_field(it, EgGpuBufferVertex, 1);       // shared, dependent
	EgGpuBufferIndex *bi0 = ecs_field(it, EgGpuBufferIndex, 2);         // shared, dependent
	EgBaseVertexIndexVec *vi0 = ecs_field(it, EgBaseVertexIndexVec, 3); // shared, dependent

	for (int i = 0; i < it->count; ++i) {
		ecs_enable(it->world, it->entities[i], false);
	} // END FOR LOOP

	ecs_log_pop_1();
	ecs_log_set_level(0);
}
