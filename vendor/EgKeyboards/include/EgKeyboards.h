#pragma once
#include <flecs.h>

#define EG_KEYBOARDS_KEYS_MAX 512

typedef struct
{
	uint8_t state[EG_KEYBOARDS_KEYS_MAX];
} EgKeyboardsKeys;





extern ECS_COMPONENT_DECLARE(EgKeyboardsKeys);

void EgKeyboardsImport(ecs_world_t *world);


