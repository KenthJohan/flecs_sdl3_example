#pragma once
#include <flecs.h>
#include <egmath.h>

typedef struct {
	float fov;
	m4f32 view;
	m4f32 projection;
	m4f32 vp;
} EgCamerasState;

typedef struct {
	uint16_t key_rotate_dx_plus;
	uint16_t key_rotate_dx_minus;
	uint16_t key_rotate_dy_plus;
	uint16_t key_rotate_dy_minus;
	uint16_t key_rotate_dz_plus;
	uint16_t key_rotate_dz_minus;
	uint16_t key_move_dx_plus;
	uint16_t key_move_dx_minus;
	uint16_t key_move_dy_plus;
	uint16_t key_move_dy_minus;
	uint16_t key_move_dz_plus;
	uint16_t key_move_dz_minus;
	uint16_t key_fov_reset;
	uint16_t key_fov_plus;
	uint16_t key_fov_minus;
} EgCamerasKeyBindings;

extern ECS_COMPONENT_DECLARE(EgCamerasState);
extern ECS_COMPONENT_DECLARE(EgCamerasKeyBindings);

void EgCamerasImport(ecs_world_t *world);