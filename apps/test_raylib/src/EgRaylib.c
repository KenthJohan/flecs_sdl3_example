#include "EgRaylib.h"
#include <EgBase.h>
#include <EgCameras.h>
#include <EgSpatials.h>
#include <EgShapes.h>
#include <EgKeyboards.h>
#include <EgCamcontrols.h>
#include <EgSpatials.h>
#include <EgWindows.h>

#include <raylib.h>
#include <raymath.h>

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"
#include "raymath.h"

#define GLSL_VERSION 330

typedef struct {
	Mesh mesh;
} EgRaylibMesh;

ECS_COMPONENT_DECLARE(EgRaylibMesh);

ECS_COMPONENT_DECLARE(EgRaylibMode3D);
ECS_COMPONENT_DECLARE(EgRaylibMode3DCreateInfo);

static Camera camera = {0};

// Load default material (using raylib intenral default shader) for non-instanced mesh drawing
// WARNING: Default shader enables vertex color attribute BUT GenMeshCube() does not generate vertex colors, so,
// when drawing the color attribute is disabled and a default color value is provided as input for thevertex attribute
Material matDefault;

// NOTE: We are assigning the intancing shader to material.shader
// to be used on mesh drawing with DrawMeshInstanced()
Material matInstances;

Shader shader;

Mesh dummy_cube;

static void System_EgRaylibMode3D_CreateMesh_FromBox(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgShapesBox const *box = ecs_field(it, EgShapesBox, 0); // self, in
	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		Mesh mesh = GenMeshCube(box[i].width, box[i].height, box[i].depth);
		ecs_set(world, e, EgRaylibMesh, {.mesh = mesh});
	}
}

static void System_EgRaylibMode3D_Init(ecs_iter_t *it)
{
	ecs_world_t *world = it->world;
	EgRaylibMode3DCreateInfo const *info = ecs_field(it, EgRaylibMode3DCreateInfo, 0); // self, in
	for (int i = 0; i < it->count; ++i) {
		ecs_entity_t e = it->entities[i];
		ecs_query_t *query = ecs_query_init(world,
		&(ecs_query_desc_t){
		.entity = ecs_entity(world, {.name = "flecs.systems.sokol.LightsQuery"}),
		.terms = {
		{.id = ecs_id(EgRaylibMesh), .trav = EcsDependsOn, .src.id = EcsUp},
		{.id = ecs_id(Transformation)},
		},
		.cache_kind = EcsQueryCacheAuto});
		ecs_set(it->world, e, EgRaylibMode3D, {.query = query});
	}
}

static void draw(ecs_world_t *world, ecs_query_t *query)
{
	ecs_iter_t it = ecs_query_iter(world, query);
	while (ecs_query_next(&it)) {
		EgRaylibMesh const *mesh = ecs_field(&it, EgRaylibMesh, 0);           // shared, in
		Transformation const *transforms = ecs_field(&it, Transformation, 1); // self, in
		DrawMeshInstancedColumnMajor(mesh->mesh, matInstances, transforms, it.count);
	}
}

static void System_EgRaylibMode3D_Draw(ecs_iter_t *it)
{
	EgRaylibMode3D *raylib3d = ecs_field(it, EgRaylibMode3D, 0);
	EgCamerasState *cam = ecs_field(it, EgCamerasState, 1);
	Position3 *campos = ecs_field(it, Position3, 2);


	// Update the light shader with the camera view position
	float cameraPos[3] = {campos->x, campos->y, campos->z};
	SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
	BeginDrawing();
	ClearBackground(RAYWHITE);
	for (int i = 0; i < it->count; ++i, ++cam, ++campos, ++raylib3d) {
		ecs_entity_t e = it->entities[i];
		// Print entity name
		// printf("Entity %08jX: %s\n", (uintmax_t)e, ecs_get_name(it->world, e));
		Camera3D cam3d = {0};
		Vector3 forward = { cam->view.c2[0], cam->view.c2[1], cam->view.c2[2] };
		Vector3 up = { cam->view.c1[0], cam->view.c1[1], cam->view.c1[2] };
		cam3d.position = (Vector3){campos->x, campos->y, campos->z};
		cam3d.target = Vector3Subtract(cam3d.position, forward);
		cam3d.up = up;
		cam3d.fovy = cam->fov;
		cam3d.projection = CAMERA_PERSPECTIVE;
		BeginMode3D(cam3d);
		DrawMesh(dummy_cube, matDefault, MatrixTranslate(-10.0f, 0.0f, 0.0f));
		draw(it->world, raylib3d[i].query);
		DrawMesh(dummy_cube, matDefault, MatrixTranslate(10.0f, 0.0f, 0.0f));
		EndMode3D();

		char *tmp = (char *)TextFormat("%f %f %f", campos->x, campos->y, campos->z);
		int width = MeasureText(tmp, 20);
		DrawText(tmp, GetScreenWidth() - 20 - width, 10, 20, DARKGREEN);
	}
	DrawFPS(10, 10);



	EndDrawing();
}

static void System_EgRaylibMode3D_EgKeyboardsState(ecs_iter_t *it)
{
	EgKeyboardsState *keyboard = ecs_field(it, EgKeyboardsState, 0); // singleton
	for (int i = 0; i < EG_KEYBOARDS_KEYS_MAX; ++i) {
		keyboard->state[i] = 0;
		if (IsKeyReleased(i)) {
			keyboard->state[i] |= EG_KEYBOARDS_STATE_RELEASED;
		}
		if (IsKeyPressed(i)) {
			keyboard->state[i] |= EG_KEYBOARDS_STATE_PRESSED;
		}
		if (IsKeyDown(i)) {
			keyboard->state[i] |= EG_KEYBOARDS_STATE_DOWN;
		}
		if (IsKeyUp(i)) {
			keyboard->state[i] |= EG_KEYBOARDS_STATE_UP;
		}
	}
	return;
}

static void System_Window(ecs_iter_t *it)
{
	EgWindowsWindow *window = ecs_field(it, EgWindowsWindow, 0);
	EgShapesRectangle *rectangle = ecs_field(it, EgShapesRectangle, 1);
	for (int i = 0; i < it->count; ++i) {
		rectangle[i].w = GetScreenWidth();  // Get current screen width
		rectangle[i].h = GetScreenHeight(); // Get current screen height
	}
	return;
}

void EgRaylibImport(ecs_world_t *world)
{
	ECS_MODULE(world, EgRaylib);
	ECS_IMPORT(world, EgSpatials);
	ECS_IMPORT(world, EgCameras);
	ECS_IMPORT(world, EgKeyboards);
	ECS_IMPORT(world, EgCamcontrols);
	ECS_IMPORT(world, EgWindows);
	ecs_set_name_prefix(world, "EgRaylib");
	ECS_COMPONENT_DEFINE(world, EgRaylibMode3D);
	ECS_COMPONENT_DEFINE(world, EgRaylibMode3DCreateInfo);
	ECS_COMPONENT_DEFINE(world, EgRaylibMesh);

	ecs_singleton_add(world, EgKeyboardsState);

	ecs_struct_init(world,
	&(ecs_struct_desc_t){
	.entity = ecs_id(EgRaylibMode3DCreateInfo),
	.members = {
	{.name = "dummy", .type = ecs_id(ecs_i32_t)},
	}});

	dummy_cube = GenMeshCube(1.0f, 1.0f, 1.0f);

	camera.position = (Vector3){-125.0f, 125.0f, -125.0f}; // Camera position
	camera.target = (Vector3){0.0f, 0.0f, 0.0f};           // Camera looking at point
	camera.up = (Vector3){0.0f, 1.0f, 0.0f};               // Camera up vector (rotation towards target)
	camera.fovy = 45.0f;                                   // Camera field-of-view Y
	camera.projection = CAMERA_PERSPECTIVE;                // Camera projection type

	IsKeyDown(KEY_W);

	// Load lighting shader
	shader = LoadShader(TextFormat("config/glsl%i/lighting_instancing.vs", GLSL_VERSION),
	TextFormat("config/glsl%i/lighting.fs", GLSL_VERSION));
	// Get shader locations
	shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(shader, "mvp");
	shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

	// Set shader value: ambient light level
	int ambientLoc = GetShaderLocation(shader, "ambient");
	SetShaderValue(shader, ambientLoc, (float[4]){0.2f, 0.2f, 0.2f, 1.0f}, SHADER_UNIFORM_VEC4);

	// Create one light
	CreateLight(LIGHT_DIRECTIONAL, (Vector3){50.0f, 50.0f, 0.0f}, Vector3Zero(), WHITE, shader);

	matInstances = LoadMaterialDefault();
	matInstances.shader = shader;
	matInstances.maps[MATERIAL_MAP_DIFFUSE].color = RED;
	matDefault = LoadMaterialDefault();
	matDefault.maps[MATERIAL_MAP_DIFFUSE].color = BLUE;

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "System_EgRaylibMode3D_EgKeyboardsState", .add = ecs_ids(ecs_dependson(EcsOnLoad))}),
	.callback = System_EgRaylibMode3D_EgKeyboardsState,
	.immediate = true,
	.query.terms = {
	{.id = ecs_id(EgKeyboardsState), .src.id = ecs_id(EgKeyboardsState)},
	}});

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "System_EgRaylibMode3D_CreateMesh_FromBox", .add = ecs_ids(ecs_dependson(EcsOnLoad))}),
	.callback = System_EgRaylibMode3D_CreateMesh_FromBox,
	.immediate = true,
	.query.terms = {
	{.id = ecs_id(EgShapesBox)},
	{.id = ecs_id(EgRaylibMesh), .oper = EcsNot},
	}});

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "System_EgRaylibMode3D_Init", .add = ecs_ids(ecs_dependson(EcsOnLoad))}),
	.callback = System_EgRaylibMode3D_Init,
	.immediate = true,
	.query.terms = {
	{.id = ecs_id(EgRaylibMode3DCreateInfo)},
	{.id = ecs_id(EgRaylibMode3D), .oper = EcsNot},
	}});

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "System_EgRaylibMode3D_Draw", .add = ecs_ids(ecs_dependson(EcsOnStore))}),
	.callback = System_EgRaylibMode3D_Draw,
	.query.terms = {
	{.id = ecs_id(EgRaylibMode3D)},
	{.id = ecs_id(EgCamerasState)},
	{.id = ecs_id(Position3)},
	}});

	ecs_system_init(world,
	&(ecs_system_desc_t){
	.entity = ecs_entity(world, {.name = "System_Window", .add = ecs_ids(ecs_dependson(EcsOnStore))}),
	.callback = System_Window,
	.query.terms = {
	{.id = ecs_id(EgWindowsWindow)},
	{.id = ecs_id(EgShapesRectangle)},
	}});
}
