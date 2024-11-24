#pragma once
#include <flecs.h>

#define EG_KEYBOARDS_KEYS_MAX 512

typedef struct
{
	int32_t id;
} EgKeyboardsDevice;

typedef struct
{
	uint8_t scancode[EG_KEYBOARDS_KEYS_MAX];
	uint8_t keycode[EG_KEYBOARDS_KEYS_MAX];
} EgKeyboardsState;




extern ECS_COMPONENT_DECLARE(EgKeyboardsDevice);
extern ECS_COMPONENT_DECLARE(EgKeyboardsState);

void EgKeyboardsImport(ecs_world_t *world);


