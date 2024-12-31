#pragma once
#include <flecs.h>





typedef struct
{
	int32_t stride;
	int32_t offset_pos;
	int32_t offset_col;
} EgMeshesInfo;

extern ECS_TAG_DECLARE(EgMeshesTriangle);
extern ECS_TAG_DECLARE(EgMeshesExpand);
extern ECS_COMPONENT_DECLARE(EgMeshesInfo);



void EgMeshesImport(ecs_world_t *world);