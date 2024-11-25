#include "EgCameras.h"

#include <EgSpatials.h>
#include <EgWindows.h>
#include <EgShapes.h>
#include <egmath.h>
#include <assert.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

ECS_COMPONENT_DECLARE(EgCamerasState);
ECS_COMPONENT_DECLARE(EgCamerasKeyBindings);

static void CameraUpdate(ecs_iter_t *it)
{
	EgCamerasState *cam = ecs_field(it, EgCamerasState, 0);      // self
	Position3 *pos = ecs_field(it, Position3, 1);                // self
	Orientation *o = ecs_field(it, Orientation, 2);              // self
	EgShapesRectangle *cr = ecs_field(it, EgShapesRectangle, 3); // shared

	for (int i = 0; i < it->count; ++i, ++cam, ++pos, ++o) {

		float rad2deg = (2.0f * M_PI) / 360.0f;
		float aspect = cr->w / cr->h;
		m4f32_perspective1(&cam->projection, cam->fov * rad2deg, aspect, 0.01f, 10000.0f);

		// Apply translation (t), rotation (r), projection - which creates the view-projection-matrix (vp).
		// The view-projection-matrix can then be later used in shaders.

		m4f32 t = M4_IDENTITY;
		m4f32_translation3(&t, (float *)pos);

		m4f32 r = M4_IDENTITY;
		qf32_unit_to_m4((float *)o, &r);

		// printf("Camera:\n");
		// m4f32_print(&v);
		m4f32_mul(&cam->view, &r, &t);
		// m4f32_print(&v);

		m4f32_mul(&cam->vp, &cam->projection, &cam->view);

		// printf("Camera:\n");
		// v4f32_print(o->q);
	}
}

/*
static void MouseRayCast(ecs_iter_t *it)
{
    Window *win = ecs_field(it, Window, 0);                          // shared
    EgCamerasState *cam = ecs_field(it, EgCamerasState, 1);          // self
    Position3 *pos = ecs_field(it, Position3, 2);                    // self
    Ray3 *ray1 = ecs_field(it, Ray3, 3);                             // self
    EgWindowsMouseRay *winray = ecs_field(it, EgWindowsMouseRay, 4); // self

    win->dt = it->delta_time;
    win->fps = 1.0f / it->delta_time;

    for (int i = 0; i < it->count; ++i, ++cam, ++pos, ++ray1, ++winray) {

        // TODO: What is this? Remove this:
        win->pos[0] = pos->x;
        win->pos[1] = pos->y;
        win->pos[2] = pos->z;

        // Normalize to mouse position to (-1 .. 1)
        float mouse_pos[2] = {win->canvas_mouse_x, win->canvas_mouse_y};
        float rectangle[2] = {win->canvas_width, win->canvas_height};
        float r[4];
        r[0] = 2.0f * (mouse_pos[0] / rectangle[0]) - 1.0f;
        r[1] = 2.0f * (mouse_pos[1] / rectangle[1]) - 1.0f;
        r[1] *= -1.0f; // Why flip, hmm?
        r[2] = -1.0;
        r[3] = 1.0;

        // Eye/Camera:
        float ray_eye[4];
        m4f32 pinv;
        m4f32_inverse((float *)&cam->projection, (float *)&pinv);
        m4f32_mulv(&pinv, r, ray_eye);
        ray_eye[2] = -1.0f;
        ray_eye[3] = 0.0f;

        // Convert to world coordinates:
        m4f32 vinv;
        float ray_world[4];
        m4f32_inverse((float *)&cam->view, (float *)&vinv);
        m4f32_mulv(&vinv, ray_eye, ray_world);

        ray1->x = ray_world[0];
        ray1->y = ray_world[1];
        ray1->z = ray_world[2];
    }
}
*/

ECS_CTOR(EgCamerasState, ptr, {
	ptr->fov = 45;
})

void EgCamerasImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgCameras);
	ECS_IMPORT(world, EgSpatials);
	ECS_IMPORT(world, EgShapes);
	ECS_IMPORT(world, EgWindows);
	ecs_set_name_prefix(world, "EgCameras");

	ECS_COMPONENT_DEFINE(world, EgCamerasState);
	ECS_COMPONENT_DEFINE(world, EgCamerasKeyBindings);

	ecs_set_hooks(world, EgCamerasState, {.ctor = ecs_ctor(EgCamerasState)});

	ecs_struct(world,
	{.entity = ecs_id(EgCamerasState),
	.members = {
	{.name = "fov", .type = ecs_id(ecs_f32_t)},
	{.name = "view", .type = ecs_id(Transformation)},
	{.name = "projection", .type = ecs_id(Transformation)},
	{.name = "vp", .type = ecs_id(Transformation)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EgCamerasKeyBindings),
	.members = {
	{.name = "key_rotate_dx_plus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_rotate_dx_minus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_rotate_dy_plus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_rotate_dy_minus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_rotate_dz_plus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_rotate_dz_minus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_move_dx_plus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_move_dx_minus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_move_dy_plus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_move_dy_minus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_move_dz_plus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_move_dz_minus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_move_dz_minus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_fov_reset", .type = ecs_id(ecs_u16_t)},
	{.name = "key_fov_plus", .type = ecs_id(ecs_u16_t)},
	{.name = "key_fov_minus", .type = ecs_id(ecs_u16_t)},
	}});

	ecs_system(world, {.entity = ecs_entity(world, {.name = "CameraUpdate", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	                  .callback = CameraUpdate,
	                  .query.terms = {
	                  {.id = ecs_id(EgCamerasState), .src.id = EcsSelf},
	                  {.id = ecs_id(Position3), .src.id = EcsSelf},
	                  {.id = ecs_id(Orientation), .src.id = EcsSelf},
	                  {.id = ecs_id(EgShapesRectangle), .trav = EcsDependsOn, .src.id = EcsUp},
	                  }});

	/*
	    ecs_system(world, {.entity = ecs_entity(world, {.name = "MouseRayCast", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	                      .callback = MouseRayCast,
	                      .query.terms =
	                      {
	                      {.id = ecs_id(Window), .src.id = ecs_id(Window)},
	                      {.id = ecs_id(EgCamerasState), .src.id = EcsSelf},
	                      {.id = ecs_id(Position3), .src.id = EcsSelf},
	                      {.id = ecs_id(Ray3), .src.id = EcsSelf},
	                      {.id = ecs_id(EgWindowsMouseRay), .src.id = EcsSelf},
	                      }});
	                      */
}