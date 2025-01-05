
#include <flecs.h>

typedef struct {
	double x;
	double y;
} Position;

typedef struct {
	double x;
	double y;
} Velocity;

int main(int argc, char *argv[])
{
	ecs_os_set_api_defaults();
	ecs_os_api_t os_api = ecs_os_get_api();
	ecs_os_set_api(&os_api);

	ecs_world_t *world = ecs_init();
	// ECS_IMPORT(world, FlecsUnits);
	// ECS_IMPORT(world, FlecsDoc);
	// ECS_IMPORT(world, FlecsStats);


	ECS_TAG(world, Load);
	ECS_COMPONENT(world, Position);
	ECS_COMPONENT(world, Velocity);
	ecs_add_id(world, Load, EcsTraversable);


	ecs_entity_t sun = ecs_entity(world, {.name = "Sun"});
	ecs_set(world, sun, Position, {0, 0});
	ecs_entity_t earth = ecs_entity(world, {.name = "Earth"});
	ecs_set(world, earth, Position, {0, 0});
	ecs_add_pair(world, earth, EcsChildOf, sun);
	ecs_add_pair(world, earth, Load, sun);


	ecs_query_t *q = ecs_query(world,
	{.cache_kind = EcsQueryCacheAll,
		.terms = {
	 {.id = ecs_id(Position), .inout = EcsIn},
	 {.id = ecs_id(Position), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsInOut},
	 {.id = ecs_id(Position), .trav = Load, .src.id = EcsUp, .inout = EcsInOut},
	 }});



	ecs_iter_t it = ecs_query_iter(world, q);
	while (ecs_query_next(&it)) {
		ecs_dbg("iterating");
	}

	while (1) {
		ecs_progress(world, 0.0f);
	}

	return 0;
}
