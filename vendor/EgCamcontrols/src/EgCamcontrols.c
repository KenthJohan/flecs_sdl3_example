#include "EgCamcontrols.h"

#include <EgCameras.h>
#include <EgSpatials.h>
#include <EgKeyboards.h>
#include <egmath.h>
#include <assert.h>

/*
TODO:
Change these systems into a more generic system that can be used for any entity that has a rotation and a velocity component.

*/


static void System_Rotate(ecs_iter_t *it)
{
	EgCamerasKeyBindings *c = ecs_field(it, EgCamerasKeyBindings, 0); // self
	Rotate3 *r = ecs_field(it, Rotate3, 1);                           // self
	EgKeyboardsState *k0 = ecs_field(it, EgKeyboardsState, 2);        // singleton
	uint8_t *keys = k0->scancode;
	float d = 0.8f * it->delta_time;
	for (int i = 0; i < it->count; ++i, ++c, ++r) {
		r->dx = !!keys[c->key_rotate_dx_plus] - !!keys[c->key_rotate_dx_minus];
		r->dy = !!keys[c->key_rotate_dy_plus] - !!keys[c->key_rotate_dy_minus];
		r->dz = !!keys[c->key_rotate_dz_plus] - !!keys[c->key_rotate_dz_minus];
		v3f32_mul((float *)r, (float *)r, d);
	}
}

static void System_Move(ecs_iter_t *it)
{
	EgCamerasKeyBindings *c = ecs_field(it, EgCamerasKeyBindings, 0); // self
	Velocity3 *v = ecs_field(it, Velocity3, 1);                       // self
	EgKeyboardsState *k0 = ecs_field(it, EgKeyboardsState, 2);        // singleton
	uint8_t *keys = k0->scancode;
	float moving_speed = 1.1f;
	float d = it->delta_time * moving_speed;
	for (int i = 0; i < it->count; ++i, ++c, ++v) {
		v->x = -(!!keys[c->key_move_dx_plus] - !!keys[c->key_move_dx_minus]);
		v->y = -(!!keys[c->key_move_dy_plus] - !!keys[c->key_move_dy_minus]);
		v->z = -(!!keys[c->key_move_dz_plus] - !!keys[c->key_move_dz_minus]);
		v3f32_mul((float *)v, (float *)v, d);
	}
}

void EgCamcontrolsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgCamcontrols);
	ECS_IMPORT(world, EgSpatials);
	ECS_IMPORT(world, EgKeyboards);
	ECS_IMPORT(world, EgCameras);
	ecs_set_name_prefix(world, "EgCamcontrols");

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_Rotate", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Rotate,
	.query.terms = {
	{.id = ecs_id(EgCamerasKeyBindings), .src.id = EcsSelf},
	{.id = ecs_id(Rotate3), .src.id = EcsSelf},
	{.id = ecs_id(EgKeyboardsState), .src.id = ecs_id(EgKeyboardsState)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_Move", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Move,
	.query.terms = {
	{.id = ecs_id(EgCamerasKeyBindings), .src.id = EcsSelf},
	{.id = ecs_id(Velocity3), .src.id = EcsSelf},
	{.id = ecs_id(EgKeyboardsState), .src.id = ecs_id(EgKeyboardsState)},
	}});

}