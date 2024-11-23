#include "EgWindows.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>

#include <EgShapes.h>

ECS_COMPONENT_DECLARE(EgWindowsWindow);
ECS_COMPONENT_DECLARE(EgWindowsWindowCreateInfo);

static void System_EgWindowsWindow_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgWindowsWindowCreateInfo *create = ecs_field(it, EgWindowsWindowCreateInfo, 0);
	ecs_log_set_level(1);
	ecs_dbg("System_EgWindowsWindow_Create() count:%i", it->count);
	ecs_log_push_1();
	for (int i = 0; i < it->count; ++i, ++create) {
		ecs_entity_t e = it->entities[i];
		ecs_dbg("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_1();
		{
			SDL_PropertiesID props = SDL_CreateProperties();
			SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, ecs_get_name(world, e));
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, 100);
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, 100);
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 100);
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 100);
			SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, flags);
			SDL_Window *window = SDL_CreateWindowWithProperties(props);
			if (window == NULL) {
				continue;
			}
			ecs_set(world, e, EgWindowsWindow, {.object = window});
			ecs_dbg("SDL_CreateWindowWithProperties() -> %p", window);
		}
		ecs_log_pop_1();

	} // END FOR LOOP
	ecs_log_pop_1();
	ecs_log_set_level(0);
}


static void System_EgWindowsWindow_Rectangle(ecs_iter_t *it)
{
	EgWindowsWindow *cw = ecs_field(it, EgWindowsWindow, 0);
	EgShapesRectangle *cr = ecs_field(it, EgShapesRectangle, 1);
	for (int i = 0; i < it->count; ++i) {
		Uint32 w;
		Uint32 h;
		SDL_GetWindowSizeInPixels(cw[i].object, (int *)&w, (int *)&h);
		cr[i].w = w;
		cr[i].h = h;
	}
}

void EgWindowsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgWindows);
	ecs_set_name_prefix(world, "EgWindows");

	ECS_COMPONENT_DEFINE(world, EgWindowsWindow);
	ECS_COMPONENT_DEFINE(world, EgWindowsWindowCreateInfo);

	ecs_struct(world,
	{.entity = ecs_id(EgWindowsWindow),
	.members = {
	{.name = "object", .type = ecs_id(ecs_uptr_t)},
	{.name = "w", .type = ecs_id(ecs_i32_t)},
	{.name = "h", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgWindowsWindowCreateInfo),
	.members = {
	{.name = "debug", .type = ecs_id(ecs_bool_t)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgWindowsWindow_Create", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgWindowsWindow_Create,
	.query.terms =
	{
	{.id = ecs_id(EgWindowsWindowCreateInfo), .src.id = EcsSelf},
	{.id = ecs_id(EgWindowsWindow), .oper = EcsNot}, // Adds this
	}});


	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgWindowsWindow_Rectangle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgWindowsWindow_Rectangle,
	.query.terms =
	{
	{.id = ecs_id(EgWindowsWindow), .src.id = EcsSelf},
	{.id = ecs_id(EgShapesRectangle), .src.id = EcsSelf},
	}});
}
