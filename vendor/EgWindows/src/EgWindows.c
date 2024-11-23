#include "EgWindows.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_mouse.h>

#include <EgShapes.h>
#include <EgSpatials.h>

ECS_COMPONENT_DECLARE(EgWindowsWindow);
ECS_COMPONENT_DECLARE(EgWindowsWindowCreateInfo);
ECS_COMPONENT_DECLARE(EgWindowsMouse);

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

static void System_EgWindowsWindow_Position(ecs_iter_t *it)
{
	EgWindowsWindow *cw = ecs_field(it, EgWindowsWindow, 0); // self, in
	Position2 *cp = ecs_field(it, Position2, 1);             // self, out
	for (int i = 0; i < it->count; ++i) {
		int x;
		int y;
		SDL_GetWindowPosition(cw[i].object, (int *)&x, (int *)&y);
		cp[i].x = x;
		cp[i].y = y;
	}
}

static void System_EgWindowsWindow_Mouse(ecs_iter_t *it)
{
	Position2 *cp0 = ecs_field(it, Position2, 0); // parent
	Position2 *cp1 = ecs_field(it, Position2, 1); // self
	// EgWindowsMouse *cm = ecs_field(it, EgWindowsMouse, 2); // self
	for (int i = 0; i < it->count; ++i) {
		float x;
		float y;
		SDL_GetGlobalMouseState(&x, &y);
		cp1[i].x = x - cp0->x;
		cp1[i].y = y - cp0->y;
	}
}

void EgWindowsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgWindows);
	ecs_set_name_prefix(world, "EgWindows");

	ECS_COMPONENT_DEFINE(world, EgWindowsWindow);
	ECS_COMPONENT_DEFINE(world, EgWindowsWindowCreateInfo);
	ECS_COMPONENT_DEFINE(world, EgWindowsMouse);

	ecs_struct(world,
	{.entity = ecs_id(EgWindowsWindow),
	.members = {
	{.name = "object", .type = ecs_id(ecs_uptr_t)}
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgWindowsWindowCreateInfo),
	.members = {
	{.name = "debug", .type = ecs_id(ecs_bool_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgWindowsMouse),
	.members = {
	{.name = "dummy", .type = ecs_id(ecs_i32_t)},
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

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgWindowsWindow_Position", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgWindowsWindow_Position,
	.query.terms =
	{
	{.id = ecs_id(EgWindowsWindow), .src.id = EcsSelf, .inout = EcsIn},
	{.id = ecs_id(Position2), .src.id = EcsSelf, .inout = EcsOut},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgWindowsWindow_Mouse", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgWindowsWindow_Mouse,
	.query.terms =
	{
	{.id = ecs_id(Position2), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(Position2), .src.id = EcsSelf, .inout = EcsOut},
	{.id = ecs_id(EgWindowsMouse), .src.id = EcsSelf, .inout = EcsIn},
	}});
}
