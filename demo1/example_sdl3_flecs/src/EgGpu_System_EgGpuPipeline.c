#include "EgGpu_System_EgGpuPipeline.h"
#include "EgGpu.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL_log.h>
#include <SDL3/SDL_gpu.h>
#include <EgBase.h>

#include "shader_spirv.h"


void System_EgGpuPipeline_Create(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgGpuDevice *gpu = ecs_field(it, EgGpuDevice, 0);
	EgGpuPipelineCreateInfo *create = ecs_field(it, EgGpuPipelineCreateInfo, 1);
	EgGpuShaderVertex *sv = ecs_field(it, EgGpuShaderVertex, 2);
	EgGpuShaderFragment *sf = ecs_field(it, EgGpuShaderFragment, 3);
	ecs_log_set_level(1);
	ecs_dbg("System_EgGpuPipeline_Create() count:%i", it->count);
	ecs_log_push_1();
	for (int i = 0; i < it->count; ++i, ++create) {
		ecs_entity_t e = it->entities[i];
		ecs_remove(world, e, EgBaseUpdate);

		ecs_dbg("Entity: '%s'", ecs_get_name(world, e));
		ecs_log_push_1();
		{
			SDL_GPUColorTargetDescription color_target_desc = {0};
			SDL_GPUVertexAttribute vertex_attributes[2] = {0};
			SDL_GPUVertexBufferDescription vertex_buffer_desc = {0};
			SDL_GPUGraphicsPipelineCreateInfo pipelinedesc = {0};
			//color_target_desc.format = SDL_GetGPUSwapchainTextureFormat(gpu->device, state->windows[0]);
			color_target_desc.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
			pipelinedesc.target_info.num_color_targets = 1;
			pipelinedesc.target_info.color_target_descriptions = &color_target_desc;
			pipelinedesc.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D16_UNORM;
			pipelinedesc.target_info.has_depth_stencil_target = true;

			pipelinedesc.depth_stencil_state.enable_depth_test = true;
			pipelinedesc.depth_stencil_state.enable_depth_write = true;
			pipelinedesc.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL;

			pipelinedesc.multisample_state.sample_count = create[i].sample_count;

			pipelinedesc.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

			pipelinedesc.vertex_shader = sv[i].object;
			pipelinedesc.fragment_shader = sf[i].object;

			vertex_buffer_desc.slot = 0;
			vertex_buffer_desc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
			vertex_buffer_desc.instance_step_rate = 0;
			vertex_buffer_desc.pitch = create->pitch;

			vertex_attributes[0].buffer_slot = 0;
			vertex_attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
			vertex_attributes[0].location = 0;
			vertex_attributes[0].offset = 0;

			vertex_attributes[1].buffer_slot = 0;
			vertex_attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
			vertex_attributes[1].location = 1;
			vertex_attributes[1].offset = sizeof(float) * 3;

			pipelinedesc.vertex_input_state.num_vertex_buffers = 1;
			pipelinedesc.vertex_input_state.vertex_buffer_descriptions = &vertex_buffer_desc;
			pipelinedesc.vertex_input_state.num_vertex_attributes = 2;
			pipelinedesc.vertex_input_state.vertex_attributes = (SDL_GPUVertexAttribute *)&vertex_attributes;

			pipelinedesc.props = 0;

			SDL_GPUGraphicsPipeline *pipeline = SDL_CreateGPUGraphicsPipeline(gpu->device, &pipelinedesc);
			if (pipeline == NULL) {
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