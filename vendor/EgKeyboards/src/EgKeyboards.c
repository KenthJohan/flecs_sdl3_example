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

/*
printf("remove1(%s,%s)\n", ecs_get_name(world, prefab), ecs_get_name(world, subject));
const ecs_type_t *type = ecs_get_type(world, subject);
char *type_str = ecs_type_str(world, type);
printf("ecs_type_str: %s\n\n", type_str);
ecs_os_free(type_str);
        char const * n1 = ecs_get_name(world, subject);
        char const * n2 = ecs_get_name(world, id);
        printf("ecs_remove_id(%s,%s)\n", n1, n2);
*/

/*
https://www.flecs.dev/flecs/md_docs_2ComponentTraits.html
*/
void remove_copies_from_prefab(ecs_world_t *world, ecs_entity_t prefab, ecs_entity_t subject)
{
	// Removes every component of subject that are also in prefab:
	const ecs_type_t *type = ecs_get_type(world, prefab);
	for (int i = 0; i < type->count; i++) {
		ecs_id_t id = type->array[i];
		if (ecs_has_id(world, subject, id) == false) {
			continue;
		}
		if (ecs_is_alive(world, id) == false) {
			continue;
		}
		ecs_remove_id(world, subject, id);
	}
	// Removes every child of subject that are also in prefab:
	ecs_query_t *q = ecs_query(world,
	{.cache_kind = EcsQueryCacheNone,
	.terms = {
	{.id = ecs_pair(prefab, EcsWildcard)},
	{.id = ecs_childof(subject)}}});
	ecs_iter_t it = ecs_query_iter(world, q);
	while (ecs_query_next(&it)) {
		for (int i = 0; i < it.count; ++i) {
			ecs_entity_t e = it.entities[i];
			ecs_delete(world, e);
		}
	}
	ecs_query_fini(q);
}

static void System_Toggle(ecs_iter_t *it)
{
	ecs_log_set_level(1);
	EgKeyboardsState *field_keyboard = ecs_field(it, EgKeyboardsState, 0);              // singleton
	EgKeyboardsActionToggleEntity *a = ecs_field(it, EgKeyboardsActionToggleEntity, 1); // self
	for (int i = 0; i < it->count; ++i, ++a) {
		//ecs_entity_t e = it->entities[i];
		uint8_t k = field_keyboard->scancode[a->key_index];
		if (k & EG_KEYBOARDS_STATE_RISING_EDGE) {
			if (ecs_has_pair(it->world, a->entity, EcsIsA, a->toggle)) {
				ecs_dbg("ecs_remove_pair(%s,%s,%s)", ecs_get_name(it->world, a->entity), ecs_get_name(it->world, EcsIsA), ecs_get_name(it->world, a->toggle));
				ecs_remove_pair(it->world, a->entity, EcsIsA, a->toggle);
				remove_copies_from_prefab(it->world, a->toggle, a->entity);
			} else {
				ecs_dbg("ecs_add_pair(%s,%s,%s)", ecs_get_name(it->world, a->entity), ecs_get_name(it->world, EcsIsA), ecs_get_name(it->world, a->toggle));
				ecs_add_pair(it->world, a->entity, EcsIsA, a->toggle);
			}
		}
	}
	ecs_log_set_level(0);
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
	{.name = "toggle", .type = ecs_id(ecs_entity_t)},
	}});

	int count;
	SDL_KeyboardID *keyboards = SDL_GetKeyboards(&count);
	for (int i = 0; i < count; i++) {
		ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){0});
		ecs_doc_set_name(world, e, SDL_GetKeyboardNameForID(keyboards[i]));
		ecs_set(world, e, EgKeyboardsDevice, {keyboards[i]});
		// printf("Keyboard %d: %d %s\n", i, keyboards[i], SDL_GetKeyboardNameForID(keyboards[i]));
	}

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_Toggle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Toggle,
	.query.terms =
	{
	{.id = ecs_id(EgKeyboardsState), .src.id = ecs_id(EgKeyboardsState)},
	{.id = ecs_id(EgKeyboardsActionToggleEntity), .src.id = EcsSelf}}});
}
