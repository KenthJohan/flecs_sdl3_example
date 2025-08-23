#include "EgQueries.h"
#include <ecsx.h>


static void test_query(ecs_world_t *world, ecs_query_t *q, ecs_entity_t parent)
{
	ecs_log_push_(0);
	ecs_iter_t it = ecs_query_iter(world, q);
	if (parent) {
		ecs_iter_set_group(&it, parent);
	}
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			ecs_trace("name: %s", ecs_get_name(world, e));
		}
	}
	ecs_log_pop_(0);
}

static void SystemCreateGuiQuery(ecs_iter_t *it)
{
	ecs_log_set_level(-1);
	ecsx_trace_system_iter(it);
	ecs_world_t *world = it->world;
	ecs_log_push_(0);
	EcsDocDescription *d = ecs_field(it, EcsDocDescription, 0);
	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		char *expr = d[i].value;
		if (expr == NULL) {
			continue;
		}
		ecsx_trace_ent(world, e, "");
		ecs_log_push_(0);
		ecs_trace("ecs_query_init expr: '%s'", expr);
		ecs_query_t *q = ecs_query_init(world,
		&(ecs_query_desc_t){
		.entity = e,
		//.cache_kind = EcsQueryCacheNone,
		.expr = expr,
		.group_by = EcsChildOf});
		if (q == NULL) {
			ecs_err("Failed to create query");
			ecs_enable(world, e, false);
			continue;
		}
		test_query(world, q, 0);
		ecs_log_pop_(0);
	}
	ecs_log_pop_(0);
	ecs_log_set_level(0);
}

void EgQueriesImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgQueries);
	ecs_set_name_prefix(world, "EgQueries");

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "CreateGuiQuery", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = SystemCreateGuiQuery,
	.immediate = true,
	.query.terms = {
	{.id = ecs_pair(ecs_id(EcsDocDescription), EcsQuery)},
	{.id = ecs_pair(ecs_id(EcsPoly), EcsQuery), .oper = EcsNot},
	{.id = EcsPrefab, .oper = EcsOptional},
	}});

}
