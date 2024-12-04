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


typedef struct
{
	int32_t dummy;
} EgWindowsMouse;




extern ECS_COMPONENT_DECLARE(EgWindowsWindow);
extern ECS_COMPONENT_DECLARE(EgWindowsWindowCreateInfo);
extern ECS_COMPONENT_DECLARE(EgWindowsMouse);
extern ECS_TAG_DECLARE(EgWindowsEventResize);

void EgWindowsImport(ecs_world_t *world);


