#pragma once
#include <flecs.h>


typedef struct
{
	void * object;
} EgWindowsWindow;

typedef struct
{
	bool debug;
} EgWindowsWindowCreateInfo;




extern ECS_COMPONENT_DECLARE(EgWindowsWindow);
extern ECS_COMPONENT_DECLARE(EgWindowsWindowCreateInfo);

void EgWindowsImport(ecs_world_t *world);


