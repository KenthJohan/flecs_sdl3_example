#include "EgKeyboards.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_events.h>
#include <EgShapes.h>
#include <EgSpatials.h>

ECS_COMPONENT_DECLARE(EgKeyboardsKeys);

static void System_EgKeyboardsKeys(ecs_iter_t *it)
{
	EgKeyboardsKeys *create = ecs_field(it, EgKeyboardsKeys, 0);
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		// SDLTest_CommonEvent(state, &event, &done);
		switch (event.type) {
		case SDL_EVENT_KEY_DOWN:
			create->state[event.key.key] = 1;
			break;
		case SDL_EVENT_KEY_UP:
			create->state[event.key.key] = 0;
			break;
		}
	}
}

void EgKeyboardsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgKeyboards);
	ecs_set_name_prefix(world, "EgKeyboards");

	ECS_COMPONENT_DEFINE(world, EgKeyboardsKeys);

	ecs_struct(world,
	{.entity = ecs_id(EgKeyboardsKeys),
	.members = {
	{.name = "state", .type = ecs_id(ecs_u8_t), .count = 512}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgKeyboardsKeys", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgKeyboardsKeys,
	.query.terms =
	{
	{.id = ecs_id(EgKeyboardsKeys), .src.id = ecs_id(EgKeyboardsKeys), .inout = EcsInOut},
	}});

	ecs_singleton_set(world, EgKeyboardsKeys, {.state = {0}});
}
