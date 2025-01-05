
#include <flecs.h>

typedef struct
{
	int32_t dummy;
} SharedComponent;

typedef struct
{
	int32_t dummy;
} QueryCreator;

typedef struct {
	ecs_query_t *query;
} QueryRunner;

ECS_COMPONENT_DECLARE(SharedComponent);
ECS_COMPONENT_DECLARE(QueryCreator);
ECS_COMPONENT_DECLARE(QueryRunner);

void System_QueryCreator(ecs_iter_t *it)
{
	QueryCreator *c = ecs_field(it, QueryCreator, 0); // self
	for (int i = 0; i < it->count; ++i) {
		ecs_query_t *q = ecs_query(it->world,
		{.terms = {
		 {.id = ecs_id(SharedComponent), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsInOut}, // EcsInOut causes segfault
		 //{.id = ecs_id(SharedComponent), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn}, // EcsIn: It works
		 }});
		ecs_set(it->world, it->entities[i], QueryRunner, {.query = q});
	}
}

void System_QueryRunner(ecs_iter_t *it)
{
	ecs_log_set_level(1);
	QueryRunner *r = ecs_field(it, QueryRunner, 0); // self
	for (int i = 0; i < it->count; ++i) {
		ecs_iter_t it2 = ecs_query_iter(it->world, r[i].query);
		while (ecs_iter_next(&it2)) {
			for (int j = 0; j < it2.count; ++j) {
				ecs_dbg("System_QueryRunner %s", ecs_get_name(it->world, it2.entities[j]));
			}
			// Second iteration causes segfault
		}
	}
	ecs_log_set_level(0);
}

int main(int argc, char *argv[])
{
	// ecs_os_set_api_defaults();
	// ecs_os_api_t os_api = ecs_os_get_api();
	// ecs_os_set_api(&os_api);

	ecs_world_t *world = ecs_init();
	// ECS_IMPORT(world, FlecsUnits);
	// ECS_IMPORT(world, FlecsDoc);
	// ECS_IMPORT(world, FlecsStats);

	ECS_TAG(world, Load);
	ECS_COMPONENT_DEFINE(world, SharedComponent);
	ECS_COMPONENT_DEFINE(world, QueryRunner);
	ECS_COMPONENT_DEFINE(world, QueryCreator);
	ecs_add_id(world, Load, EcsTraversable);

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_QueryCreator", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_QueryCreator,
	.query.terms = {
	{.id = ecs_id(QueryCreator)},
	{.id = ecs_id(QueryRunner), .oper = EcsNot}, // Adds this
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_QueryRunner", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_QueryRunner,
	.query.terms = {
	{.id = ecs_id(QueryRunner)},
	}});

	ecs_entity_t query_creator = ecs_entity(world, {.name = "query_runner"});
	ecs_add(world, query_creator, QueryCreator);

	ecs_entity_t common = ecs_entity(world, {.name = "common"});
	ecs_add(world, common, SharedComponent);

	ecs_entity_t e0 = ecs_entity(world, {.name = "e0"});
	ecs_entity_t e1 = ecs_entity(world, {.name = "e1"});
	ecs_add_pair(world, e0, EcsChildOf, common);
	ecs_add_pair(world, e1, EcsChildOf, common);

	while (1) {
		ecs_progress(world, 0.0f);
	}

	return 0;
}
