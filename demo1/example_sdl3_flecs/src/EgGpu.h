#pragma once
#include <flecs.h>


typedef struct
{
	bool debug;
} EgGpuDeviceCreateInfo;
typedef struct
{
	void * device;
} EgGpuDevice;

typedef struct
{
	uint32_t windows;
	uint32_t textures;
	uint32_t buffers;
	uint32_t shaders;
	uint32_t pipelines;
} EgGpuStats;

typedef struct
{
	EgGpuStats max;
	EgGpuStats sum;
} EgGpuDeviceDebug;

typedef struct
{
	int dummy;
} EgGpuWindow;


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
	uint32_t cap;
	uint32_t last;
} EgGpuBufferVertex;

typedef struct
{
	void * object;
	uint32_t cap;
	uint32_t last;
} EgGpuBufferIndex;

typedef struct
{
	void * object;
	uint32_t cap;
	uint32_t last;
	void * mapped;
} EgGpuBufferTransfer;

typedef struct
{
	uint32_t cap;
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

typedef struct
{
	int32_t dummy;
} EgGpuTransferCreateInfo;

typedef struct
{
	void *dst; // SDL_GPUBuffer
	uint32_t src_offset;
	uint32_t dst_offset;
	uint32_t size;
} EgGpuTransferCmd;

#define EG_GPU_TRANSFER_MAX_CMDS 16
typedef struct
{
	ecs_query_t * query1;
	uint8_t *data;
	uint32_t cmd_last;
	EgGpuTransferCmd cmd[EG_GPU_TRANSFER_MAX_CMDS];
} EgGpuTransfer;






extern ECS_TAG_DECLARE(EgGpuVertexFormat);

extern ECS_COMPONENT_DECLARE(EgGpuDevice);
extern ECS_COMPONENT_DECLARE(EgGpuWindow);
extern ECS_COMPONENT_DECLARE(EgGpuStats);
extern ECS_COMPONENT_DECLARE(EgGpuDeviceCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuDeviceDebug);
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
extern ECS_COMPONENT_DECLARE(EgGpuTransferCreateInfo);
extern ECS_COMPONENT_DECLARE(EgGpuTransferCmd);
extern ECS_COMPONENT_DECLARE(EgGpuTransfer);


void EgGpuImport(ecs_world_t *world);


