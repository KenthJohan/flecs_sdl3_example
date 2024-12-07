#include "EgMeshes.h"


#include <stdlib.h>
#include <stdio.h>
#include <EgSpatials.h>
#include <EgBase.h>


ECS_COMPONENT_DECLARE(EgMeshesMesh);

void System_EgMeshesMesh(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgMeshesMesh *m = ecs_field(it, EgMeshesMesh, 0);
	for (int i = 0; i < it->count; ++i, ++m) {
		ecs_entity_t e = it->entities[i];
		ecs_remove(world, e, EgBaseUpdate);
		ecs_vec_reset_t(NULL, &m->data, Position3);
	} // END FOR LOOP
}

void EgMeshesImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgMeshes);
	ecs_set_name_prefix(world, "EgMeshes");
	ECS_COMPONENT_DEFINE(world, EgMeshesMesh);


	ecs_struct(world,
	{.entity = ecs_id(EgMeshesMesh),
	.members = {
	{.name = "usage", .type = ecs_id(EgBaseVec)},
	}});
	

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgMeshesMesh", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgMeshesMesh,
	.query.terms = {
	{.id = ecs_id(EgMeshesMesh), .src.id = EcsSelf},
	{.id = EgBaseUpdate},
	{.id = EgBaseError, .oper = EcsNot}}});
}
