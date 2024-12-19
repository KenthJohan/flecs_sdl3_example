#pragma once
#include <flecs.h>


typedef struct
{
	ecs_vec_t data;
	int32_t stride;
	int32_t offset_pos;
	int32_t offset_col;
} EgMeshesMesh;

extern ECS_COMPONENT_DECLARE(EgMeshesMesh);



void EgMeshesImport(ecs_world_t *world);