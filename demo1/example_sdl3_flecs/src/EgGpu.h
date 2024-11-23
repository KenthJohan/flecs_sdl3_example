#pragma once
#include <flecs.h>


typedef struct
{
	void * device;
} EgGpuDevice;

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

extern ECS_COMPONENT_DECLARE(EgGpuDevice);
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

void EgGpuImport(ecs_world_t *world);


