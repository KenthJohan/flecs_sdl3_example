#pragma once
#include <flecs.h>


typedef struct
{
	ecs_vec_t vertices;
	ecs_vec_t indices;
} EgMeshesMesh;

typedef struct
{
	int32_t stride;
	int32_t offset_pos;
	int32_t offset_col;
} EgMeshesCreateInfo;

extern ECS_TAG_DECLARE(EgMeshesExpand);
extern ECS_COMPONENT_DECLARE(EgMeshesMesh);
extern ECS_COMPONENT_DECLARE(EgMeshesCreateInfo);



void EgMeshesImport(ecs_world_t *world);