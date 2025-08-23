#include "egmisc/eg_popen.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int eg_popen(const char *command, int *out_exit_code, ecs_strbuf_t *str)
{
	FILE *f = NULL;
	//printf("popen: %s\n", command);
	f = popen(command, "r");
	if (f == NULL) {
		ecs_err("popen() returns NULL");
		return -1;
	}
	char buffer[128];
	while (1) {
		char *r = fgets(buffer, sizeof(buffer) - 1, f);
		if (r == NULL) {
			break;
		}
		if (str) {
			ecs_strbuf_appendstr(str, buffer);
		}
	}
	int status = pclose(f);
	if (!WIFEXITED(status)) {
		ecs_err("Subprocess did not exit normally");
		return -1;
	}
	*out_exit_code = WEXITSTATUS(status);
	if (*out_exit_code != 0) {
		ecs_err("Subprocess exit code not zero");
	}
	return 0;
}