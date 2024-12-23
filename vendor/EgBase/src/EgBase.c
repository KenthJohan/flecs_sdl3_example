#include "EgBase.h"


ECS_COMPONENT_DECLARE(EgBaseVec);
ECS_TAG_DECLARE(EgBaseUpdate);
ECS_TAG_DECLARE(EgBaseError);

void EgBaseImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgBase);
	ecs_set_name_prefix(world, "EgBase");
	ECS_COMPONENT_DEFINE(world, EgBaseVec);
	ECS_TAG_DEFINE(world, EgBaseUpdate);
	ECS_TAG_DEFINE(world, EgBaseError);

	ecs_add_id(world, EgBaseUpdate, EcsTraversable);

	/*
	typedef struct ecs_vec_t {
		void *array;
		int32_t count;
		int32_t size;
	#ifdef FLECS_SANITIZE
		ecs_size_t elem_size;
	#endif
	} ecs_vec_t;
	*/
	ecs_struct(world,
	{.entity = ecs_id(EgBaseVec),
	.members = {
	{.name = "array", .type = ecs_id(ecs_uptr_t)},
	{.name = "count", .type = ecs_id(ecs_i32_t)},
	{.name = "size", .type = ecs_id(ecs_i32_t)},
#ifdef FLECS_SANITIZE
	{.name = "elem_size", .type = ecs_id(ecs_i32_t)}, //ecs_size_t
#endif
	}});



}

