#pragma once
#include <flecs.h>

typedef struct {
	int dummy;
} EgRenderersContext;

typedef struct {
	int dummy;
} EgRenderersCreateInfo;


extern ECS_COMPONENT_DECLARE(EgRenderersContext);
extern ECS_COMPONENT_DECLARE(EgRenderersCreateInfo);

void EgRenderersImport(ecs_world_t *world);
