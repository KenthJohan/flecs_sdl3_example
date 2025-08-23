#pragma once
#include <flecs.h>

void ecsx_reparent_by_subname(ecs_world_t *world, char const *filters[], ecs_query_t *q, const ecs_id_t *add);
void ecsx_reparent_by_subname1(ecs_world_t *world, char const *filters[], ecs_entity_t component, const ecs_id_t *add);