#pragma once
#include <flecs.h>

void ecsx_trace_system_iter(ecs_iter_t *it);

void ecsx_trace_path(ecs_world_t * world, ecs_entity_t e, char const * prefix);
void ecsx_trace_type(ecs_world_t * world, ecs_entity_t e, char const * prefix);
void ecsx_trace_ent(ecs_world_t * world, ecs_entity_t e, char const * prefix);