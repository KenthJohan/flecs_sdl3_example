#include "EgMeshes.h"

#include <stdlib.h>
#include <stdio.h>
#include <EgSpatials.h>
// #include <EgColor.h>
#include <EgBase.h>
#include <egmath.h>

ECS_TAG_DECLARE(EgMeshesTriangle);
ECS_TAG_DECLARE(EgMeshesExpand);
ECS_COMPONENT_DECLARE(EgMeshesInfo);

static void gen3_triangle(void *vertices, int stride)
{
	uint8_t *v = vertices;
	v3f32_xyz((float *)v, 0.0f, 0.5f, 0.0f);
	v += stride;
	v3f32_xyz((float *)v, 0.5f, -0.5f, 0.0f);
	v += stride;
	v3f32_xyz((float *)v, -0.5f, -0.5f, 0.0f);
}

static void gen3_color(void *vertices, int stride)
{
	uint8_t *v = vertices;
	v4f32_xyzw((float *)v, 1.0f, 0.0f, 0.0f, 1.0f);
	v += stride;
	v4f32_xyzw((float *)v, 0.0f, 1.0f, 0.0f, 1.0f);
	v += stride;
	v4f32_xyzw((float *)v, 0.0f, 0.0f, 1.0f, 1.0f);
}

static void System_EgMeshesMesh(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgMeshesInfo *field_info = ecs_field(it, EgMeshesInfo, 0);               // self
	EgBaseVertexIndexVec *field_vi = ecs_field(it, EgBaseVertexIndexVec, 1); // self
	EcsComponent *field_component = ecs_field(it, EcsComponent, 2);          // shared
	ecs_entity_t field_component_src = ecs_field_src(it, 2);                 // shared

	int32_t stride;
	int32_t offset_pos = -1;
	int32_t offset_col = -1;

	ecs_iter_t it2 = ecs_children(world, field_component_src);
	while (ecs_children_next(&it2)) {
		for (int i = 0; i < it2.count; i++) {
			ecs_entity_t child = it2.entities[i];
			EcsMember const *m = ecs_get(world, child, EcsMember);
			if (m == NULL) {
				ecs_err("m is NULL");
				break;
			}
			if (m->unit == EcsMeters) {
				// printf("m: %i\n", m->count);
				offset_pos = m->offset;
			}
			if (m->unit == EcsColorRgb) {
				// printf("m: %i\n", m->count);
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
	} // END FOR LOOP

	for (int i = 0; i < it->count; ++i, ++field_info) {
		field_info->stride = stride;
		field_info->offset_pos = offset_pos;
		field_info->offset_col = offset_col;
	} // END FOR LOOP

	for (int i = 0; i < it->count; ++i) {
		ecs_vec_t *vertices = &field_vi[i].vertices;
		ecs_vec_reset(NULL, vertices, stride);
		// Put example data into the vertices:
		uint8_t *v = ecs_vec_grow(NULL, vertices, stride, 3);
		gen3_triangle(v + offset_pos, stride);
		gen3_color(v + offset_col, stride);
	} // END FOR LOOP

	return;
on_error:
	for (int i = 0; i < it->count; ++i) {
		ecs_add(world, it->entities[i], EgBaseError);
	}
	return;
}



static void System_VertexIndexVec_Indexing(ecs_iter_t *it)
{
	EgBaseVertexIndexVec *vi0 = ecs_field(it, EgBaseVertexIndexVec, 0); // shared, parent
	EgMeshesInfo *info0 = ecs_field(it, EgMeshesInfo, 1);          // // shared, parent

	int total = it->count * 3;
	uint8_t *v = ecs_vec_grow(NULL, &vi0->vertices, info0->stride, total);

	for (int i = 0; i < it->count; ++i) {
		ecs_set(it->world, it->entities[i], EgBaseOffsetCount, {vi0->vertices.count, 3});
		gen3_triangle(v + info0->offset_pos, info0->stride);
		gen3_color(v + info0->offset_col, info0->stride);
		v += info0->stride * 3;
	} // END FOR LOOP
}

void EgMeshesImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgMeshes);
	ECS_IMPORT(world, EgBase);
	ecs_set_name_prefix(world, "EgMeshes");
	ECS_TAG_DEFINE(world, EgMeshesExpand);
	ECS_TAG_DEFINE(world, EgMeshesTriangle);
	ECS_COMPONENT_DEFINE(world, EgMeshesInfo);

	ecs_struct(world,
	{.entity = ecs_id(EgMeshesInfo),
	.members = {
	{.name = "stride", .type = ecs_id(ecs_i32_t)},
	{.name = "offset_pos", .type = ecs_id(ecs_i32_t)},
	{.name = "offset_col", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_EgMeshesMesh", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_EgMeshesMesh,
	.query.terms = {
	{.id = ecs_id(EgMeshesInfo), .src.id = EcsSelf},
	{.id = ecs_id(EgBaseVertexIndexVec), .src.id = EcsSelf},
	{.id = ecs_id(EcsComponent), .trav = EcsDependsOn, .src.id = EcsUp, .inout = EcsIn},
	{.id = EgBaseUpdate}, // Removes this tag
	{.id = EgBaseError, .oper = EcsNot}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_VertexIndexVec_Indexing", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_VertexIndexVec_Indexing,
	.query.terms = {
	{.id = ecs_id(EgBaseVertexIndexVec), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgMeshesInfo), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgBaseOffsetCount), .oper = EcsNot}, // Adds this
	{.id = EgMeshesTriangle},
	{.id = EgBaseUpdate, .trav = EcsChildOf, .src.id = EcsUp, .oper = EcsNot},
	{.id = EgBaseError, .oper = EcsNot}}});
}
