#pragma once
#include <flecs.h>
#include <egmath.h>

typedef struct {
	float fov;
	m4f32 view;
	m4f32 projection;
	m4f32 vp;
} Camera;

extern ECS_COMPONENT_DECLARE(Camera);

void EgCamerasImport(ecs_world_t *world);