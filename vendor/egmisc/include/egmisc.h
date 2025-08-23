#pragma once

#include "egmisc/eg_assert.h"
#include "egmisc/eg_file.h"
#include "egmisc/eg_log.h"
#include "egmisc/eg_popen.h"
#include "egmisc/eg_str.h"


int ecsx_get_entities_by_query(ecs_world_t *world, ecs_query_t *query, ecs_entity_t entities[], int count);
int ecsx_get_entities_from_parent(ecs_world_t *world, ecs_entity_t parent, ecs_id_t component, ecs_entity_t entities[], int count);

bool ecsx_has_children(ecs_world_t *world, ecs_entity_t entity);

int32_t ecsx_sum_offset(ecs_world_t *world, ecs_entity_t const members[], ecs_entity_t *last);

int32_t ecsx_children_count(ecs_world_t *world, ecs_entity_t parent);

