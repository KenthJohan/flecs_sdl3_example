#include "ecsx/ecsx_reparent.h"
#include <ctype.h>
#include <stdio.h>

/*
Hej1, Hej => true, a_endptr[0] = 1
Heja, Hej => true, a_endptr[0] = a
Heja, wej => false
*/
static bool str_cmp_sub0(char const *a, char const *b, char **a_endptr)
{
	/*
	if (strcmp(a, "TIM1_BK") == 0) {
		printf("strcmp(a, \"TIM17\") == 0\n");
	}
	*/
	while (*a && *b) {
		if (isdigit(*a) && (*b) == '*') {
			while (*a && isdigit(*a)) {
				a++;
			}
			break;
		} else if (isalpha(*a) && (*b) == '#') {
			while (*a && isalpha(*a)) {
				a++;
			}
			break;
		} else if (*a == *b) {
			b++;
		} else {
			if (a_endptr) {
				*a_endptr = (char *)a;
			}
			return false;
		}
		a++;
	}
	if (a_endptr) {
		*a_endptr = (char *)a;
	}
	return true;
}

static int str_cmp_sub0v(char const *a, char const *bv[], char **a_endptr)
{
	for (int i = 0; bv[i]; i++) {
		if (str_cmp_sub0(a, bv[i], a_endptr)) {
			return i;
		}
	}
	return -1;
}

void ecsx_reparent_by_subname(ecs_world_t *world, char const *filters[], ecs_query_t *q, const ecs_id_t *add)
{
	ecs_defer_begin(world);
	ecs_iter_t it = ecs_query_iter(world, q);
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			char const *name = ecs_get_name(world, it.entities[i]);
			if (name == NULL) {
				continue;
			}
			// Check if name matches any of the filters
			// If it does, we need to reparent the entity
			char *name_endptr = NULL;
			int j = str_cmp_sub0v(name, filters, &name_endptr);
			if (j < 0) {
				continue;
			}
			// Setup a name for a new child parent
			char namebuf[64] = {0};
			int size = name_endptr - name;
			if (size < 0) {
				ecs_err("name too short: %s", name);
				continue;
			} else if (size >= (intptr_t)sizeof(namebuf)) {
				ecs_err("name too long: %s", name);
				continue;
			}
			memcpy(namebuf, name, size);
			namebuf[size] = 0;
			// Change the child parant to grandparent
			// Creata a new parent entity and insert between grandparent and child
			ecs_entity_t grandparent = ecs_get_parent(world, it.entities[i]);
			ecs_entity_t parent = ecs_entity_init(world,
			&(ecs_entity_desc_t){
			.name = namebuf,
			.parent = grandparent,
			.add = add,
			});
			ecs_add_pair(world, it.entities[i], EcsChildOf, parent);
		}
	}
	ecs_defer_end(world);
}

void ecsx_reparent_by_subname1(ecs_world_t *world, char const *names[], ecs_entity_t component, const ecs_id_t *add)
{
	//str_cmp_sub0("TIM17", "TIM*", NULL);
	ecs_query_t *q = ecs_query_init(world,
	&(ecs_query_desc_t){
	.terms = {
	{.id = component},
	}});
	ecsx_reparent_by_subname(world, names, q, add);
	ecs_query_fini(q);
}