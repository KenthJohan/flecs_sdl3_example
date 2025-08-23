#include "EgGpu_System_EgGpuTransfer.h"
#include "EgGpu.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <EgBase.h>

#include "EgMeshes.h"

#include "test1.h"

/*
Work in progress.
The goal is to have ability to send commands from CPU flecs to GPU.
For example:
	* To send a command to copy data from CPU vertex buffer to GPU vertex buffer.
	* To send a command to copy data from CPU index buffer to GPU index buffer.
	* To send a command to copy data from CPU texture to GPU texture.
*/
void Runner_EgGpuTransfer(ecs_iter_t *it)
{
	ecs_log_set_level(0);
	//ecs_dbg("Runner_EgGpuTransfer() count:%i", it->count);
	ecs_log_push_(0);
	while (ecs_query_next(it)) {
		EgGpuDevice *d0 = ecs_field(it, EgGpuDevice, 0);                    // shared, parent
		EgGpuBufferTransfer *bt0 = ecs_field(it, EgGpuBufferTransfer, 1);   // shared, parent
		EgGpuBufferVertex *bv0 = ecs_field(it, EgGpuBufferVertex, 2);       // shared, dependent
		EgGpuBufferIndex *bi0 = ecs_field(it, EgGpuBufferIndex, 3);         // shared, dependent
		EgBaseVertexIndexVec *vi0 = ecs_field(it, EgBaseVertexIndexVec, 4); // shared, dependent

		for (int i = 0; i < it->count; ++i) {
			ecs_enable(it->world, it->entities[i], false);
		} // END FOR LOOP
	}
	ecs_log_pop_(0);
	ecs_log_set_level(0);
}
