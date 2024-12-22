#pragma once

#include <flecs.h>
#include "egmath.h"

typedef struct {
	float x;
	float y;
} Position2;

typedef struct {
	float x;
	float y;
	float z;
} Position3;

typedef struct {
	float r;
	float g;
	float b;
} Color3;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} Vector4;

typedef struct {
	float x;
	float y;
	float z;
} Ray3;

typedef struct {
	float x;
	float y;
	float z;
} Scale3;

typedef struct {
	float x;
	float y;
	float z;
} Position3World;

typedef struct {
	float x;
	float y;
} Velocity2;

typedef struct {
	float x;
	float y;
	float z;
} Velocity3;

typedef struct {
	m4f32 matrix;
} Transformation;

typedef struct {
	m4f32 * data;
	int32_t count;
	int32_t cap;
	int32_t total;
} TransformationCollector;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} Orientation;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} OrientationWorld;

typedef struct {
	float pitch;
	float yaw;
	float roll;
} EulerAngles;

typedef struct {
	float dx;
	float dy;
	float dz;
} Rotate3;

typedef struct {
	float x1;
	float y1;
	float z1;
	float x2;
	float y2;
	float z2;
	float x3;
	float y3;
	float z3;
} RotMat3;

typedef struct {
	float frequency;
	float amplitude;
} Sinewave;


extern ECS_COMPONENT_DECLARE(Position2);
extern ECS_COMPONENT_DECLARE(Position3);
extern ECS_COMPONENT_DECLARE(Ray3);
extern ECS_COMPONENT_DECLARE(Scale3);
extern ECS_COMPONENT_DECLARE(Position3World);
extern ECS_COMPONENT_DECLARE(Velocity2);
extern ECS_COMPONENT_DECLARE(Velocity3);
extern ECS_COMPONENT_DECLARE(Orientation);
extern ECS_COMPONENT_DECLARE(OrientationWorld);
extern ECS_COMPONENT_DECLARE(EulerAngles);
extern ECS_COMPONENT_DECLARE(Rotate3);
extern ECS_COMPONENT_DECLARE(Transformation);
extern ECS_COMPONENT_DECLARE(TransformationCollector);
extern ECS_COMPONENT_DECLARE(RotMat3);
extern ECS_COMPONENT_DECLARE(Sinewave);
extern ECS_COMPONENT_DECLARE(Color3);

extern ECS_TAG_DECLARE(EgRotateOrder1);
extern ECS_TAG_DECLARE(EgRotateOrder2);
extern ECS_TAG_DECLARE(EgPositionWorldNoReset);

void EgSpatialsImport(ecs_world_t *world);