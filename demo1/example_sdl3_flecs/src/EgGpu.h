#pragma once
#include <flecs.h>


typedef struct
{
	void * device;
} EgGpuDevice;

typedef struct
{
	int dummy;
} EgGpuWindow;

typedef struct
{
	bool debug;
} EgGpuDeviceCreateInfo;

typedef struct
{
	void * object;
} EgGpuShaderVertex;

typedef struct
{
	void * object;
} EgGpuShaderFragment;

typedef struct
{
	uint32_t stage;
} EgGpuShaderVertexCreateInfo;

typedef struct
{
	uint32_t stage;
} EgGpuShaderFragmentCreateInfo;

typedef struct
{
	void * object;
} EgGpuPipeline;

typedef struct
{
	uint32_t sample_count;
	bool target_info_has_depth_stencil_target;
	uint32_t pitch;
} EgGpuPipelineCreateInfo;


typedef struct
{
	int dummy;
} EgGpuDrawCube;


typedef struct
{
	void * object;
} EgGpuBuffer;

typedef struct
{
	uint32_t usage;
} EgGpuBufferCreateInfo;


typedef struct
{
	void * object;
} EgGpuTexture;

typedef struct
{
	uint32_t sample_count;
} EgGpuTextureCreateInfo;

typedef struct
{
	ecs_query_t * query;
} EgGpuDraw1;



extern ECS_COMPONENT_DECLARE(EgGpuDevice);
extern ECS_COMPONENT_DECLARE(EgGpuWindow);
extern ECS_COMPONENT_DECLARE(EgGpuDeviceCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuShaderVertex);
extern ECS_COMPONENT_DECLARE(EgGpuShaderVertexCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuShaderFragment);
extern ECS_COMPONENT_DECLARE(EgGpuShaderFragmentCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuPipeline);
extern ECS_COMPONENT_DECLARE(EgGpuPipelineCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuDrawCube);
extern ECS_COMPONENT_DECLARE(EgGpuBuffer);
extern ECS_COMPONENT_DECLARE(EgGpuBufferCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuTexture);
extern ECS_COMPONENT_DECLARE(EgGpuTextureCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuDraw1);

void EgGpuImport(ecs_world_t *world);


