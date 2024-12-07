#pragma once
#include <flecs.h>


typedef struct
{
	ecs_vec_t data;
} EgMeshesMesh;

extern ECS_COMPONENT_DECLARE(EgMeshesMesh);



void EgMeshesImport(ecs_world_t *world);