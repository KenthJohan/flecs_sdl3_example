#pragma once

#include <flecs.h>

typedef struct {
	ecs_query_t *query;
} EgRaylibMode3D;

typedef struct {
	int32_t dummy;
} EgRaylibMode3DCreateInfo;

extern ECS_COMPONENT_DECLARE(EgRaylibMode3D);
extern ECS_COMPONENT_DECLARE(EgRaylibMode3DCreateInfo);


void EgRaylibImport(ecs_world_t *world);