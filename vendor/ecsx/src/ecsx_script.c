
// #define _GNU_SOURCE /* To get defns of NI_MAXSERV and NI_MAXHOST */

// Fixes: error: variable ‘ifr’ has initializer but incomplete type
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif


#include "ecsx/ecsx_script.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

int dirlists_get(void)
{
	DIR *d;
	struct dirent *dir;
	d = opendir("./config");
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			printf("%s\n", dir->d_name);
		}
		closedir(d);
	}
	return (0);
}

int ecs_plecs_from_dir(ecs_world_t *world, char const *dirpath)
{
	int rc = -1;
	struct dirent **namelist;
	int n;
	ecs_log(0, "scandir: %s", dirpath);
	n = scandir(dirpath, &namelist, 0, alphasort);
	if (n < 0) {
		ecs_err("scandir");
		return -1;
	}
	ecs_log_push();
	for (int i = 0; i < n; ++i) {
		char const *name = namelist[i]->d_name;
		char buf[512] = {0};
		snprintf(buf, 512, "%s/%s", dirpath, name);
		char const *ext = strstr(name, ".flecs");
		if (ext) {
			ecs_log(0, "ecs_plecs_from_file: %s", buf);
			rc = ecs_script_run_file(world, buf);
		}
		free(namelist[i]);
	}
	ecs_log_pop();
	free(namelist);

	return rc;
}
