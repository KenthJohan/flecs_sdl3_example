#include "EgShapes.h"

ECS_COMPONENT_DECLARE(EgShapesBox);
ECS_COMPONENT_DECLARE(EgShapesTorus);
ECS_COMPONENT_DECLARE(EgShapesCylinder);
ECS_COMPONENT_DECLARE(EgShapesSphere);
ECS_COMPONENT_DECLARE(EgShapesLine);
ECS_COMPONENT_DECLARE(EgShapesRectangle);
ECS_COMPONENT_DECLARE(EgShapesTriangle);

void EgShapesImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgShapes);
	ecs_set_name_prefix(world, "EgShapes");

	ECS_COMPONENT_DEFINE(world, EgShapesBox);
	ECS_COMPONENT_DEFINE(world, EgShapesTorus);
	ECS_COMPONENT_DEFINE(world, EgShapesCylinder);
	ECS_COMPONENT_DEFINE(world, EgShapesSphere);
	ECS_COMPONENT_DEFINE(world, EgShapesLine);
	ECS_COMPONENT_DEFINE(world, EgShapesRectangle);
	ECS_COMPONENT_DEFINE(world, EgShapesTriangle);

	ecs_struct(world,
	{.entity = ecs_id(EgShapesBox),
	.members = {
	{.name = "width", .type = ecs_id(ecs_f32_t)},
	{.name = "height", .type = ecs_id(ecs_f32_t)},
	{.name = "depth", .type = ecs_id(ecs_f32_t)},
	{.name = "random_colors", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgShapesLine),
	.members = {
	{.name = "a", .type = ecs_id(ecs_f32_t), .count = 3},
	{.name = "b", .type = ecs_id(ecs_f32_t), .count = 3},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgShapesTorus),
	.members = {
	{.name = "radius", .type = ecs_id(ecs_f32_t)},
	{.name = "ring_radius", .type = ecs_id(ecs_f32_t)},
	{.name = "rings", .type = ecs_id(ecs_i32_t)},
	{.name = "sides", .type = ecs_id(ecs_i32_t)},
	{.name = "random_colors", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgShapesCylinder),
	.members = {
	{.name = "radius", .type = ecs_id(ecs_f32_t)},
	{.name = "height", .type = ecs_id(ecs_f32_t)},
	{.name = "slices", .type = ecs_id(ecs_i32_t)},
	{.name = "stacks", .type = ecs_id(ecs_i32_t)},
	{.name = "random_colors", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgShapesSphere),
	.members = {
	{.name = "radius", .type = ecs_id(ecs_f32_t)},
	{.name = "slices", .type = ecs_id(ecs_i32_t)},
	{.name = "stacks", .type = ecs_id(ecs_i32_t)},
	{.name = "random_colors", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgShapesRectangle),
	.members = {
	{.name = "w", .type = ecs_id(ecs_f32_t)},
	{.name = "h", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgShapesTriangle),
	.members = {
	{.name = "w", .type = ecs_id(ecs_f32_t)},
	{.name = "h", .type = ecs_id(ecs_f32_t)},
	}});
}