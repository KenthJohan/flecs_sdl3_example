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
	EgMeshesMesh *field_mesh = ecs_field(it, EgMeshesMesh, 0);      // self
	EcsComponent *field_component = ecs_field(it, EcsComponent, 1); // shared
	ecs_entity_t field_component_src = ecs_field_src(it, 1);        // shared

	int32_t stride;
	int32_t offset_pos;
	int32_t offset_col;

	ecs_iter_t it2 = ecs_children(world, field_component_src);
	while (ecs_children_next(&it2)) {
		for (int i = 0; i < it2.count; i ++) {
			ecs_entity_t child = it2.entities[i];
			EcsMember const * m = ecs_get(world, child, EcsMember);
			if (m->unit == EcsMeters) {
				//printf("m: %i\n", m->count);
				offset_pos = m->offset;
			}
			if (m->unit == EcsColorRgb) {
				//printf("m: %i\n", m->count);
				offset_col = m->offset;
			}
		}
	}

	stride = field_component->size;

	/*
	float x6[] = {55.0f, 0.0f, 1.0f, 0.0f, 3.0f, 0.0f};
	char * json = ecs_ptr_to_json(it->world, field_component_src, x6);
	printf("%s\n", json);
	ecs_os_free(json);
	*/

	for (int i = 0; i < it->count; ++i, ++field_mesh) {
		field_mesh->stride = stride;
		field_mesh->offset_pos = offset_pos;
		field_mesh->offset_col = offset_col;
		ecs_entity_t e = it->entities[i];
		ecs_remove(world, e, EgBaseUpdate);
		ecs_vec_reset(NULL, &field_mesh->data, sizeof(float) * 6);
		float *v = ecs_vec_grow(NULL, &field_mesh->data, sizeof(float) * 6, 36);
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
	{.name = "stride", .type = ecs_id(ecs_i32_t)},
	{.name = "offset_pos", .type = ecs_id(ecs_i32_t)},
	{.name = "offset_col", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgMeshesMesh", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgMeshesMesh,
	.query.terms = {
	{.id = ecs_id(EgMeshesMesh), .src.id = EcsSelf},
	{.id = ecs_id(EcsComponent), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = EgBaseUpdate},
	{.id = EgBaseError, .oper = EcsNot}}});
}
