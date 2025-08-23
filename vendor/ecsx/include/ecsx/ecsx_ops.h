#pragma once
#include <flecs.h>


const char *ecsx_meta_type_op_kind_str(ecs_meta_op_kind_t kind);
char const * ecsx_meta_type_op_kind_str1(ecs_meta_op_kind_t kind);

int ecsx_ops_print(ecs_world_t * world, ecs_entity_t type);