#pragma once

#include <flecs.h>

typedef struct {
	float radius;
	float ring_radius;
	int32_t rings;
	int32_t sides;
	int32_t random_colors;
} EgShapesTorus;

typedef struct {
	float radius;
	float height;
	int32_t slices;
	int32_t stacks;
	int32_t random_colors;
} EgShapesCylinder;

typedef struct {
	float radius;
	int32_t slices;
	int32_t stacks;
	int32_t random_colors;
} EgShapesSphere;

typedef struct {
	float width;
	float height;
	float depth;
	int32_t random_colors;
} EgShapesBox;

typedef struct {
	float a[3];
	float b[3];
} EgShapesLine;

typedef struct {
	float w;
	float h;
} EgShapesRectangle;

typedef struct {
	float w;
	float h;
} EgShapesTriangle;

extern ECS_COMPONENT_DECLARE(EgShapesBox);
extern ECS_COMPONENT_DECLARE(EgShapesTorus);
extern ECS_COMPONENT_DECLARE(EgShapesCylinder);
extern ECS_COMPONENT_DECLARE(EgShapesSphere);
extern ECS_COMPONENT_DECLARE(EgShapesLine);
extern ECS_COMPONENT_DECLARE(EgShapesRectangle);
extern ECS_COMPONENT_DECLARE(EgShapesTriangle);

void EgShapesImport(ecs_world_t *world);