#include "EgMeshes.h"

#include <stdlib.h>
#include <stdio.h>
#include <EgSpatials.h>
#include <EgBase.h>

ECS_COMPONENT_DECLARE(EgMeshesMesh);




static void gen_box24_vertex(float *x, int stride, float l0, float l1, float l2)
{
	/* Front face. */
	/* Bottom left */
	x[0] = -l0;
	x[1] = l1;
	x[2] = -l2;
	x += stride;
	x[0] = l0;
	x[1] = -l1;
	x[2] = -l2;
	x += stride;
	x[0] = -l0;
	x[1] = -l1;
	x[2] = -l2;
	x += stride;

	/* Top right */
	x[0] = -l0;
	x[1] = l1;
	x[2] = -l2;
	x += stride;
	x[0] = l0;
	x[1] = l1;
	x[2] = -l2;
	x += stride;
	x[0] = l0;
	x[1] = -l1;
	x[2] = -l2;
	x += stride;

	/* Left face */
	/* Bottom left */
	x[0] = -l0;
	x[1] = l1;
	x[2] = l2;
	x += stride;
	x[0] = -l0;
	x[1] = -l1;
	x[2] = -l2;
	x += stride;
	x[0] = -l0;
	x[1] = -l1;
	x[2] = l2;
	x += stride;

	/* Top right */
	x[0] = -l0;
	x[1] = l1;
	x[2] = l2;
	x += stride;
	x[0] = -l0;
	x[1] = l1;
	x[2] = -l2;
	x += stride;
	x[0] = -l0;
	x[1] = -l1;
	x[2] = -l2;
	x += stride;

	/* Top face */
	/* Bottom left */
	x[0] = -l0;
	x[1] = l1;
	x[2] = l2;
	x += stride;
	x[0] = l0;
	x[1] = l1;
	x[2] = -l2;
	x += stride;
	x[0] = -l0;
	x[1] = l1;
	x[2] = -l2;
	x += stride;

	/* Top right */
	x[0] = -l0;
	x[1] = l1;
	x[2] = l2;
	x += stride;
	x[0] = l0;
	x[1] = l1;
	x[2] = l2;
	x += stride;
	x[0] = l0;
	x[1] = l1;
	x[2] = -l2;
	x += stride;

	/* Right face */
	/* Bottom left */
	x[0] = l0;
	x[1] = l1;
	x[2] = -l2;
	x += stride;
	x[0] = l0;
	x[1] = -l1;
	x[2] = l2;
	x += stride;
	x[0] = l0;
	x[1] = -l1;
	x[2] = -l2;
	x += stride;

	/* Top right */
	x[0] = l0;
	x[1] = l1;
	x[2] = -l2;
	x += stride;
	x[0] = l0;
	x[1] = l1;
	x[2] = l2;
	x += stride;
	x[0] = l0;
	x[1] = -l1;
	x[2] = l2;
	x += stride;
}

static void repeat_rgb(float *x, int stride, int n, float r, float g, float b)
{
	for (int i = 0; i < n; ++i, x += stride) {
		x[0] = r;
		x[1] = g;
		x[2] = b;
	}
}

static void gen_box24_color(float *x, int stride)
{
	repeat_rgb(x, stride, 6, 1.0f, 1.0f, 0.0f);
	x += stride * 6;
	repeat_rgb(x, stride, 6, 0.0f, 0.0f, 1.0f);
	x += stride * 6;
	repeat_rgb(x, stride, 6, 1.0f, 1.0f, 1.0f);
	x += stride * 6;
	repeat_rgb(x, stride, 6, 0.0f, 1.0f, 0.0f);
	x += stride * 6;
	repeat_rgb(x, stride, 6, 0.0f, 1.0f, 1.0f);
	x += stride * 6;
	repeat_rgb(x, stride, 6, 1.0f, 0.0f, 0.0f); 
	x += stride * 6;
}

static void System_EgMeshesMesh(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgMeshesMesh *m = ecs_field(it, EgMeshesMesh, 0);
	for (int i = 0; i < it->count; ++i, ++m) {
		ecs_entity_t e = it->entities[i];
		ecs_remove(world, e, EgBaseUpdate);
		ecs_vec_reset(NULL, &m->data, sizeof(float) * 6);
		float * v = ecs_vec_grow(NULL, &m->data, sizeof(float)*6, 36);
		gen_box24_vertex(v + 0, 6, 0.5f, 0.5f, 0.5f);
		gen_box24_color(v + 3, 6);
	} // END FOR LOOP
}

void EgMeshesImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgMeshes);
	ecs_set_name_prefix(world, "EgMeshes");
	ECS_COMPONENT_DEFINE(world, EgMeshesMesh);

	ecs_struct(world,
	{.entity = ecs_id(EgMeshesMesh),
	.members = {
	{.name = "usage", .type = ecs_id(EgBaseVec)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgMeshesMesh", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgMeshesMesh,
	.query.terms = {
	{.id = ecs_id(EgMeshesMesh), .src.id = EcsSelf},
	{.id = EgBaseUpdate},
	{.id = EgBaseError, .oper = EcsNot}}});
}
