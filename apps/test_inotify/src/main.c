#include <stdlib.h>
#include <stdio.h>
#define FLECS_SCRIPT_MATH
#include <flecs.h>
#include <EgFs.h>
// include header for getcwd
#include <unistd.h>
#include <limits.h> // For PATH_MAX

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
	case -4:
		printf("Break here.\n");
		break;
	}
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
	printf("ecs_get_max_id: %ld\n", ecs_get_max_id(world));
	ecs_set_entity_range(world, 5000, 0); // Some modules uses entity below 5000 a a
	printf("ecs_get_max_id: %ld\n", ecs_get_max_id(world));

	// print current directory
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("Current working dir: %s\n", cwd);
	} else {
		perror("getcwd() error");
	}

	ECS_IMPORT(world, FlecsUnits);
	ECS_IMPORT(world, FlecsDoc);
	ECS_IMPORT(world, FlecsStats);
	ECS_IMPORT(world, FlecsScriptMath);
	ECS_IMPORT(world, EgFs);
	ECS_IMPORT(world, EgFsEpoll);
	ECS_IMPORT(world, EgFsInotify);
	ECS_IMPORT(world, EgFsSocket);

	ecs_set(world, EcsWorld, EcsRest, {.port = 0});
	printf("Remote: %s\n", "https://www.flecs.dev/explorer/?remote=true");

	ecs_log_set_level(0);
	// Managed scripts can be inspected and modified from the explorer.
	ecs_entity_t s1 = ecs_script_init(world, &(ecs_script_desc_t){.filename = "config/script1.flecs"});
	if (!s1) {
		ecs_err("failed to load script");
		return -1;
	}
	ecs_log_set_level(-1);

	while (1) {
		ecs_progress(world, 0.1f);
	}

	return 0;
}