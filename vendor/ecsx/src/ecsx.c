#include "ecsx.h"
#include <stdlib.h>

int ecsx_get_entities_by_query(ecs_world_t *world, ecs_query_t *query, ecs_entity_t entities[], int count)
{
	ecs_iter_t it = ecs_query_iter(world, query);
	int i = 0;
	while (ecs_query_next(&it)) {
		for (int j = 0; j < it.count; j++) {
			if (i >= count) {
				goto exit_return;
			}
			entities[i] = it.entities[j];
			i++;
		}
	}
exit_return:
	return i;
}

int ecsx_get_entities_from_parent(ecs_world_t *world, ecs_entity_t parent, ecs_id_t component, ecs_entity_t entities[], int count)
{
	ecs_query_t *q = ecs_query(world,
	{.terms = {
	 {ecs_childof(parent), .inout = EcsInOutFilter},
	 {component, .inout = EcsInOutFilter},
	 }});
	int k = ecsx_get_entities_by_query(world, q, entities, count);
	ecs_query_fini(q);
	return k;
}

bool ecsx_has_children(ecs_world_t *world, ecs_entity_t entity)
{
	ecs_iter_t it = ecs_children(world, entity);
	if (it.world == NULL) {
		return false;
	}
	bool r = ecs_iter_is_true(&it);
	ecs_iter_fini(&it);
	return r;
}


int32_t ecsx_sum_offset(ecs_world_t *world, ecs_entity_t const members[], ecs_entity_t *last)
{
	int32_t o = 0;
	int i = 0;
	while (members[i]) {
		EcsMember const *m = ecs_get(world, members[i], EcsMember);
		if (m == NULL) {
			break;
		}
		o += m->offset;
		(*last) = members[i];
		i++;
	}
	return o;
}


int32_t ecsx_children_count(ecs_world_t *world, ecs_entity_t parent)
{
	ecs_iter_t it = ecs_children(world, parent);
	if (it.world == NULL) {
		return 0;
	}
	int32_t count = 0;
	while (ecs_children_next(&it)) {
		count += it.count;
	}
	ecs_iter_fini(&it);
	return count;
}


