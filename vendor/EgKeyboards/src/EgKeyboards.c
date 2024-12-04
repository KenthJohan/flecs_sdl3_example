#include "EgKeyboards.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_events.h>
#include <EgShapes.h>
#include <EgSpatials.h>

ECS_COMPONENT_DECLARE(EgKeyboardsDevice);
ECS_COMPONENT_DECLARE(EgKeyboardsState);





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


	int count;
	SDL_KeyboardID *keyboards = SDL_GetKeyboards(&count);
	for (int i = 0; i < count; i++) {
		ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
		ecs_doc_set_name(world, e, SDL_GetKeyboardNameForID(keyboards[i]));
		ecs_set(world, e, EgKeyboardsDevice, {keyboards[i]});
		// printf("Keyboard %d: %d %s\n", i, keyboards[i], SDL_GetKeyboardNameForID(keyboards[i]));
	}
}
