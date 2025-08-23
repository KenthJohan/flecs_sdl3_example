#include "shader_spirv.h"
#include <stdio.h>
#include <stdint.h>
#include <flecs.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <SDL3/SDL_gpu.h>

#include <egmisc/eg_popen.h>
#include <egmisc/eg_file.h>



static int static_glslangValidator(const char *namepart, const char *type, char const * ofile)
{
	char buffer[128];
	// int len = snprintf(buffer, sizeof(buffer), "glslangValidator %s.%s.glsl -V -S %s -o %s.%s.spv --quiet", namepart, type, type, namepart, type);
	int len = snprintf(buffer, sizeof(buffer), "glslangValidator %s.%s.glsl -V -S %s -o %s", namepart, type, type, ofile);
	if (len < 0) {
		ecs_err("sprintf error");
		return -1;
	}
	if ((size_t)len > sizeof(buffer)) {
		ecs_err("command buffer to small");
		return -1;
	}

	ecs_strbuf_t str = ECS_STRBUF_INIT;
	int r;
	int exit_code = 0;
	r = eg_popen(buffer, &exit_code, &str);

	char *strval = ecs_strbuf_get(&str);
	if(strval) {
		fputs(strval, stdout);
		ecs_os_free(strval);
	} else {
		ecs_err("ecs_strbuf_get() error");
	}


	return r;
}


static int static_spv_make_path(char * buffer, size_t buflen, const char *namepart, const char *type)
{
	int len = snprintf(buffer, buflen, "%s.%s.spv", namepart, type);
	if (len < 0) {
		ecs_err("sprintf error");
		return -1;
	}
	if ((size_t)len > buflen) {
		ecs_err("command buffer to small");
		return -1;
	}
	return len;
}




SDL_GPUShader * shader_spirv_compile(SDL_GPUDevice *device, const char *namepart, SDL_GPUShaderStage stage)
{
	SDL_GPUShaderFormat format = SDL_GetGPUShaderFormats(device);
	if (format != SDL_GPU_SHADERFORMAT_SPIRV) {
		fprintf(stderr, "Format is not SDL_GPU_SHADERFORMAT_SPIRV");
		return NULL;
	}

	char ofilepath[128];
	char const * type = NULL;
	switch (stage)
	{
	case SDL_GPU_SHADERSTAGE_VERTEX:
		type = "vert";
		break;
	case SDL_GPU_SHADERSTAGE_FRAGMENT:
		type = "frag";
		break;
	default:
		break;
	}
	int r;
	r = static_spv_make_path(ofilepath, sizeof(ofilepath), namepart, type);
	if (r < 0) {
		return NULL;
	}
	r = static_glslangValidator(namepart, type, ofilepath);
	if (r < 0) {
		return NULL;
	}

	size_t length = 0;
	char const *alloc_code = eg_file_load_alloc(ofilepath, &length);
	if (alloc_code == NULL) {
		return NULL;
	}

	SDL_GPUShaderCreateInfo createinfo = {0};
	createinfo.num_samplers = 0;
	createinfo.num_storage_buffers = 0;
	createinfo.num_storage_textures = 0;
	createinfo.num_uniform_buffers = (stage == SDL_GPU_SHADERSTAGE_VERTEX);
	createinfo.props = 0;
	createinfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
	createinfo.code = (uint8_t const *)alloc_code;
	createinfo.code_size = length;
	createinfo.entrypoint = "main";
	createinfo.stage = stage;
	SDL_GPUShader * s = SDL_CreateGPUShader(device, &createinfo);
	ecs_os_free((void*)alloc_code);

	if (s == NULL) {
		ecs_err("SDL_CreateGPUShader() failed");
		return NULL;
	}
	return s;
}







