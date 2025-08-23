#pragma once
#include <flecs.h>

extern ECS_TAG_DECLARE(EgFsDir);
extern ECS_TAG_DECLARE(EgFsFile);
extern ECS_TAG_DECLARE(EgFsUpdate);

void EgFsImport(ecs_world_t *world);
