#include "EgMeshes.h"

#include <stdlib.h>
#include <stdio.h>
#include <EgSpatials.h>
#include <EgShapes.h>
// #include <EgColor.h>
#include <EgBase.h>
#include <egmath.h>

ECS_TAG_DECLARE(EgMeshesExpand);
ECS_COMPONENT_DECLARE(EgMeshesInfo);

static void gen6_rectangle(EgBaseVertexIndexVec *vi, EgMeshesInfo *m)
{
	uint8_t *v = ecs_vec_grow(NULL, &vi->vertices, vi->stride_vertices, 6);
	v4f32_xyzw((float *)(v + m->offset_col), 1.0f, 0.0f, 0.0f, 1.0f);
	v3f32_xyz((float *)(v + m->offset_pos), -0.5f, -0.5f, 0.0f);
	v += vi->stride_vertices;
	v4f32_xyzw((float *)(v + m->offset_col), 1.0f, 0.0f, 0.0f, 1.0f);
	v3f32_xyz((float *)(v + m->offset_pos), -0.5f, 0.5f, 0.0f);
	v += vi->stride_vertices;
	v4f32_xyzw((float *)(v + m->offset_col), 1.0f, 0.0f, 0.0f, 1.0f);
	v3f32_xyz((float *)(v + m->offset_pos), 0.5f, -0.5f, 0.0f);
	v += vi->stride_vertices;
	v4f32_xyzw((float *)(v + m->offset_col), 1.0f, 0.0f, 0.0f, 1.0f);
	v3f32_xyz((float *)(v + m->offset_pos), 0.5f, -0.5f, 0.0f);
	v += vi->stride_vertices;
	v4f32_xyzw((float *)(v + m->offset_col), 1.0f, 0.0f, 0.0f, 1.0f);
	v3f32_xyz((float *)(v + m->offset_pos), -0.5f, 0.5f, 0.0f);
	v += vi->stride_vertices;
	v4f32_xyzw((float *)(v + m->offset_col), 1.0f, 0.0f, 0.0f, 1.0f);
	v3f32_xyz((float *)(v + m->offset_pos), 0.5f, 0.5f, 0.0f);
	uint16_t *i = ecs_vec_grow_t(NULL, &vi->indices, uint16_t, 6);
	m->index_counter += 6;
	i[0] = m->index_counter + 0;
	i[1] = m->index_counter + 1;
	i[2] = m->index_counter + 2;
	i[3] = m->index_counter + 3;
	i[4] = m->index_counter + 4;
	i[5] = m->index_counter + 5;
}

static void gen3_triangle(EgBaseVertexIndexVec *vi, EgMeshesInfo *m)
{
	uint8_t *v = ecs_vec_grow(NULL, &vi->vertices, vi->stride_vertices, 3);
	v4f32_xyzw((float *)(v + m->offset_col), 1.0f, 0.0f, 0.0f, 1.0f);
	v3f32_xyz((float *)(v + m->offset_pos), 0.0f, 0.5f, 0.0f);
	v += vi->stride_vertices;
	v4f32_xyzw((float *)(v + m->offset_col), 0.0f, 1.0f, 0.0f, 1.0f);
	v3f32_xyz((float *)(v + m->offset_pos), -0.5f, -0.5f, 0.0f);
	v += vi->stride_vertices;
	v4f32_xyzw((float *)(v + m->offset_col), 0.0f, 0.0f, 1.0f, 1.0f);
	v3f32_xyz((float *)(v + m->offset_pos), 0.5f, -0.5f, 0.0f);
	uint16_t *i = ecs_vec_grow_t(NULL, &vi->indices, uint16_t, 3);
	m->index_counter += 3;
	i[0] = m->index_counter + 0;
	i[1] = m->index_counter + 1;
	i[2] = m->index_counter + 2;
}

static void System_EgMeshesMesh(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EcsComponent *field_component = ecs_field(it, EcsComponent, 2); // shared
	ecs_entity_t field_component_src = ecs_field_src(it, 2);        // shared

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

	for (int i = 0; i < it->count; ++i) {
		ecs_remove(world, it->entities[i], EgBaseUpdate);
	} // END FOR LOOP

	{
		EgMeshesInfo *m = ecs_field(it, EgMeshesInfo, 0); // self
		for (int i = 0; i < it->count; ++i, ++m) {
			m->stride = stride;
			m->offset_pos = offset_pos;
			m->offset_col = offset_col;
		} // END FOR LOOP
	}

	{
		EgBaseVertexIndexVec *vi = ecs_field(it, EgBaseVertexIndexVec, 1); // self
		for (int i = 0; i < it->count; ++i, ++vi) {
			vi->stride_vertices = stride;
			ecs_vec_reset(NULL, &vi->vertices, stride);
			// Put example data into the vertices:
			// uint8_t *v = ecs_vec_grow(NULL, vertices, stride, 3);
			// gen3_triangle(v + offset_pos, stride);
			// gen3_color(v + offset_col, stride);
		} // END FOR LOOP
	}

	return;
on_error:
	for (int i = 0; i < it->count; ++i) {
		ecs_add(world, it->entities[i], EgBaseError);
	}
	return;
}

static void System_Triangle(ecs_iter_t *it)
{
	EgBaseVertexIndexVec *vi0 = ecs_field(it, EgBaseVertexIndexVec, 0); // shared, parent
	EgMeshesInfo *m0 = ecs_field(it, EgMeshesInfo, 1);                  // shared, parent
	for (int i = 0; i < it->count; ++i) {
		int offset = ecs_vec_count(&vi0->vertices);
		ecs_set(it->world, it->entities[i], EgBaseOffsetCount, {offset, 3});
		gen3_triangle(vi0, m0);
	} // END FOR LOOP
}

static void System_Rectangle(ecs_iter_t *it)
{
	EgBaseVertexIndexVec *vi0 = ecs_field(it, EgBaseVertexIndexVec, 0); // shared, parent
	EgMeshesInfo *m0 = ecs_field(it, EgMeshesInfo, 1);                  // shared, parent
	for (int i = 0; i < it->count; ++i) {
		int offset = ecs_vec_count(&vi0->vertices);
		ecs_set(it->world, it->entities[i], EgBaseOffsetCount, {offset, 6});
		gen6_rectangle(vi0, m0);
	} // END FOR LOOP
}

void EgMeshesImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgMeshes);
	ECS_IMPORT(world, EgBase);
	ecs_set_name_prefix(world, "EgMeshes");
	ECS_TAG_DEFINE(world, EgMeshesExpand);
	ECS_COMPONENT_DEFINE(world, EgMeshesInfo);

	ecs_struct(world,
	{.entity = ecs_id(EgMeshesInfo),
	.members = {
	{.name = "stride", .type = ecs_id(ecs_i32_t)},
	{.name = "offset_pos", .type = ecs_id(ecs_i32_t)},
	{.name = "offset_col", .type = ecs_id(ecs_i32_t)},
	{.name = "index_counter", .type = ecs_id(ecs_i32_t)}}});

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
	{.entity = ecs_entity(world, {.name = "System_Triangle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Triangle,
	.query.terms = {
	{.id = ecs_id(EgBaseVertexIndexVec), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgMeshesInfo), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgBaseOffsetCount), .oper = EcsNot}, // Adds this
	{.id = ecs_id(EgShapesTriangle)},
	{.id = EgBaseUpdate, .trav = EcsChildOf, .src.id = EcsUp, .oper = EcsNot},
	{.id = EgBaseError, .oper = EcsNot}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "System_Rectangle", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = System_Rectangle,
	.query.terms = {
	{.id = ecs_id(EgBaseVertexIndexVec), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgMeshesInfo), .trav = EcsChildOf, .src.id = EcsUp, .inout = EcsIn},
	{.id = ecs_id(EgBaseOffsetCount), .oper = EcsNot}, // Adds this
	{.id = ecs_id(EgShapesRectangle)},
	{.id = EgBaseUpdate, .trav = EcsChildOf, .src.id = EcsUp, .oper = EcsNot},
	{.id = EgBaseError, .oper = EcsNot}}});
}
