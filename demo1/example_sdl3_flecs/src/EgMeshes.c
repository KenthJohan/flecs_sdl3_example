#include "EgMeshes.h"

#include <stdlib.h>
#include <stdio.h>
#include <EgSpatials.h>
#include <EgBase.h>
#include <egmath.h>

ECS_COMPONENT_DECLARE(EgMeshesMesh);




static void gen_triangle(void *vertices, int stride)
{
	uint8_t *v = vertices;
	v3f32_xyz((float*)v, 0.0f, 0.5f, 0.0f);
	v += stride;
	v3f32_xyz((float*)v, 0.5f, -0.5f, 0.0f);
	v += stride;
	v3f32_xyz((float*)v, -0.5f, -0.5f, 0.0f);
}


static void gen_color(void *vertices, int stride)
{
	uint8_t *v = vertices;
	v4f32_xyzw((float*)v, 1.0f, 0.0f, 0.0f, 1.0f);
	v += stride;
	v4f32_xyzw((float*)v, 0.0f, 1.0f, 0.0f, 1.0f);
	v += stride;
	v4f32_xyzw((float*)v, 0.0f, 0.0f, 1.0f, 1.0f);
}


static void System_EgMeshesMesh(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgMeshesMesh *field_mesh = ecs_field(it, EgMeshesMesh, 0);      // self
	EcsComponent *field_component = ecs_field(it, EcsComponent, 1); // shared
	ecs_entity_t field_component_src = ecs_field_src(it, 1);        // shared

	int32_t stride;
	int32_t offset_pos = -1;
	int32_t offset_col = -1;

	ecs_iter_t it2 = ecs_children(world, field_component_src);
	while (ecs_children_next(&it2)) {
		for (int i = 0; i < it2.count; i ++) {
			ecs_entity_t child = it2.entities[i];
			EcsMember const * m = ecs_get(world, child, EcsMember);
			if (m == NULL) {
				ecs_err("m is NULL");
				break;
			}
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

	if (offset_pos == -1) {
		ecs_err("offset_pos not found");
		goto on_error;
	}

	if (offset_col == -1) {
		ecs_err("offset_col not found");
		goto on_error;
	}

	/*
	float x6[] = {55.0f, 0.0f, 1.0f, 0.0f, 3.0f, 0.0f};
	char * json = ecs_ptr_to_json(it->world, field_component_src, x6);
	printf("%s\n", json);
	ecs_os_free(json);
	*/
	for (int i = 0; i < it->count; ++i) {
		ecs_remove(world, it->entities[i], EgBaseUpdate);
	}

	for (int i = 0; i < it->count; ++i, ++field_mesh) {
		field_mesh->stride = stride;
		field_mesh->offset_pos = offset_pos;
		field_mesh->offset_col = offset_col;
		ecs_vec_reset(NULL, &field_mesh->vertices, stride);
		uint8_t *v = ecs_vec_grow(NULL, &field_mesh->vertices, stride, 3);
		gen_triangle(v + offset_pos, stride);
		gen_color(v + offset_col, stride);
	} // END FOR LOOP

	return;
on_error:
	for (int i = 0; i < it->count; ++i) {
		ecs_add(world, it->entities[i], EgBaseError);
	}
	return;
}

void EgMeshesImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgMeshes);
	ecs_set_name_prefix(world, "EgMeshes");
	ECS_COMPONENT_DEFINE(world, EgMeshesMesh);

	ecs_struct(world,
	{.entity = ecs_id(EgMeshesMesh),
	.members = {
	{.name = "vertices", .type = ecs_id(EgBaseVec)},
	{.name = "indices", .type = ecs_id(EgBaseVec)},
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
