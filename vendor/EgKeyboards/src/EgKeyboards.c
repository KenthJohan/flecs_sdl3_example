#include "EgKeyboards.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_events.h>
#include <EgShapes.h>
#include <EgSpatials.h>


ECS_COMPONENT_DECLARE(EgKeyboardsDevice);
ECS_COMPONENT_DECLARE(EgKeyboardsState);


static void System_EgKeyboardsState(ecs_iter_t *it)
{
	EgKeyboardsState *create = ecs_field(it, EgKeyboardsState, 0);
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// SDLTest_CommonEvent(state, &event, &done);
		switch (event.type) {
		case SDL_EVENT_KEY_DOWN:
			create->keycode[event.key.key] = 1;
			create->scancode[event.key.scancode] = 1;
			break;
		case SDL_EVENT_KEY_UP:
			create->keycode[event.key.key] = 0;
			create->scancode[event.key.scancode] = 0;
			break;
		}
	}
}

void EgKeyboardsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgKeyboards);
	ecs_set_name_prefix(world, "EgKeyboards");

	ECS_COMPONENT_DEFINE(world, EgKeyboardsDevice);
	ECS_COMPONENT_DEFINE(world, EgKeyboardsState);


	ecs_struct(world,
	{.entity = ecs_id(EgKeyboardsState),
	.members = {
	{.name = "scancode", .type = ecs_id(ecs_u8_t), .count = 512},
	{.name = "keycode", .type = ecs_id(ecs_u8_t), .count = 512},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgKeyboardsDevice),
	.members = {
	{.name = "id", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgKeyboardsState", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgKeyboardsState,
	.query.terms =
	{
	{.id = ecs_id(EgKeyboardsState), .src.id = ecs_id(EgKeyboardsState), .inout = EcsInOut},
	}});

	ecs_singleton_set(world, EgKeyboardsState, {.keycode = {0}, .scancode = {0}});


	int count;
	SDL_KeyboardID *keyboards = SDL_GetKeyboards(&count);
	for(int i = 0; i < count; i++) {
		ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
		ecs_doc_set_name(world, e, SDL_GetKeyboardNameForID(keyboards[i]));
		ecs_set(world, e, EgKeyboardsDevice, {keyboards[i]});
		//printf("Keyboard %d: %d %s\n", i, keyboards[i], SDL_GetKeyboardNameForID(keyboards[i]));
	}

}
