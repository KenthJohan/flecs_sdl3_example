#include "PlatformSdlKeyboards.h"
#include <EgKeyboards.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_events.h>




void PlatformSdlKeyboardsImport(ecs_world_t *world)
{
	ECS_MODULE(world, PlatformSdlKeyboards);
	ecs_set_name_prefix(world, "PlatformSdlKeyboards");


}