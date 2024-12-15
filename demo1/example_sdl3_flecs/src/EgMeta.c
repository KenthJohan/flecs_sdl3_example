#include "EgMeta.h"
#include <EgBase.h>

void EgMetaImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgMeta);
	ECS_IMPORT(world, EgBase);
	ecs_set_name_prefix(world, "EgMeta");


}