#include "EgDisplay.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>

static void private_list_modes(ecs_world_t *world, ecs_entity_t parent, SDL_DisplayID displayID)
{
	SDL_DisplayMode **modes;
	/* Print available fullscreen video modes */
	int m;
	const SDL_DisplayMode *mode;
	modes = SDL_GetFullscreenDisplayModes(displayID, &m);
	if (m == 0) {
		SDL_Log("No available fullscreen video modes\n");
		return;
	}
	
	for (int j = 0; j < m; ++j) {
		mode = modes[j];
		int bpp;
		Uint32 Rmask, Gmask, Bmask, Amask;
		SDL_GetMasksForPixelFormat(mode->format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);

		char buf[256];
		snprintf(buf, sizeof(buf), "Mode %i", j);
		ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){.name = buf, .parent = parent});
		snprintf(buf, sizeof(buf), "%i: %dx%d@%gx %gHz, %d bits-per-pixel (%s)", j, mode->w, mode->h, mode->pixel_density, mode->refresh_rate, bpp, SDL_GetPixelFormatName(mode->format));
		ecs_doc_set_name(world, e, buf);
		//SDL_Log("Mode %d: %dx%d@%gx %gHz, %d bits-per-pixel (%s)\n", j, mode->w, mode->h, mode->pixel_density, mode->refresh_rate, bpp, SDL_GetPixelFormatName(mode->format));
	}

	SDL_free(modes);
}

static void private_list_displays(ecs_world_t *world)
{
	SDL_DisplayID *displays;
	SDL_Rect bounds, usablebounds;
	int bpp;
	Uint32 Rmask, Gmask, Bmask, Amask;
#ifdef SDL_VIDEO_DRIVER_WINDOWS
	int adapterIndex = 0;
	int outputIndex = 0;
#endif
	int n;
	displays = SDL_GetDisplays(&n);
	SDL_Log("Number of displays: %d\n", n);
	for (int i = 0; i < n; ++i) {
		SDL_DisplayID displayID = displays[i];
		SDL_Log("Display %" SDL_PRIu32 ": %s\n", displayID, SDL_GetDisplayName(displayID));

		char buf[64];
		snprintf(buf, sizeof(buf), "Display %i", displayID);
		ecs_entity_t e = ecs_entity_init(world, &(ecs_entity_desc_t){.name = buf});
		ecs_doc_set_name(world, e, SDL_GetDisplayName(displayID));

		SDL_zero(bounds);
		SDL_GetDisplayBounds(displayID, &bounds);

		SDL_zero(usablebounds);
		SDL_GetDisplayUsableBounds(displayID, &usablebounds);

		SDL_Log("Bounds: %dx%d at %d,%d\n", bounds.w, bounds.h, bounds.x, bounds.y);
		SDL_Log("Usable bounds: %dx%d at %d,%d\n", usablebounds.w, usablebounds.h, usablebounds.x, usablebounds.y);

		const SDL_DisplayMode *mode = SDL_GetDesktopDisplayMode(displayID);
		SDL_GetMasksForPixelFormat(mode->format, &bpp, &Rmask, &Gmask,
		&Bmask, &Amask);
		SDL_Log("  Desktop mode: %dx%d@%gx %gHz, %d bits-per-pixel (%s)\n",
		mode->w, mode->h, mode->pixel_density, mode->refresh_rate, bpp,
		SDL_GetPixelFormatName(mode->format));
		if (Rmask || Gmask || Bmask) {
			SDL_Log("      Red Mask   = 0x%.8" SDL_PRIx32 "\n", Rmask);
			SDL_Log("      Green Mask = 0x%.8" SDL_PRIx32 "\n", Gmask);
			SDL_Log("      Blue Mask  = 0x%.8" SDL_PRIx32 "\n", Bmask);
			if (Amask) {
				SDL_Log("      Alpha Mask = 0x%.8" SDL_PRIx32 "\n", Amask);
			}
		}

		private_list_modes(world, e, displayID);

#if defined(SDL_VIDEO_DRIVER_WINDOWS) && !defined(SDL_PLATFORM_XBOXONE) && !defined(SDL_PLATFORM_XBOXSERIES)
		/* Print the D3D9 adapter index */
		adapterIndex = SDL_GetDirect3D9AdapterIndex(displayID);
		SDL_Log("D3D9 Adapter Index: %d", adapterIndex);

		/* Print the DXGI adapter and output indices */
		SDL_GetDXGIOutputInfo(displayID, &adapterIndex, &outputIndex);
		SDL_Log("DXGI Adapter Index: %d  Output Index: %d", adapterIndex, outputIndex);
#endif
	}
	SDL_free(displays);
}

void EgDisplayImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgDisplay);
	ecs_set_name_prefix(world, "EgDisplay");
	private_list_displays(world);
}
