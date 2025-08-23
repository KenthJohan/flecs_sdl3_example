#include "ecsx/ecsx_ser.h"

#define flecs_uto(T, value) (T)(value)
#define flecs_ito(T, value) (T)(value)

int ecsx_flecs_expr_ser_primitive(const ecs_world_t *world, ecs_primitive_kind_t kind, const void *base, ecs_strbuf_t *str, bool is_expr)
{
	switch (kind) {
	case EcsBool:
		if (*(const bool *)base) {
			ecs_strbuf_appendlit(str, "true");
		} else {
			ecs_strbuf_appendlit(str, "false");
		}
		break;
	case EcsChar: {
		/*
		char chbuf[3];
		char ch = *(const char*)base;
		if (ch) {
		    flecs_chresc(chbuf, *(const char*)base, '"');
		    if (is_expr) ecs_strbuf_appendch(str, '"');
		    ecs_strbuf_appendstr(str, chbuf);
		    if (is_expr) ecs_strbuf_appendch(str, '"');
		} else {
		    ecs_strbuf_appendch(str, '0');
		}
		break;
		*/
	}
	case EcsByte:
		ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint8_t *)base));
		break;
	case EcsU8:
		ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint8_t *)base));
		break;
	case EcsU16:
		ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint16_t *)base));
		break;
	case EcsU32:
		ecs_strbuf_appendint(str, flecs_uto(int64_t, *(const uint32_t *)base));
		break;
	case EcsU64:
		ecs_strbuf_append(str, "%llu", *(const uint64_t *)base);
		break;
	case EcsI8:
		ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int8_t *)base));
		break;
	case EcsI16:
		ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int16_t *)base));
		break;
	case EcsI32:
		ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const int32_t *)base));
		break;
	case EcsI64:
		ecs_strbuf_appendint(str, *(const int64_t *)base);
		break;
	case EcsF32:
		ecs_strbuf_appendflt(str, (double)*(const float *)base, 0);
		break;
	case EcsF64:
		ecs_strbuf_appendflt(str, *(const double *)base, 0);
		break;
	case EcsIPtr:
		ecs_strbuf_appendint(str, flecs_ito(int64_t, *(const intptr_t *)base));
		break;
	case EcsUPtr:
		ecs_strbuf_append(str, "%u", *(const uintptr_t *)base);
		break;
	case EcsString: {
		const char *value = *ECS_CONST_CAST(const char **, base);
		if (value) {
			if (!is_expr) {
				ecs_strbuf_appendstr(str, value);
			} else {
				ecs_size_t length = flecs_stresc(NULL, 0, '"', value);
				if (length == ecs_os_strlen(value)) {
					ecs_strbuf_appendch(str, '"');
					ecs_strbuf_appendstrn(str, value, length);
					ecs_strbuf_appendch(str, '"');
				} else {
					char *out = ecs_os_malloc(length + 3);
					flecs_stresc(out + 1, length, '"', value);
					out[0] = '"';
					out[length + 1] = '"';
					out[length + 2] = '\0';
					ecs_strbuf_appendstr(str, out);
					ecs_os_free(out);
				}
			}
		} else {
			ecs_strbuf_appendlit(str, "null");
		}
		break;
	}
	case EcsEntity: {
		ecs_entity_t e = *(const ecs_entity_t *)base;
		if (!e) {
			ecs_strbuf_appendlit(str, "#0");
		} else {
			ecs_get_path_w_sep_buf(world, 0, e, ".", NULL, str, false);
		}
		break;
	}
	case EcsId: {
		ecs_id_t id = *(const ecs_id_t *)base;
		if (!id) {
			ecs_strbuf_appendlit(str, "#0");
		} else {
			ecs_id_str_buf(world, id, str);
		}
		break;
	}
	default:
		ecs_err("invalid primitive kind");
		return -1;
	}

	return 0;
}

int ecsx_expr_ser_bitmask(const ecs_world_t *world, ecs_entity_t type, const void *ptr, ecs_strbuf_t *str)
{
	const EcsBitmask *bitmask_type = ecs_get(world, type, EcsBitmask);
	ecs_check(bitmask_type != NULL, ECS_INVALID_PARAMETER, NULL);
	uint32_t value = *(const uint32_t *)ptr;

	ecs_strbuf_list_push(str, "", "|");

	/* Multiple flags can be set at a given time. Iterate through all the flags
	 * and append the ones that are set. */
	ecs_map_iter_t it = ecs_map_iter(&bitmask_type->constants);
	int count = 0;
	while (ecs_map_next(&it)) {
		ecs_bitmask_constant_t *c = ecs_map_ptr(&it);
		ecs_map_key_t key = ecs_map_key(&it);
		if ((value & key) == key) {
			ecs_strbuf_list_appendstr(str, ecs_get_name(world, c->constant));
			count++;
			value -= (uint32_t)key;
		}
	}

	if (value != 0) {
		/* All bits must have been matched by a constant */
		char *path = ecs_get_path(world, type);
		ecs_err(
		"value for bitmask %s contains bits (%u) that cannot be mapped to constant",
		path, value);
		ecs_os_free(path);
		goto error;
	}

	if (!count) {
		ecs_strbuf_list_appendstr(str, "0");
	}

	ecs_strbuf_list_pop(str, "");

	return 0;
error:
	return -1;
}