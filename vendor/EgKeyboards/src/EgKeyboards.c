#include "EgKeyboards.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_events.h>
#include <EgShapes.h>
#include <EgSpatials.h>

ECS_COMPONENT_DECLARE(EgKeyboardsDevice);
ECS_COMPONENT_DECLARE(EgKeyboardsState);
ECS_COMPONENT_DECLARE(EgKeyboardsActionToggleEntity);

static void System_Toggle(ecs_iter_t *it)
{
	EgKeyboardsState *field_keyboard = ecs_field(it, EgKeyboardsState, 0);                         // singleton
	EgKeyboardsActionToggleEntity *field_action = ecs_field(it, EgKeyboardsActionToggleEntity, 1); // self

	for (int i = 0; i < it->count; ++i, ++field_action) {
		EgKeyboardsActionToggleEntity * a = field_action + i;
		if (field_keyboard->scancode[field_action->key_index]) {
			// ecs_add_id(it->world, it->entities[i], action->entity);
			if (ecs_has_pair(it->world, it->entities[i], EcsIsA, a->entity)) {
				ecs_remove_pair(it->world, it->entities[i], EcsIsA, a->entity);
			} else {
				ecs_add_pair(it->world, it->entities[i], EcsIsA, a->entity);
			}
		}
	}
}

void EgKeyboardsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgKeyboards);
	ecs_set_name_prefix(world, "EgKeyboards");

	ECS_COMPONENT_DEFINE(world, EgKeyboardsDevice);
	ECS_COMPONENT_DEFINE(world, EgKeyboardsState);
	ECS_COMPONENT_DEFINE(world, EgKeyboardsActionToggleEntity);

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

	ecs_struct(world,
	{.entity = ecs_id(EgKeyboardsActionToggleEntity),
	.members = {
	{.name = "key_index", .type = ecs_id(ecs_i32_t)},
	{.name = "entity", .type = ecs_id(ecs_entity_t)},
	}});

	int count;
	SDL_KeyboardID *keyboards = SDL_GetKeyboards(&count);
	for (int i = 0; i < count; i++) {
		ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
		ecs_doc_set_name(world, e, SDL_GetKeyboardNameForID(keyboards[i]));
		ecs_set(world, e, EgKeyboardsDevice, {keyboards[i]});
		// printf("Keyboard %d: %d %s\n", i, keyboards[i], SDL_GetKeyboardNameForID(keyboards[i]));
	}


	ecs_system(world,{
	.entity = ecs_entity(world, {.name = "System_Toggle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Toggle,
	.query.terms =
	{
	{.id = ecs_id(EgKeyboardsState), .src.id = ecs_id(EgKeyboardsState)},
	{.id = ecs_id(EgKeyboardsActionToggleEntity), .src.id = EcsSelf}
	}});

}
