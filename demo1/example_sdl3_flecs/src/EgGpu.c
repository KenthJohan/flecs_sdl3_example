#include "EgGpu.h"
#include <EgBase.h>
#include <EgShapes.h>
#include <EgWindows.h>
#include <SDL3/SDL_gpu.h>

#include "EgGpu_System_EgGpuShader.h"
#include "EgGpu_System_EgGpuPipeline.h"
#include "EgGpu_System_EgGpuDevice.h"
#include "EgGpu_System_EgGpuBuffer.h"
#include "EgGpu_System_EgGpuTexture.h"

ECS_TAG_DECLARE(EgGpuVertexFormat);
ECS_COMPONENT_DECLARE(EgGpuDevice);
ECS_COMPONENT_DECLARE(EgGpuWindow);
ECS_COMPONENT_DECLARE(EgGpuDeviceCreateInfo);
ECS_COMPONENT_DECLARE(EgGpuShaderVertex);
ECS_COMPONENT_DECLARE(EgGpuShaderVertexCreateInfo);
ECS_COMPONENT_DECLARE(EgGpuShaderFragment);
ECS_COMPONENT_DECLARE(EgGpuShaderFragmentCreateInfo);
ECS_COMPONENT_DECLARE(EgGpuPipeline);
ECS_COMPONENT_DECLARE(EgGpuPipelineCreateInfo);
ECS_COMPONENT_DECLARE(EgGpuDrawPrimitive);
ECS_COMPONENT_DECLARE(EgGpuBuffer);
ECS_COMPONENT_DECLARE(EgGpuBufferCreateInfo);
ECS_COMPONENT_DECLARE(EgGpuTexture);
ECS_COMPONENT_DECLARE(EgGpuTextureCreateInfo);
ECS_COMPONENT_DECLARE(EgGpuDraw1);
ECS_COMPONENT_DECLARE(EgGpuLocation);


void System_Claim(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgGpuDevice *c_gpu = ecs_field(it, EgGpuDevice, 0);         // up
	EgWindowsWindow *c_win = ecs_field(it, EgWindowsWindow, 1); // self
	ecs_log_set_level(1);
	ecs_dbg("System_Claim() count:%i", it->count);
	ecs_log_push_1();
	for (int i = 0; i < it->count; ++i, ++c_gpu) {
		ecs_entity_t e = it->entities[i];
		//printf("e=%s, e_win=%s\n", ecs_get_name(world, e), ecs_get_name(world, e_win1));
		bool success = SDL_ClaimWindowForGPUDevice(c_gpu->device, c_win->object);
		if (!success) {
			ecs_add_id(world, e, EgBaseError);
			ecs_err("SDL_ClaimWindowForGPUDevice() failed");
		} else {
			ecs_dbg("SDL_ClaimWindowForGPUDevice() success");
			ecs_add(world, e, EgGpuWindow);
		}
	} // END FOR LOOP
	ecs_log_pop_1();
	ecs_log_set_level(0);
}




void EgGpuImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgGpu);
	ECS_IMPORT(world, EgBase);
	ECS_IMPORT(world, EgShapes);
	ecs_set_name_prefix(world, "EgGpu");

	ECS_TAG_DEFINE(world, EgGpuVertexFormat);
	ECS_COMPONENT_DEFINE(world, EgGpuDevice);
	ECS_COMPONENT_DEFINE(world, EgGpuWindow);
	ECS_COMPONENT_DEFINE(world, EgGpuDeviceCreateInfo);
	ECS_COMPONENT_DEFINE(world, EgGpuShaderVertex);
	ECS_COMPONENT_DEFINE(world, EgGpuShaderVertexCreateInfo);
	ECS_COMPONENT_DEFINE(world, EgGpuShaderFragment);
	ECS_COMPONENT_DEFINE(world, EgGpuShaderFragmentCreateInfo);
	ECS_COMPONENT_DEFINE(world, EgGpuPipeline);
	ECS_COMPONENT_DEFINE(world, EgGpuPipelineCreateInfo);
	ECS_COMPONENT_DEFINE(world, EgGpuDrawPrimitive);
	ECS_COMPONENT_DEFINE(world, EgGpuBuffer);
	ECS_COMPONENT_DEFINE(world, EgGpuBufferCreateInfo);
	ECS_COMPONENT_DEFINE(world, EgGpuTexture);
	ECS_COMPONENT_DEFINE(world, EgGpuTextureCreateInfo);
	ECS_COMPONENT_DEFINE(world, EgGpuDraw1);
	ECS_COMPONENT_DEFINE(world, EgGpuLocation);

	ecs_struct(world,
	{.entity = ecs_id(EgGpuDeviceCreateInfo),
	.members = {
	{.name = "debug", .type = ecs_id(ecs_bool_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuDevice),
	.members = {
	{.name = "device", .type = ecs_id(ecs_uptr_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuShaderVertexCreateInfo),
	.members = {
	{.name = "stage", .type = ecs_id(ecs_u32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuShaderVertex),
	.members = {
	{.name = "device", .type = ecs_id(ecs_uptr_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuShaderFragmentCreateInfo),
	.members = {
	{.name = "stage", .type = ecs_id(ecs_u32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuShaderFragment),
	.members = {
	{.name = "device", .type = ecs_id(ecs_uptr_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuPipelineCreateInfo),
	.members = {
	{.name = "sample_count", .type = ecs_id(ecs_u32_t)},
	{.name = "target_info_has_depth_stencil_target", .type = ecs_id(ecs_bool_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuPipeline),
	.members = {
	{.name = "object", .type = ecs_id(ecs_uptr_t)}
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuBuffer),
	.members = {
	{.name = "object", .type = ecs_id(ecs_uptr_t)},
	{.name = "usage", .type = ecs_id(ecs_u32_t)},
	{.name = "size", .type = ecs_id(ecs_u32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuBufferCreateInfo),
	.members = {
	{.name = "usage", .type = ecs_id(ecs_u32_t)},
	{.name = "size", .type = ecs_id(ecs_u32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuTexture),
	.members = {
	{.name = "object", .type = ecs_id(ecs_uptr_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuTextureCreateInfo),
	.members = {
	{.name = "sample_count", .type = ecs_id(ecs_u32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuLocation),
	.members = {
	{.name = "location", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgGpuDrawPrimitive),
	.members = {
	{.name = "instance", .type = ecs_id(ecs_entity_t)},
	{.name = "num_vertices", .type = ecs_id(ecs_u32_t)},
	{.name = "num_instances", .type = ecs_id(ecs_u32_t)},
	{.name = "first_vertex", .type = ecs_id(ecs_u32_t)},
	{.name = "first_instance", .type = ecs_id(ecs_u32_t)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgGpuDevice_Create", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgGpuDevice_Create,
	.query.terms =
	{
	{.id = ecs_id(EgGpuDeviceCreateInfo), .src.id = EcsSelf},
	{.id = ecs_id(EgGpuDevice), .oper = EcsNot}, // Adds this
	{.id = EgBaseUpdate},
	{.id = EgBaseError, .oper = EcsNot}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgGpuShaderFragment_Create", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgGpuShaderFragment_Create,
	.query.terms =
	{
	{.id = ecs_id(EgGpuDevice), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuShaderFragmentCreateInfo), .src.id = EcsSelf},
	{.id = ecs_id(EgGpuShaderFragment), .oper = EcsNot}, // Adds this
	{.id = EgBaseUpdate},
	{.id = EgBaseError, .oper = EcsNot}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgGpuShaderVertex_Create", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgGpuShaderVertex_Create,
	.query.terms =
	{
	{.id = ecs_id(EgGpuDevice), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuShaderVertexCreateInfo), .src.id = EcsSelf},
	{.id = ecs_id(EgGpuShaderVertex), .oper = EcsNot}, // Adds this
	{.id = EgBaseUpdate},
	{.id = EgBaseError, .oper = EcsNot}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgGpuPipeline_Create", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgGpuPipeline_Create,
	.query.terms = {
	{.id = ecs_id(EgGpuDevice), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuPipelineCreateInfo), .src.id = EcsSelf},
	{.id = ecs_id(EgGpuShaderVertex), .trav = EcsDependsOn, .src.id = EcsUp},
	{.id = ecs_id(EgGpuShaderFragment), .trav = EcsDependsOn, .src.id = EcsUp},
	{.id = ecs_id(EcsComponent), .trav = EcsDependsOn, .src.id = EcsUp},
	{.id = ecs_id(EgGpuPipeline), .oper = EcsNot}, // Adds this
	{.id = EgBaseUpdate},
	{.id = EgBaseError, .oper = EcsNot}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgGpuBuffer_Create", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgGpuBuffer_Create,
	.query.terms = {
	{.id = ecs_id(EgGpuDevice), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuBufferCreateInfo), .src.id = EcsSelf},
	{.id = ecs_id(EgGpuBuffer), .oper = EcsNot}, // Adds this
	{.id = EgBaseError, .oper = EcsNot}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgGpuBuffer_Fill", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgGpuBuffer_Fill,
	.query.terms = {
	{.id = ecs_id(EgGpuDevice), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuBuffer)},
	{.id = ecs_id(EgBaseVertexIndexVec), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = EgBaseUpdate}, // Removes this
	{.id = EgBaseError, .oper = EcsNot}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgGpuTexture_Create", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgGpuTexture_Create,
	.query.terms = {
	{.id = ecs_id(EgGpuDevice), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgShapesRectangle), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuTextureCreateInfo), .src.id = EcsSelf},
	{.id = ecs_id(EgGpuTexture), .oper = EcsNot}, // Adds this
	{.id = EgBaseUpdate}, // Removes this
	{.id = EgBaseError, .oper = EcsNot}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgGpuTexture_Release", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgGpuTexture_Release,
	.query.terms = {
	{.id = ecs_id(EgGpuDevice), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuTexture)}, // Removes this
	{.id = EgBaseUpdate},
	{.id = EgBaseError, .oper = EcsNot}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_Claim", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Claim,
	.query.terms = {
	{.id = ecs_id(EgGpuDevice), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgWindowsWindow), .src.id = EcsSelf, .inout = EcsIn},
	{.id = ecs_id(EgGpuWindow), .oper = EcsNot},
	{.id = ecs_id(EgBaseError), .oper = EcsNot},
	}});


}
