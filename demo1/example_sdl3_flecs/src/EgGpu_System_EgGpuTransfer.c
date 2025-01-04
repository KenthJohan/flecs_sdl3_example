#include "EgGpu_System_EgGpuTransfer.h"
#include "EgGpu.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <EgBase.h>

#include "EgMeshes.h"

#include "test1.h"

void System_EgGpuTransfer(ecs_iter_t *it)
{
	EgGpuTransferCreateInfo *c = ecs_field(it, EgGpuTransferCreateInfo, 0);
	for (int i = 0; i < it->count; ++i) {
		ecs_query_t *q = ecs_query(it->world,
		{.terms = {
		 {.id = ecs_id(EgGpuDevice), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
		 {.id = ecs_id(EgGpuTransfer), .src.id = it->entities[i], .inout = EcsIn},
		 {.id = ecs_id(EgGpuBufferVertex), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
		 {.id = ecs_id(EgGpuBufferIndex), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
		 {.id = ecs_id(EgBaseVertexIndexVec), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
		 }});
		ecs_set(it->world, it->entities[i], EgGpuTransfer, {.query1 = q});
	} // END FOR LOOP
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
