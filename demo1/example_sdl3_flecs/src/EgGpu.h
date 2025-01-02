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
} EgGpuPipelineCreateInfo;


typedef struct
{
	ecs_entity_t instance;
	uint32_t num_vertices;
	uint32_t num_instances;
	uint32_t first_vertex;
	uint32_t first_instance;
} EgGpuDrawPrimitive;


typedef struct
{
	void * object;
	uint32_t size;
} EgGpuBufferVertex;

typedef struct
{
	void * object;
	uint32_t size;
} EgGpuBufferIndex;

typedef struct
{
	void * object;
	uint32_t size;
} EgGpuBufferTransfer;

typedef struct
{
	uint32_t size;
	bool is_vertex;
	bool is_index;
	bool is_transfer;
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

typedef struct
{
	int32_t location;
} EgGpuLocation;






extern ECS_TAG_DECLARE(EgGpuVertexFormat);

extern ECS_COMPONENT_DECLARE(EgGpuDevice);
extern ECS_COMPONENT_DECLARE(EgGpuWindow);
extern ECS_COMPONENT_DECLARE(EgGpuDeviceCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuShaderVertex);
extern ECS_COMPONENT_DECLARE(EgGpuShaderVertexCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuShaderFragment);
extern ECS_COMPONENT_DECLARE(EgGpuShaderFragmentCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuPipeline);
extern ECS_COMPONENT_DECLARE(EgGpuPipelineCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuDrawPrimitive);
extern ECS_COMPONENT_DECLARE(EgGpuBufferVertex);
extern ECS_COMPONENT_DECLARE(EgGpuBufferIndex);
extern ECS_COMPONENT_DECLARE(EgGpuBufferTransfer);
extern ECS_COMPONENT_DECLARE(EgGpuBufferCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuTexture);
extern ECS_COMPONENT_DECLARE(EgGpuTextureCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuDraw1);
extern ECS_COMPONENT_DECLARE(EgGpuLocation);

void EgGpuImport(ecs_world_t *world);


