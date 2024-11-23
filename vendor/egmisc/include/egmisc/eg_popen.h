#pragma once
#include <flecs.h>

int eg_popen(const char *command, int *out_exit_code, ecs_strbuf_t *str);