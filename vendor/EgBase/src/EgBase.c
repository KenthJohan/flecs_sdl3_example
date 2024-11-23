#include "EgBase.h"

ECS_COMPONENT_DECLARE(EgBaseMemoryGPU);
ECS_COMPONENT_DECLARE(EgBaseShapeBuffer);
ECS_COMPONENT_DECLARE(EgBaseShowDrawReference);
ECS_COMPONENT_DECLARE(EgBaseMemory);
ECS_COMPONENT_DECLARE(EgBaseMemory2);

ECS_TAG_DECLARE(EgBaseUse);
ECS_TAG_DECLARE(EgBaseUpdate);
ECS_TAG_DECLARE(EgBaseDraw);
ECS_TAG_DECLARE(EgBaseError);

ECS_CTOR(EgBaseShapeBuffer, ptr, {
	ecs_os_memset_t(ptr, 0, EgBaseShapeBuffer);
})

void EgBaseImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgBase);

	ecs_set_name_prefix(world, "EgBase");

	ECS_COMPONENT_DEFINE(world, EgBaseMemoryGPU);
	ECS_COMPONENT_DEFINE(world, EgBaseShapeBuffer);
	ECS_COMPONENT_DEFINE(world, EgBaseShowDrawReference);
	ECS_COMPONENT_DEFINE(world, EgBaseMemory);
	ECS_COMPONENT_DEFINE(world, EgBaseMemory2);
	ECS_TAG_DEFINE(world, EgBaseUse);
	ECS_TAG_DEFINE(world, EgBaseUpdate);
	ECS_TAG_DEFINE(world, EgBaseDraw);
	ECS_TAG_DEFINE(world, EgBaseError);

	ecs_add_id(world, EgBaseUse, EcsTraversable);
	ecs_add_id(world, EgBaseUpdate, EcsTraversable);

	ecs_set_hooks(world, EgBaseShapeBuffer, {.ctor = ecs_ctor(EgBaseShapeBuffer)});

	ecs_struct(world,
	{.entity = ecs_id(EgBaseMemoryGPU),
	.members = {
	{.name = "id", .type = ecs_id(ecs_u32_t)},
	{.name = "cap", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgBaseShapeBuffer),
	.members = {
	//{.name = "mem", .type = ecs_id(EgBaseMemory), .count = 2},
	{.name = "vbuf", .type = ecs_id(EgBaseMemoryGPU)},
	{.name = "ibuf", .type = ecs_id(EgBaseMemoryGPU)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgBaseMemory),
	.members = {
	{.name = "ptr", .type = ecs_id(ecs_uptr_t)},
	{.name = "cap", .type = ecs_id(ecs_i32_t)},
	{.name = "size", .type = ecs_id(ecs_i32_t)},
	{.name = "last", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgBaseMemory2),
	.members = {
	//{.name = "mem", .type = ecs_id(EgBaseMemory), .count = 2},
	{.name = "mem0", .type = ecs_id(EgBaseMemory)},
	{.name = "mem1", .type = ecs_id(EgBaseMemory)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgBaseShowDrawReference),
	.members = {
	{.name = "flags", .type = ecs_id(ecs_u32_t)},
	}});
}