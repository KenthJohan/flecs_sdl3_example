#include "EgFs.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_filesystem.h>

static void str_replace_ab(char *str, char a, char b)
{
	char *p = str;
	while (p[0]) {
		if (p[0] == a) {
			p[0] = b;
		}
		p++;
	}
}

static SDL_EnumerationResult SDLCALL enum_callback(ecs_world_t *world, const char *origdir, const char *fname)
{
	SDL_PathInfo info;
	char *fullpath = NULL;

/* you can use '/' for a path separator on Windows, but to make the log output look correct, we'll #ifdef this... */
#ifdef SDL_PLATFORM_WINDOWS
	const char *pathsep = "\\";
#else
	const char *pathsep = "/";
#endif

	if (SDL_asprintf(&fullpath, "%s%s%s", origdir, *origdir ? pathsep : "", fname) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Out of memory!");
		return SDL_ENUM_FAILURE;
	}

	if (!SDL_GetPathInfo(fullpath, &info)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't stat '%s': %s", fullpath, SDL_GetError());
		SDL_free(fullpath);
		return SDL_ENUM_CONTINUE;
	}
	if (fullpath[0] != '.') {
		SDL_free(fullpath);
		return SDL_ENUM_CONTINUE;
	}

	char buf[1024];
	snprintf(buf, 1024, "%s", fullpath);
	str_replace_ab(buf, '.', 'X'); // Fix this
	str_replace_ab(buf, '-', 'Y'); // Fix this
	printf("File: %s\n", buf);
	ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){.name = buf, .sep = "/"});
	char * rel = strrchr(fullpath, '/');
	if (rel) {
		ecs_doc_set_name(world, e, rel + 1);
	} else {
		ecs_doc_set_name(world, e, fullpath);
	}

	if (info.type == SDL_PATHTYPE_FILE) {
		ecs_doc_set_color(world, e, "#358844FF");
		ecs_doc_set_brief(world, e, "File");
	} else if (info.type == SDL_PATHTYPE_DIRECTORY) {
		ecs_doc_set_color(world, e, "#888811FF");
		ecs_doc_set_brief(world, e, "Folder");
	} else {
		ecs_doc_set_color(world, e, "#880000FF");
		ecs_doc_set_brief(world, e, "Other");
	}

	//SDL_Log("%s (type=%i, size=%" SDL_PRIu64 ", create=%" SDL_PRIu64 ", mod=%" SDL_PRIu64 ", access=%" SDL_PRIu64 ") %jx", fullpath, info.type, info.size, info.modify_time, info.create_time, info.access_time, e);

	if (info.type == SDL_PATHTYPE_DIRECTORY) {
		if (!SDL_EnumerateDirectory(fullpath, (SDL_EnumerateDirectoryCallback)enum_callback, world)) {
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Enumeration failed!");
		}
	}

	SDL_free(fullpath);
	return SDL_ENUM_CONTINUE;
}

void EgFsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgFs);
	ecs_set_name_prefix(world, "EgFs");

	if (!SDL_EnumerateDirectory(".", (SDL_EnumerateDirectoryCallback)enum_callback, world)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Base path enumeration failed!");
	}
}
