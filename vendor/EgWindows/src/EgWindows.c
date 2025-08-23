#include "EgWindows.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_events.h>

#include <EgShapes.h>
#include <EgSpatials.h>
#include <EgKeyboards.h>
#include <EgBase.h>

ECS_COMPONENT_DECLARE(EgWindowsWindow);
ECS_COMPONENT_DECLARE(EgWindowsWindowCreateInfo);
ECS_COMPONENT_DECLARE(EgWindowsMouse);
ECS_TAG_DECLARE(EgWindowsEventResize);


// https://github.com/SanderMertens/flecs/blob/57ebed1083274ee4875631a940452f08ff08aef9/test/collections/src/Map.c#L54
// We need to map the SDL_WindowID to the ecs_entity_t so we can look up the entity from the SDL_WindowID
// This is needed for the SDL_EVENT_WINDOW_RESIZED event
static ecs_map_t static_window_map;

static void System_EgWindowsWindow_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgWindowsWindowCreateInfo *create = ecs_field(it, EgWindowsWindowCreateInfo, 0);
	ecs_log_set_level(1);
	ecs_trace("System_EgWindowsWindow_Create() count:%i", it->count);
	ecs_log_push_(0);
	for (int i = 0; i < it->count; ++i, ++create) {
		ecs_entity_t e = it->entities[i];
		ecs_trace("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_(0);
		{
			SDL_PropertiesID props = SDL_CreateProperties();
			SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, ecs_get_name(world, e));
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, 100);
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, 100);
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, 600);
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, 400);
			SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
			SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_FLAGS_NUMBER, flags);
			SDL_Window *window = SDL_CreateWindowWithProperties(props);
			if (window == NULL) {
				continue;
			}
			// Map the SDL_WindowID to the ecs_entity_t:
			SDL_WindowID id = SDL_GetWindowID(window);
			ecs_map_insert(&static_window_map, id, e);
			ecs_set(world, e, EgWindowsWindow, {.object = window});
			ecs_trace("SDL_CreateWindowWithProperties() -> %p", window);
		}
		ecs_log_pop_(0);

	} // END FOR LOOP
	ecs_log_pop_(0);
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

static void System_EgKeyboardsState(ecs_iter_t *it)
{
	EgKeyboardsState *s = ecs_field(it, EgKeyboardsState, 0); // Singleton
	SDL_Event event;
	for (int i = 0; i < EG_KEYBOARDS_KEYS_MAX; i++) {
		s->scancode[i] &= ~EG_KEYBOARDS_STATE_FALLING_EDGE;
		s->scancode[i] &= ~EG_KEYBOARDS_STATE_RISING_EDGE;
	}
	while (SDL_PollEvent(&event)) {
		// SDLTest_CommonEvent(state, &event, &done);
		switch (event.type) {
		case SDL_EVENT_KEY_DOWN:
			if (s->scancode[event.key.scancode] & EG_KEYBOARDS_STATE_DOWN) {
				//s->scancode[event.key.scancode] &= ~EG_KEYBOARDS_STATE_RISING_EDGE;
			} else {
				s->scancode[event.key.scancode] |= EG_KEYBOARDS_STATE_RISING_EDGE;
			}
			s->scancode[event.key.scancode] |= EG_KEYBOARDS_STATE_DOWN;
			break;
		case SDL_EVENT_KEY_UP:
			s->scancode[event.key.scancode] &= ~EG_KEYBOARDS_STATE_DOWN;
			break;
		case SDL_EVENT_WINDOW_RESIZED: {
			// Get the entity from the SDL_WindowID
			SDL_Window *window = SDL_GetWindowFromEvent(&event);
			SDL_WindowID id = SDL_GetWindowID(window);
			ecs_map_val_t * ev = ecs_map_get(&static_window_map, id);
			if (ev == NULL) {
				ecs_err("SDL_EVENT_WINDOW_RESIZED: No entity found for SDL_WindowID: %i", id);
				break;
			}
			ecs_entity_t e = ev[0];
			printf("SDL_EVENT_WINDOW_RESIZED: %s\n", ecs_get_name(it->world, e));
			ecs_add(it->world, e, EgWindowsEventResize);
			break;
		} // END CASE
		} // END SWITCH
	} // END WHILE
	//printf("EG_KEYBOARDS_STATE_RISING_EDGE: %i\n", s->scancode[SDL_SCANCODE_B] & EG_KEYBOARDS_STATE_RISING_EDGE);
}


static void System_Resize(ecs_iter_t *it)
{
	ecs_id_t pair = ecs_field_id(it, 1);
	ecs_entity_t food = ecs_pair_second(it->world, pair);
	ecs_trace("food: %s\n", ecs_get_name(it->world, food));
	ecs_add(it->world, food, EgBaseUpdate);
	for (int i = 0; i < it->count; ++i) {
		ecs_remove(it->world, it->entities[i], EgWindowsEventResize);
	}
}


void EgWindowsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgWindows);
	ECS_IMPORT(world, EgKeyboards);
	ecs_set_name_prefix(world, "EgWindows");

    ecs_map_init(&static_window_map, NULL);

	ECS_COMPONENT_DEFINE(world, EgWindowsWindow);
	ECS_COMPONENT_DEFINE(world, EgWindowsWindowCreateInfo);
	ECS_COMPONENT_DEFINE(world, EgWindowsMouse);
	ECS_TAG_DEFINE(world, EgWindowsEventResize);

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

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgKeyboardsState", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgKeyboardsState,
	.query.terms =
	{
	{.id = ecs_id(EgKeyboardsState), .src.id = ecs_id(EgKeyboardsState), .inout = EcsInOut},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_Resize", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Resize,
	.query.terms =
	{
	{.id = ecs_id(EgWindowsEventResize), .src.id = EcsSelf, .inout = EcsOut},
	{.id = ecs_pair(EgWindowsEventResize, EcsWildcard ), .src.id = EcsSelf, .inout = EcsOut},
	}});


	ecs_singleton_set(world, EgKeyboardsState, {.keycode = {0}, .scancode = {0}});

}
