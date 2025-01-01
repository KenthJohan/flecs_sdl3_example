#include "EgGpu_System_EgGpuPipeline.h"
#include "EgGpu.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <EgBase.h>

#include "shader_spirv.h"

/**
 * Specifies the format of a vertex attribute.
 *
 * \since This enum is available since SDL 3.1.3
 *
 * \sa SDL_CreateGPUGraphicsPipeline

typedef enum SDL_GPUVertexElementFormat
{
    SDL_GPU_VERTEXELEMENTFORMAT_INVALID,

    // 32-bit Signed Integers
    SDL_GPU_VERTEXELEMENTFORMAT_INT,
    SDL_GPU_VERTEXELEMENTFORMAT_INT2,
    SDL_GPU_VERTEXELEMENTFORMAT_INT3,
    SDL_GPU_VERTEXELEMENTFORMAT_INT4,

    // 32-bit Unsigned Integers
    SDL_GPU_VERTEXELEMENTFORMAT_UINT,
    SDL_GPU_VERTEXELEMENTFORMAT_UINT2,
    SDL_GPU_VERTEXELEMENTFORMAT_UINT3,
    SDL_GPU_VERTEXELEMENTFORMAT_UINT4,

    // 32-bit Floats
    SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
    SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
    SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
    SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,

    // 8-bit Signed Integers
    SDL_GPU_VERTEXELEMENTFORMAT_BYTE2,
    SDL_GPU_VERTEXELEMENTFORMAT_BYTE4,

    // 8-bit Unsigned Integers
    SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2,
    SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4,

    // 8-bit Signed Normalized
    SDL_GPU_VERTEXELEMENTFORMAT_BYTE2_NORM,
    SDL_GPU_VERTEXELEMENTFORMAT_BYTE4_NORM,

    // 8-bit Unsigned Normalized
    SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2_NORM,
    SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,

    // 16-bit Signed Integers
    SDL_GPU_VERTEXELEMENTFORMAT_SHORT2,
    SDL_GPU_VERTEXELEMENTFORMAT_SHORT4,

    // 16-bit Unsigned Integers
    SDL_GPU_VERTEXELEMENTFORMAT_USHORT2,
    SDL_GPU_VERTEXELEMENTFORMAT_USHORT4,

    // 16-bit Signed Normalized
    SDL_GPU_VERTEXELEMENTFORMAT_SHORT2_NORM,
    SDL_GPU_VERTEXELEMENTFORMAT_SHORT4_NORM,

    // 16-bit Unsigned Normalized
    SDL_GPU_VERTEXELEMENTFORMAT_USHORT2_NORM,
    SDL_GPU_VERTEXELEMENTFORMAT_USHORT4_NORM,

    // 16-bit Floats
    SDL_GPU_VERTEXELEMENTFORMAT_HALF2,
    SDL_GPU_VERTEXELEMENTFORMAT_HALF4
} SDL_GPUVertexElementFormat;
 */

static int iterate_attributes(ecs_world_t *world, ecs_entity_t parent, SDL_GPUVertexAttribute *out_attr, int out_attr_length)
{
	// Iterate children of parent entity
	ecs_iter_t it = ecs_children(world, parent);
	int j = 0;
	while (ecs_children_next(&it)) {
		if (it.count > out_attr_length) {
			ecs_warn("Too many attributes");
			return -1;
		}
		// Iterate entities in scope
		for (int i = 0; i < it.count; i++) {
			ecs_entity_t e = it.entities[i];
			// Get member component from entity
			EcsMember const *member = ecs_get(world, e, EcsMember);
			EgGpuLocation const *location = ecs_get(world, e, EgGpuLocation);
			if (member == NULL) {
				return -1;
			}
			if (member->count <= 0) {
				ecs_warn("Too little elements");
				return -1;
			}

			if (member->count > 4) {
				ecs_warn("Too many elements");
				return -1;
			}

			if (member->type == ecs_id(ecs_f32_t) && (member->count <= 4)) {
				out_attr->format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT + member->count - 1;
			} else if (member->type == ecs_id(ecs_i32_t) && (member->count <= 4)) {
				out_attr->format = SDL_GPU_VERTEXELEMENTFORMAT_INT + member->count - 1;
			} else if (member->type == ecs_id(ecs_u32_t) && (member->count <= 4)) {
				out_attr->format = SDL_GPU_VERTEXELEMENTFORMAT_UINT + member->count - 1;
			} else if (member->type == ecs_id(ecs_i8_t) && (member->count == 2)) {
				out_attr->format = SDL_GPU_VERTEXELEMENTFORMAT_BYTE2;
			} else if (member->type == ecs_id(ecs_i8_t) && (member->count == 4)) {
				out_attr->format = SDL_GPU_VERTEXELEMENTFORMAT_BYTE4;
			} else if (member->type == ecs_id(ecs_u8_t) && (member->count == 2)) {
				out_attr->format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2;
			} else if (member->type == ecs_id(ecs_u8_t) && (member->count == 4)) {
				out_attr->format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4;
			}

			out_attr->offset = member->offset;
			out_attr->location = location->location;
			out_attr->buffer_slot = 0;
			out_attr++;
			j++;
		}
	}
	return j;
}

#define MAX_ATRTIBUTES 8

void System_EgGpuPipeline_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	ecs_log_set_level(1);
	ecs_dbg("System_EgGpuPipeline_Create() count:%i", it->count);
	ecs_log_push_1();

	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		ecs_remove(world, e, EgBaseUpdate);
		// Entities can be annotated with the Final trait, which prevents using them with IsA relationship.
		ecs_add_id(world, e, EcsFinal);
	}

	EgGpuDevice *gpu = ecs_field(it, EgGpuDevice, 0);                                // shared, parent
	EgGpuPipelineCreateInfo *field_info = ecs_field(it, EgGpuPipelineCreateInfo, 1); // self
	EgGpuShaderVertex *field_svertex = ecs_field(it, EgGpuShaderVertex, 2);          // shared
	EgGpuShaderFragment *field_sfragment = ecs_field(it, EgGpuShaderFragment, 3);    // shared
	EcsComponent *field_component = ecs_field(it, EcsComponent, 4);                  // shared
	ecs_entity_t field_component_src_entity = ecs_field_src(it, 4);                  // shared

	for (int i = 0; i < it->count; ++i, ++field_info) {
		ecs_entity_t e = it->entities[i];
		ecs_dbg("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_1();
		{
			SDL_GPUColorTargetDescription color_target_desc = {0};
			SDL_GPUVertexAttribute vertex_attributes[MAX_ATRTIBUTES] = {0};
			SDL_GPUVertexBufferDescription vertex_buffer_desc = {0};
			SDL_GPUGraphicsPipelineCreateInfo pipelinedesc = {0};
			// color_target_desc.format = SDL_GetGPUSwapchainTextureFormat(gpu->device, state->windows[0]);
			color_target_desc.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
			pipelinedesc.target_info.num_color_targets = 1;
			pipelinedesc.target_info.color_target_descriptions = &color_target_desc;
			pipelinedesc.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
			pipelinedesc.target_info.has_depth_stencil_target = true;

			pipelinedesc.depth_stencil_state.enable_depth_test = true;
			pipelinedesc.depth_stencil_state.enable_depth_write = true;
			pipelinedesc.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL;

			pipelinedesc.multisample_state.sample_count = field_info[i].sample_count;

			pipelinedesc.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

			pipelinedesc.vertex_shader = field_svertex[i].object;
			pipelinedesc.fragment_shader = field_sfragment[i].object;

			vertex_buffer_desc.slot = 0;
			vertex_buffer_desc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
			vertex_buffer_desc.instance_step_rate = 0;
			vertex_buffer_desc.pitch = field_component->size;

			/*
			vertex_attributes[0].location = 0;
			vertex_attributes[1].location = 0;
			vertex_attributes[0].buffer_slot = 0;
			vertex_attributes[1].buffer_slot = 0;
			vertex_attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
			vertex_attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
			vertex_attributes[0].offset = 0;
			vertex_attributes[1].offset = sizeof(float) * 3;
			*/

			pipelinedesc.vertex_input_state.num_vertex_buffers = 1;
			pipelinedesc.vertex_input_state.vertex_buffer_descriptions = &vertex_buffer_desc;

			{
				int n = iterate_attributes(world, field_component_src_entity, vertex_attributes, MAX_ATRTIBUTES);
				if (n < 0) {
					ecs_err("iterate_attributes() failed");
					ecs_add(world, e, EgBaseError);
					continue;
				}
				pipelinedesc.vertex_input_state.num_vertex_attributes = n;
			}

			pipelinedesc.vertex_input_state.vertex_attributes = (SDL_GPUVertexAttribute *)&vertex_attributes;

			pipelinedesc.props = 0;

			SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(gpu->device, &pipelinedesc);
			if (pipeline == NULL) {
				ecs_err("SDL_CreateGPUGraphicsPipeline() failed");
				ecs_add(world, e, EgBaseError);
				continue;
			}
			ecs_set(world, e, EgGpuPipeline, {.object = pipeline});
			ecs_dbg("SDL_CreateGPUGraphicsPipeline(%p) -> %p", gpu->device, pipeline);
		}
		ecs_log_pop_1();
	} // END FOR LOOP
	ecs_log_pop_1();
	ecs_log_set_level(0);
}