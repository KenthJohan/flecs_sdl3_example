#include "EgSpatials.h"
#include <EgBase.h>

ECS_COMPONENT_DECLARE(Position2);
ECS_COMPONENT_DECLARE(Position3);
ECS_COMPONENT_DECLARE(Vector4);
ECS_COMPONENT_DECLARE(Ray3);
ECS_COMPONENT_DECLARE(Scale3);
ECS_COMPONENT_DECLARE(Position3World);
ECS_COMPONENT_DECLARE(Velocity2);
ECS_COMPONENT_DECLARE(Velocity3);
ECS_COMPONENT_DECLARE(Rotate3);
ECS_COMPONENT_DECLARE(RotMat3);
ECS_COMPONENT_DECLARE(Orientation);
ECS_COMPONENT_DECLARE(OrientationWorld);
ECS_COMPONENT_DECLARE(EulerAngles);
ECS_COMPONENT_DECLARE(Transformation);
ECS_COMPONENT_DECLARE(TransformationCollector);
ECS_COMPONENT_DECLARE(Sinewave);
ECS_COMPONENT_DECLARE(Color3);
ECS_TAG_DECLARE(EgRotateOrder1);
ECS_TAG_DECLARE(EgRotateOrder2);
ECS_TAG_DECLARE(EgPositionWorldNoReset);

static void Position3World_Reset(ecs_iter_t *it)
{
	Position3World *l = ecs_field(it, Position3World, 0); // self, out
	for (int i = 0; i < it->count; ++i, ++l) {
		l[0].x = 0;
		l[0].y = 0;
		l[0].z = 0;
	}
}

static void Orientation_To_RotMat3(ecs_iter_t *it)
{
	RotMat3 *r = ecs_field(it, RotMat3, 0);         // self, out
	Orientation *o = ecs_field(it, Orientation, 1); // self, in
	for (int i = 0; i < it->count; ++i, ++o, ++r) {
		qf32_unit_to_m3((float *)o, (m3f32 *)r);
	}
}

static void Orientation_Cascade(ecs_iter_t *it)
{
	OrientationWorld *g = ecs_field(it, OrientationWorld, 0); // self, out
	Orientation *l = ecs_field(it, Orientation, 1);           // self, in
	OrientationWorld *p = ecs_field(it, OrientationWorld, 2); // parent, in
	for (int i = 0; i < it->count; ++i, ++l, ++g) {
		g->x = l->x;
		g->y = l->y;
		g->z = l->z;
		g->w = l->w;
		if (p) {
			qf32_mul((float *)g, (float const *)p, (float const *)g);
			// qf32_rotate_vector(g, pos, pos);
			// qf32_mul((float*)g, (float const*)g, (float const*)p);
		}
	}
}

static void Position3_Cascade(ecs_iter_t *it)
{
	Position3World *g = ecs_field(it, Position3World, 0);            // self, out
	Position3 const *l = ecs_field(it, Position3, 1);                // self, in
	Position3World const *p = ecs_field(it, Position3World, 2);      // parent, in
	OrientationWorld const *qq = ecs_field(it, OrientationWorld, 3); // parent, in
	for (int i = 0; i < it->count; ++i, ++l, ++g) {
		float bb[3] = {l->x, l->y, l->z};
		if (qq) {
			qf32_rotate_vector((float const *)qq, (float const *)l, bb);
		}
		g->x += bb[0];
		g->y += bb[1];
		g->z += bb[2];
		if (p) {
			g->x += p->x;
			g->y += p->y;
			g->z += p->z;
		}
	}
}

static void RotateQuaternion1(ecs_iter_t *it)
{
	Orientation *orientation = ecs_field(it, Orientation, 0); // self, out
	Rotate3 const *rotate = ecs_field(it, Rotate3, 1);        // self, in
	for (int i = 0; i < it->count; ++i, ++rotate, ++orientation) {
		float *q = (float *)orientation;
		// assert(fabsf(V4_DOT(q, q) - 1.0f) < 0.1f);         // Check quaternion validity
		float dq_pitch[4];                                 // Quaternion delta pitch rotation
		float dq_yaw[4];                                   // Quaternion delta yaw rotation
		float dq_roll[4];                                  // Quaternion delta roll rotation
		qf32_normalize(q, q, 0.000001f);                   // Normalize quaternion against floating point error
		qf32_xyza(dq_pitch, 1.0f, 0.0f, 0.0f, rotate->dx); // Make delta pitch quaternion
		qf32_xyza(dq_yaw, 0.0f, 1.0f, 0.0f, rotate->dy);   // Make delta yaw quaternion
		qf32_xyza(dq_roll, 0.0f, 0.0f, 1.0f, rotate->dz);  // Make delta roll quaternion
		qf32_mul(q, q, dq_roll);                           // Apply roll delta rotation
		qf32_mul(q, q, dq_yaw);                            // Apply yaw delta rotation
		qf32_mul(q, q, dq_pitch);                          // Apply pitch delta rotation
	}
}

static void RotateQuaternion2(ecs_iter_t *it)
{
	Orientation *orientation = ecs_field(it, Orientation, 0); // self, out
	Rotate3 const *rotate = ecs_field(it, Rotate3, 1);        // self, in
	for (int i = 0; i < it->count; ++i, ++rotate, ++orientation) {
		float *q = (float *)orientation;
		// assert(fabsf(V4_DOT(q, q) - 1.0f) < 0.1f);         // Check quaternion validity
		float dq_pitch[4];                                 // Quaternion delta pitch rotation
		float dq_yaw[4];                                   // Quaternion delta yaw rotation
		float dq_roll[4];                                  // Quaternion delta roll rotation
		qf32_normalize(q, q, 0.000001f);                   // Normalize quaternion against floating point error
		qf32_xyza(dq_pitch, 1.0f, 0.0f, 0.0f, rotate->dx); // Make delta pitch quaternion
		qf32_xyza(dq_yaw, 0.0f, 1.0f, 0.0f, rotate->dy);   // Make delta yaw quaternion
		qf32_xyza(dq_roll, 0.0f, 0.0f, 1.0f, rotate->dz);  // Make delta roll quaternion
		qf32_mul(q, dq_roll, q);                           // Apply roll delta rotation
		qf32_mul(q, dq_yaw, q);                            // Apply yaw delta rotation
		qf32_mul(q, dq_pitch, q);                          // Apply pitch delta rotation
	}
}

static void TransformationPosition(ecs_iter_t *it)
{
	Transformation *t = ecs_field(it, Transformation, 0);                     // self, out
	Position3World const *pos = ecs_field(it, Position3World, 1);             // self, in
	OrientationWorld const *orientation = ecs_field(it, OrientationWorld, 2); // self, in
	Scale3 const *scale = ecs_field(it, Scale3, 3);                           // self, in
	for (int i = 0; i < it->count; ++i, ++t, ++pos, ++orientation, ++scale) {
		// t->matrix = (m4f32)M4_IDENTITY;
		// qf32_unit_to_m4((float *)orientation, &t->matrix);
		// m4f32_translation3(&t->matrix, (float const *)pos);
		// float s[3] = {1,1,1};
		m4f32_trs((float const *)pos, (float *)orientation, (float *)scale, &t->matrix);
	}
}

static void Move(ecs_iter_t *it)
{
	Position3 *p = ecs_field(it, Position3, 0);           // self, out
	Velocity3 const *v = ecs_field(it, Velocity3, 1);     // self, in
	Orientation const *o = ecs_field(it, Orientation, 2); // self, in

	for (int i = 0; i < it->count; ++i, ++p, ++o) {
		// Convert unit quaternion to rotation matrix (r)
		m4f32 r = M4_IDENTITY;
		qf32_unit_to_m4((float *)o, &r);

		// Translate postion (pos) relative to direction of camera rotation:
		float dir[3];
		dir[0] = V3_DOT((float *)v, r.c0);
		dir[1] = V3_DOT((float *)v, r.c1);
		dir[2] = V3_DOT((float *)v, r.c2);

		/*
		TODO:
		Rotation matrix transposed is its inverse.
		Figure out if we need a common move system for both camera and 3d objects.
		*/

		// Move 3D-objects releative to its own orientation:
		// dir[0] = V3_DOTE((float *)v, M3_R0(r));
		// dir[1] = V3_DOTE((float *)v, M3_R1(r));
		// dir[2] = V3_DOTE((float *)v, M3_R2(r));

		// v3f32_print((float*)dir);
		v3f32_add((float *)p, (float *)p, dir);
	}
}

static void SinewaveSystem(ecs_iter_t *it)
{
	Position3World *p = ecs_field(it, Position3World, 0); // self, out
	Sinewave const *w = ecs_field(it, Sinewave, 1);       // self, in
	for (int i = 0; i < it->count; ++i, ++w, ++p) {
		ecs_time_t time;
		ecs_os_get_time(&time);
		double t = (float)time.sec + ((double)time.nanosec / (1000.0 * 1000.0 * 1000.0));
		double a = w->frequency * t * 2.0 * 3.14;
		p->x += sin(a) * w->amplitude;
		p->y += cos(a) * w->amplitude;
	}
}

static void EulerToQ(ecs_iter_t *it)
{
	Orientation *o = ecs_field(it, Orientation, 0);       // self, out
	EulerAngles const *e = ecs_field(it, EulerAngles, 1); // self, in
	for (int i = 0; i < it->count; ++i, ++e, ++o) {
		qf32_from_euler((float *)o, e->pitch, e->yaw, e->roll);
	}
}

static void TransformationCollector_Append(ecs_iter_t *it)
{
	Transformation *t = ecs_field(it, Transformation, 0);                   // self
	TransformationCollector *d = ecs_field(it, TransformationCollector, 1); // up, shared

	int32_t sum = d->count + it->count;
	if (sum > d->cap) {
		d->cap = sum * 2;
		ecs_trace("AppendTransforms::ecs_os_realloc_n");
		d->data = ecs_os_realloc_n(d->data, m4f32, d->cap);
	}

	for (int i = 0; i < it->count; ++i, ++t) {
		d->data[d->count] = t->matrix;
		d->count++;
	}
}

static void TransformationCollector_Reset(ecs_iter_t *it)
{
	TransformationCollector *d = ecs_field(it, TransformationCollector, 0);
	for (int i = 0; i < it->count; ++i, ++d) {
		d->total = d->count;
		d->count = 0;
	}
}

ECS_CTOR(TransformationCollector, ptr, {
	ecs_trace("TransformationCollector::Ctor");
	ecs_os_memset_t(ptr, 0, TransformationCollector);
})

ECS_DTOR(TransformationCollector, ptr, {
	ecs_trace("TransformationCollector::Dtor");
	ecs_os_free(ptr->data);
})

ECS_MOVE(TransformationCollector, dst, src, {
	ecs_trace("TransformationCollector::Move");
	ecs_os_free(dst->data);
	dst->data = src->data;
	src->data = NULL;
})

ECS_COPY(TransformationCollector, dst, src, {
	ecs_trace("TransformationCollector::Copy");
	ecs_os_free(dst->data);
	dst->data = ecs_os_memdup_n(src->data, m4f32, src->count);
})

ECS_CTOR(Transformation, ptr, {
	ptr->matrix = (m4f32)M4_IDENTITY;
})

ECS_CTOR(RotMat3, ptr, {
	ptr->x1 = 1.0f;
	ptr->y1 = 0.0f;
	ptr->z1 = 0.0f;
	ptr->x2 = 0.0f;
	ptr->y2 = 1.0f;
	ptr->z2 = 0.0f;
	ptr->x3 = 0.0f;
	ptr->y3 = 0.0f;
	ptr->z3 = 1.0f;
})

ECS_CTOR(Orientation, ptr, {
	// QF32_IDENTITY;
	// printf("Orientation::ECS_CTOR\n");
	ptr->x = 0.0f;
	ptr->y = 0.0f;
	ptr->z = 0.0f;
	ptr->w = 1.0f;
})

ECS_CTOR(OrientationWorld, ptr, {
	// QF32_IDENTITY;
	// printf("Orientation::ECS_CTOR\n");
	ptr->x = 0.0f;
	ptr->y = 0.0f;
	ptr->z = 0.0f;
	ptr->w = 1.0f;
})

ECS_CTOR(Scale3, ptr, {
	ptr->x = 1.0f;
	ptr->y = 1.0f;
	ptr->z = 1.0f;
})

void EgSpatialsImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgSpatials);
	ECS_IMPORT(world, EgBase);
	ecs_set_name_prefix(world, "EgSpatials");

	ECS_COMPONENT_DEFINE(world, Position2);
	ECS_COMPONENT_DEFINE(world, Position3);
	ECS_COMPONENT_DEFINE(world, Vector4);
	ECS_COMPONENT_DEFINE(world, Ray3);
	ECS_COMPONENT_DEFINE(world, Scale3);
	ECS_COMPONENT_DEFINE(world, Position3World);
	ECS_COMPONENT_DEFINE(world, Velocity2);
	ECS_COMPONENT_DEFINE(world, Velocity3);
	ECS_COMPONENT_DEFINE(world, Orientation);
	ECS_COMPONENT_DEFINE(world, OrientationWorld);
	ECS_COMPONENT_DEFINE(world, EulerAngles);
	ECS_COMPONENT_DEFINE(world, Rotate3);
	ECS_COMPONENT_DEFINE(world, Transformation);
	ECS_COMPONENT_DEFINE(world, TransformationCollector);
	ECS_COMPONENT_DEFINE(world, RotMat3);
	ECS_COMPONENT_DEFINE(world, Sinewave);
	ECS_COMPONENT_DEFINE(world, Color3);

	ECS_TAG_DEFINE(world, EgRotateOrder1);
	ECS_TAG_DEFINE(world, EgRotateOrder2);
	ECS_TAG_DEFINE(world, EgPositionWorldNoReset);

	ecs_set_hooks(world, Orientation, {.ctor = ecs_ctor(Orientation)});
	ecs_set_hooks(world, OrientationWorld, {.ctor = ecs_ctor(OrientationWorld)});
	ecs_set_hooks(world, Transformation, {.ctor = ecs_ctor(Transformation)});
	ecs_set_hooks(world, RotMat3, {.ctor = ecs_ctor(RotMat3)});
	ecs_set_hooks(world, Scale3, {.ctor = ecs_ctor(Scale3)});
	ecs_set_hooks(world, TransformationCollector,
	{
	.ctor = ecs_ctor(TransformationCollector),
	.move = ecs_move(TransformationCollector),
	.copy = ecs_copy(TransformationCollector),
	.dtor = ecs_dtor(TransformationCollector),
	});

	ecs_struct(world,
	{.entity = ecs_id(Position2),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(Position3),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	{.name = "z", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(Color3),
	.members = {
	{.name = "r", .type = ecs_id(ecs_f32_t)},
	{.name = "g", .type = ecs_id(ecs_f32_t)},
	{.name = "b", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(Ray3),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	{.name = "z", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(Scale3),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	{.name = "z", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(Position3World),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	{.name = "z", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(Velocity2),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(Velocity3),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	{.name = "z", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(Orientation),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	{.name = "z", .type = ecs_id(ecs_f32_t)},
	{.name = "w", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(OrientationWorld),
	.members = {
	{.name = "x", .type = ecs_id(ecs_f32_t)},
	{.name = "y", .type = ecs_id(ecs_f32_t)},
	{.name = "z", .type = ecs_id(ecs_f32_t)},
	{.name = "w", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(EulerAngles),
	.members = {
	{.name = "pitch", .type = ecs_id(ecs_f32_t)},
	{.name = "yaw", .type = ecs_id(ecs_f32_t)},
	{.name = "roll", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(Rotate3),
	.members = {
	{.name = "dx", .type = ecs_id(ecs_f32_t)},
	{.name = "dy", .type = ecs_id(ecs_f32_t)},
	{.name = "dz", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_struct(world, {.entity = ecs_id(Vector4),
	                  .members = {
	                  {.name = "x", .type = ecs_id(ecs_f32_t)},
	                  {.name = "y", .type = ecs_id(ecs_f32_t)},
	                  {.name = "z", .type = ecs_id(ecs_f32_t)},
	                  {.name = "w", .type = ecs_id(ecs_f32_t)},
	                  }});

	ecs_struct(world,
	{.entity = ecs_id(Transformation),
	.members = {
	//{.name = "matrix", .type = ecs_id(ecs_f32_t), .count = 16},
	{.name = "c0", .type = ecs_id(Vector4)},
	{.name = "c1", .type = ecs_id(Vector4)},
	{.name = "c2", .type = ecs_id(Vector4)},
	{.name = "c3", .type = ecs_id(Vector4)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(TransformationCollector),
	.members = {
	{.name = "data", .type = ecs_id(ecs_uptr_t)},
	{.name = "count", .type = ecs_id(ecs_i32_t)},
	{.name = "cap", .type = ecs_id(ecs_i32_t)},
	{.name = "total", .type = ecs_id(ecs_i32_t)},
	}});

	ecs_struct(world,
	{.entity = ecs_id(RotMat3),
	.members = {
	{.name = "x1", .type = ecs_id(ecs_f32_t)},
	{.name = "y1", .type = ecs_id(ecs_f32_t)},
	{.name = "z1", .type = ecs_id(ecs_f32_t)},
	{.name = "x2", .type = ecs_id(ecs_f32_t)},
	{.name = "y2", .type = ecs_id(ecs_f32_t)},
	{.name = "z2", .type = ecs_id(ecs_f32_t)},
	{.name = "x3", .type = ecs_id(ecs_f32_t)},
	{.name = "y3", .type = ecs_id(ecs_f32_t)},
	{.name = "z3", .type = ecs_id(ecs_f32_t)}}});

	ecs_struct(world,
	{.entity = ecs_id(Sinewave),
	.members = {
	{.name = "frequency", .type = ecs_id(ecs_f32_t)},
	{.name = "amplitude", .type = ecs_id(ecs_f32_t)},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "RotateQuaternion1", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = RotateQuaternion1,
	.query.terms = {
	{.id = ecs_id(Orientation), .inout = EcsOut},
	{.id = ecs_id(Rotate3), .inout = EcsIn},
	{.id = EgRotateOrder1},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "RotateQuaternion2", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = RotateQuaternion2,
	.query.terms = {
	{.id = ecs_id(Orientation), .inout = EcsOut},
	{.id = ecs_id(Rotate3), .inout = EcsIn},
	{.id = EgRotateOrder2},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Position3World_Reset", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Position3World_Reset,
	.query.terms = {
	{.id = ecs_id(Position3World), .inout = EcsOut},
	{.id = EgPositionWorldNoReset, .oper = EcsNot},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "EulerToQ", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = EulerToQ,
	.query.terms = {
	{.id = ecs_id(Orientation), .inout = EcsOut},
	{.id = ecs_id(EulerAngles), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Orientation_To_RotMat3", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Orientation_To_RotMat3,
	.query.terms = {
	{.id = ecs_id(RotMat3), .inout = EcsOut},
	{.id = ecs_id(Orientation), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Move", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Move,
	.query.terms = {
	{.id = ecs_id(Position3), .inout = EcsOut},
	{.id = ecs_id(Velocity3), .inout = EcsIn},
	{.id = ecs_id(Orientation), .inout = EcsIn}}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "SinewaveSystem", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = SinewaveSystem,
	.query.terms = {
	{.id = ecs_id(Position3World), .inout = EcsOut},
	{.id = ecs_id(Sinewave), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Orientation_Cascade", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Orientation_Cascade,
	.query.terms = {
	{.id = ecs_id(OrientationWorld), .inout = EcsOut},
	{.id = ecs_id(Orientation), .inout = EcsIn},
	{.id = ecs_id(OrientationWorld), .src.id = EcsCascade, .inout = EcsIn, .oper = EcsOptional},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "Position3_Cascade", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = Position3_Cascade,
	.query.terms = {
	{.id = ecs_id(Position3World), .inout = EcsOut},
	{.id = ecs_id(Position3), .inout = EcsIn},
	{.id = ecs_id(Position3World), .src.id = EcsCascade, .inout = EcsIn, .oper = EcsOptional},
	{.id = ecs_id(OrientationWorld), .src.id = EcsUp, .inout = EcsIn, .oper = EcsOptional},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "TransformationPosition", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = TransformationPosition,
	.query.terms = {
	{.id = ecs_id(Transformation), .inout = EcsOut},
	{.id = ecs_id(Position3World), .inout = EcsIn},
	{.id = ecs_id(OrientationWorld), .inout = EcsIn},
	{.id = ecs_id(Scale3), .inout = EcsIn},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "TransformationCollector_Append", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = TransformationCollector_Append,
	.query.terms = {
	{.id = ecs_id(Transformation), .src.id = EcsSelf},
	{.id = ecs_id(TransformationCollector), .trav = EcsDependsOn, .src.id = EcsUp},
	}});

	ecs_system(world,
	{.entity = ecs_entity(world, {.name = "TransformationCollector_Reset", .add = ecs_ids(ecs_dependson(EcsOnUpdate))}),
	.callback = TransformationCollector_Reset,
	.query.terms = {
	{.id = ecs_id(TransformationCollector), .src.id = EcsSelf},
	}});
}