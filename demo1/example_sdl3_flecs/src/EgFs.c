#include "EgFs.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_filesystem.h>

#include <ecsx.h>
#include <egmisc.h>

ECS_TAG_DECLARE(EgFsDir);
ECS_TAG_DECLARE(EgFsFile);
ECS_TAG_DECLARE(EgFsUpdate);


typedef struct {
	ecs_world_t *world;
	ecs_entity_t entity;
} ecs_world_entity_t;

static SDL_EnumerationResult SDLCALL enum_callback(ecs_world_entity_t *we, const char *dirname, const char *fname)
{
	ecs_world_t *world = we->world;
	ecs_entity_t root = we->entity;
	char buf[1024];
	snprintf(buf, 1024, "%s%s", dirname, fname);
	ecs_entity_t e = ecs_entity_init(world,
	&(ecs_entity_desc_t){
	.name = NULL,
	.parent = root,
	});
	ecs_doc_set_name(world, e, buf);
	SDL_PathInfo info;
	if (!SDL_GetPathInfo(buf, &info)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't stat '%s': %s", buf, SDL_GetError());
		return SDL_ENUM_CONTINUE;
	}
	if (info.type == SDL_PATHTYPE_FILE) {
		ecs_doc_set_color(world, e, "#358844FF");
		ecs_doc_set_brief(world, e, "File");
		ecs_add_id(world, e, EgFsFile);
	} else if (info.type == SDL_PATHTYPE_DIRECTORY) {
		ecs_doc_set_color(world, e, "#888811FF");
		ecs_doc_set_brief(world, e, "Folder");
		ecs_add_id(world, e, EgFsDir);
		ecs_add_id(world, e, EgFsUpdate);
	} else {
		ecs_doc_set_color(world, e, "#880000FF");
		ecs_doc_set_brief(world, e, "Other");
	}
	return SDL_ENUM_CONTINUE;
}

static void System_enumerate_directory(ecs_iter_t *it)
{
	ecs_log_set_level(0);
	ecs_world_t *world = it->world;
	ecsx_trace_system_iter(it);
	ecs_log_push_(0);
	EcsDocDescription *d = ecs_field(it, EcsDocDescription, 0);
	for (int i = 0; i < it->count; ++i, ++d) {
		ecs_entity_t e = it->entities[i];
		char const *name = ecs_get_name(world, e);

		ecs_remove_id(world, e, EgFsUpdate);

		if (d->value == NULL) {
			ecs_err("No path set for directory entity '%s'", name);
			continue;
		}
		ecs_world_entity_t we = {.world = world, .entity = e};
		ecs_trace("Enumerate directory '%s' ('%s')", d->value, name);
		bool success = SDL_EnumerateDirectory(d->value, (SDL_EnumerateDirectoryCallback)enum_callback, &we);

		if (!success) {
			ecs_err("Base path enumeration failed!");
		}
	}
	ecs_log_pop_(0);
	ecs_log_set_level(0);
}

void EgFsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgFs);
	ecs_set_name_prefix(world, "EgFs");

	ECS_TAG_DEFINE(world, EgFsDir);
	ECS_TAG_DEFINE(world, EgFsFile);
	ECS_TAG_DEFINE(world, EgFsUpdate);

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "System_enumerate_directory", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_enumerate_directory,
	.immediate = true,
	.query.terms = {
	{.id = ecs_pair(ecs_id(EcsDocDescription), EcsName)},
	{.id = EgFsDir},
	{.id = EgFsUpdate},
	}});
}
