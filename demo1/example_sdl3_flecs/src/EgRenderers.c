#include "EgRenderers.h"
#include "EgGpu.h"
#include <EgBase.h>
#include <EgWindows.h>


ECS_COMPONENT_DECLARE(EgRenderersContext);
ECS_COMPONENT_DECLARE(EgRenderersCreateInfo);



void System_Create(ecs_iter_t *it)
{
	EgRenderersCreateInfo *r = ecs_field(it, EgRenderersCreateInfo, 0);
	for (int i = 0; i < it->count; i++) {
		ecs_entity_t e = it->entities[i];
		ecs_set(it->world, e, EgRenderersContext, {0});
	}
}

void EgRenderersImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgRenderers);
	ECS_IMPORT(world, EgBase);
	ECS_IMPORT(world, EgWindows);
	ECS_IMPORT(world, EgGpu);
	ecs_set_name_prefix(world, "EgRenderers");
	ECS_COMPONENT_DEFINE(world, EgRenderersContext);
	ECS_COMPONENT_DEFINE(world, EgRenderersCreateInfo);

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_Create", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Create,
	.query.terms = {
	{.id = ecs_id(EgRenderersCreateInfo)},
	{.id = ecs_id(EgGpuDevice), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuPipeline), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuBufferVertex), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuTexture), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsInOut}, // Depth texture
	{.id = ecs_id(EgWindowsWindow), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgGpuWindow), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgRenderersContext), .oper = EcsNot}, // Adds this
	{.id = EgBaseError, .oper = EcsNot}}});

}
