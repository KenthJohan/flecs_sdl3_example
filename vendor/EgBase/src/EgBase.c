#include "EgBase.h"


ECS_TAG_DECLARE(EgBaseUpdate);
ECS_TAG_DECLARE(EgBaseError);

void EgBaseImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgBase);
	ecs_set_name_prefix(world, "EgBase");
	ECS_TAG_DEFINE(world, EgBaseUpdate);
	ECS_TAG_DEFINE(world, EgBaseError);

	ecs_add_id(world, EgBaseUpdate, EcsTraversable);
}