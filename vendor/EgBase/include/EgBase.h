#pragma once
#include <flecs.h>



extern ECS_TAG_DECLARE(EgBaseUpdate);
extern ECS_TAG_DECLARE(EgBaseError);


#define ecs_field_paranoid(it, T, index)\
    (ecs_field_id(it, index) == ecs_id(T)) ? (ECS_CAST(T*, ecs_field_w_size(it, sizeof(T), index))) : \
    (ecs_abort_(ECS_INVALID_PARAMETER, __FILE__, __LINE__, \
    "Field %i (%s) does not match %s", \
    index, ecs_get_symbol(it->world, ecs_field_id(it, index)), ecs_get_symbol(it->world, ecs_id(T))), \
    ecs_os_abort(), NULL)

#undef ecs_field
#define ecs_field(it, T, index) ecs_field_paranoid(it, T, index)


void EgBaseImport(ecs_world_t *world);