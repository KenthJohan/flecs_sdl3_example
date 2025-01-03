#include <SDL3/SDL_gpu.h>
#include <flecs.h>

typedef struct {
	SDL_GPUDevice *device;
	SDL_GPUBuffer* vb;
	SDL_GPUBuffer* ib;
	SDL_GPUTransferBuffer* tb;
} gpu_transfer_info_t;


void upload1(ecs_iter_t *it)
{
}