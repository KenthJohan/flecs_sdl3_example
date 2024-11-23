#include "egmisc/eg_file.h"
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <flecs.h>

char *eg_file_load_alloc(const char *filename, size_t * length)
{
	FILE *file;
	char *content = NULL;
	int32_t bytes;
	size_t size;

	/* Open file for reading */
	ecs_os_fopen(&file, filename, "r");
	if (!file) {
		ecs_err("%s (%s)", ecs_os_strerror(errno), filename);
		goto error;
	}

	/* Determine file size */
	fseek(file, 0, SEEK_END);
	bytes = (int32_t)ftell(file);
	if (bytes == -1) {
		goto error;
	}
	*length = bytes;
	fseek(file, 0, SEEK_SET);

	/* Load contents in memory */
	content = ecs_os_malloc(bytes + 1);
	size = (size_t)bytes;
	if (!(size = fread(content, 1, size, file)) && bytes) {
		ecs_err("%s: read zero bytes instead of %d", filename, size);
		ecs_os_free(content);
		content = NULL;
		goto error;
	} else {
		content[size] = '\0';
	}

	fclose(file);

	return content;
error:
	ecs_os_free(content);
	return NULL;
}