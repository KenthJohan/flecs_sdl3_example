#include "ecsx/ecsx_ops.h"
#include <stdio.h>
#include <printf.h>

const char *ecsx_meta_type_op_kind_str(ecs_meta_op_kind_t kind)
{
	switch (kind) {
	case EcsOpEnum: return "Enum";
	case EcsOpBitmask: return "Bitmask";
	case EcsOpPushStruct: return "Struct";
	case EcsOpPushArray: return "Array";
	case EcsOpPushVector: return "Vector";
	case EcsOpOpaqueStruct: return "OpaqueStruct";
	case EcsOpPop: return "Pop";
	case EcsOpPrimitive: return "Primitive";
	case EcsOpBool: return "Bool";
	case EcsOpChar: return "Char";
	case EcsOpByte: return "Byte";
	case EcsOpU8: return "U8";
	case EcsOpU16: return "U16";
	case EcsOpU32: return "U32";
	case EcsOpU64: return "U64";
	case EcsOpI8: return "I8";
	case EcsOpI16: return "I16";
	case EcsOpI32: return "I32";
	case EcsOpI64: return "I64";
	case EcsOpF32: return "F32";
	case EcsOpF64: return "F64";
	case EcsOpUPtr: return "UPtr";
	case EcsOpIPtr: return "IPtr";
	case EcsOpString: return "String";
	case EcsOpEntity: return "Entity";
	case EcsOpId: return "Id";
	case EcsOpScope: return "Scope";
	default: return "<< invalid kind >>";
	}
}

char const *ecsx_meta_type_op_kind_str1(ecs_meta_op_kind_t kind)
{
	switch (kind) {
	case EcsOpPushStruct:
		return "S{";
	case EcsOpPushArray:
		return "A{";
	case EcsOpPushVector:
		return "V{";
	case EcsOpPop:
		return "}";
	case EcsOpScope:
	case EcsOpEnum:
		return "e";
	case EcsOpBitmask:
	case EcsOpPrimitive:
	case EcsOpBool:
	case EcsOpChar:
	case EcsOpByte:
	case EcsOpU8:
	case EcsOpU16:
	case EcsOpU32:
	case EcsOpU64:
		return "U";
	case EcsOpI8:
	case EcsOpI16:
	case EcsOpI32:
	case EcsOpI64:
		return "I";
	case EcsOpF32:
	case EcsOpF64:
		return "F";
	case EcsOpUPtr:
		return "U";
	case EcsOpIPtr:
		return "I";
	case EcsOpEntity:
	case EcsOpId:
		return "E";
	case EcsOpString:
		return "S";
	default:
		return "?";
	}
}

int ecsx_ops_print(ecs_world_t *world, ecs_entity_t type)
{
	const EcsComponent *comp = ecs_get(world, type, EcsComponent);
	if (!comp) {
		char *path = ecs_get_path(world, type);
		ecs_err("cannot serialize to JSON, '%s' is not a component", path);
		ecs_os_free(path);
		return -1;
	}
	const EcsTypeSerializer *ser = ecs_get(world, type, EcsTypeSerializer);
	if (!ser) {
		char *path = ecs_get_path(world, type);
		ecs_err("cannot serialize to JSON, '%s' has no reflection data", path);
		ecs_os_free(path);
		return -1;
	}
	ecs_meta_op_t *ops = ecs_vec_first_t(&ser->ops, ecs_meta_op_t);
	int32_t count = ecs_vec_count(&ser->ops);
	for (int i = 0; i < count; ++i) {
		ecs_meta_op_t *op = ops + i;
		printf("%i%s", op->op_count, ecsx_meta_type_op_kind_str1(op->kind));
	}
	printf("\n");
	return 0;
}