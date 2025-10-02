#include <stdlib.h>
#include <stdio.h>
#define FLECS_SCRIPT_MATH
#include <flecs.h>

static ecs_os_api_t os_api_default = {0};

void main_abort()
{
	exit(1);
}

void main_log(int32_t level, const char *file, int32_t line, const char *msg)
{
	os_api_default.log_(level, file, line, msg);
	switch (level) {
	case -3:
		printf("Break here\n");
		break;
	case -4:
		printf("Break here\n");
		break;
	}
}

typedef struct {
	int32_t fd;
} C1;

ECS_COMPONENT_DECLARE(C1);

ECS_ENTITY_DECLARE(Tag1);
ECS_ENTITY_DECLARE(Tag2);



static void System_test(ecs_iter_t *it)
{
	ecs_log_set_level(-1);
	ecs_world_t *world = it->world;
	C1 *parent = ecs_field(it, C1, 0); // shared, parent
	C1 *this = ecs_field(it, C1, 1); // self
	ecs_entity_t parent_entity = ecs_field_src(it, 0);
	printf("parent: %s\n", ecs_get_name(world, parent_entity));
	for (int i = 0; i < it->count; ++i) {
		// print name of entity
		ecs_entity_t e = it->entities[i];
		printf("%s\n", ecs_get_name(world, e));
	} // END FOR LOOP
	ecs_log_set_level(-1);
}

int main(int argc, char *argv[])
{
	ecs_os_set_api_defaults();
	os_api_default = ecs_os_get_api();
	ecs_os_api_t os_api = os_api_default;
	os_api.log_ = main_log;
	os_api.abort_ = main_abort;
	ecs_os_set_api(&os_api);

	ecs_world_t *world = ecs_init();
	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, FlecsDoc);
	ECS_IMPORT(world, FlecsStats);
	ECS_IMPORT(world, FlecsScriptMath);

	ECS_COMPONENT_DEFINE(world, C1);
	ECS_TAG_DEFINE(world, Tag1);
	ECS_TAG_DEFINE(world, Tag2);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	ecs_log_set_level(0);
	ecs_script_run_file(world, "config/script1.flecs");
	ecs_log_set_level(-1);

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "System_test", .add = ecs_ids(ecs_dependson(EcsOnValidate))}),
	.callback = System_test,
	.query.terms =
	{
	{.id = ecs_id(C1), .trav = EcsChildOf, .src.id = EcsUp},
	{.id = ecs_id(C1), .src.id = EcsSelf},
	{.id = Tag1},
	}});


	while (1) {
		ecs_progress(world, 0.0f);
	}

	return 0;
}