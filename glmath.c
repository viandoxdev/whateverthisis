#include "glmath.h"
#include <math.h>

#define PREFIX vec2
#define TYPE Vec2
#define _CONCAT(x, y) x##y
#define CONCAT(x, y) _CONCAT(x, y)
#define CONCAT3(x, y, z) CONCAT(x, CONCAT(y, z))
// theses macro makes a pure version of an impure function of
// form void (args, type* dest) for, no args (NA), one arg (1A)
// or two args (2A).
#define MAKE_PURE_NA(method) \
	TYPE CONCAT(PREFIX, method)() { \
		TYPE res = {}; \
		CONCAT3(PREFIX, method, _to)(&res); \
		return res; \
	}
#define MAKE_PURE_1A(type1, method) \
	TYPE CONCAT(PREFIX, method)(type1 v) { \
		TYPE res = {}; \
		CONCAT3(PREFIX, method, _to)(v, &res); \
		return res; \
	}
#define MAKE_PURE_2A(type1, type2, method) \
	TYPE CONCAT(PREFIX, method)(type1 a, type2 b) { \
		TYPE res = {}; \
		CONCAT3(PREFIX, method, _to)(a, b, &res); \
		return res; \
	}

Vec2 vec2new(glmf_t x, glmf_t y) {
	Vec2 res = {};
	res.x = x;
	res.y = y;
	return res;
}
void vec2zero_to(Vec2* dest) {
	dest->x = 0;
	dest->y = 0;
}
void vec2fill_to(glmf_t v, Vec2 * dest) {
	dest->x = v;
	dest->y = v;
}
void vec2clone_to(Vec2 v, Vec2* dest) {
	dest->x = v.x;
	dest->y = v.y;
}
void vec2add_to(Vec2 a, Vec2 b, Vec2* dest) {
	dest->x = a.x + b.x;
	dest->y = a.y + b.y;
}
void vec2sub_to(Vec2 a, Vec2 b, Vec2* dest) {
	dest->x = a.x - b.x;
	dest->y = a.y - b.y;
}
void vec2mul_to(Vec2 a, Vec2 b, Vec2* dest) {
	dest->x = a.x * b.x;
	dest->y = a.y * b.y;
}
void vec2div_to(Vec2 a, Vec2 b, Vec2 *dest) {
	dest->x = a.x / b.x;
	dest->y = a.y / b.y;
}
void vec2scale_to(Vec2 a, glmf_t b, Vec2 *dest) {
	dest->x = a.x * b;
	dest->y = a.y * b;
}
void vec2normalize_to(Vec2 v, Vec2* dest) {
	vec2scale_to(v, (glmf_t)1 / vec2length(v), dest);
}
void vec2polar_to(glmf_t angle, glmf_t lenght, Vec2* dest) {
	dest->x = cosg(angle) * lenght;
	dest->y = sing(angle) * lenght;
}
void vec2perpendicular_cw_to(Vec2 v, Vec2* dest) {
	dest->x = -v.y;
	dest->y = v.x;
}
void vec2perpendicular_ccw_to(Vec2 v, Vec2* dest) {
	dest->x = v.y;
	dest->y = -v.x;
}
glmf_t vec2dot(Vec2 a, Vec2 b) {
	return a.x * b.x + a.y * b.y;
}
glmf_t vec2squared_length(Vec2 v) {
	return vec2dot(v, v);
}
glmf_t vec2angle(Vec2 v) {
	return atan2g(v.x, v.y);
}
glmf_t vec2length(Vec2 v) {
	return sqrtg(vec2squared_length(v));
}
MAKE_PURE_NA(zero)
MAKE_PURE_1A(Vec2, clone)
MAKE_PURE_1A(glmf_t, fill)
MAKE_PURE_1A(Vec2, normalize)
MAKE_PURE_1A(Vec2, perpendicular_cw)
MAKE_PURE_1A(Vec2, perpendicular_ccw)
MAKE_PURE_2A(Vec2, Vec2, add)
MAKE_PURE_2A(Vec2, Vec2, sub)
MAKE_PURE_2A(Vec2, Vec2, mul)
MAKE_PURE_2A(Vec2, Vec2, div)
MAKE_PURE_2A(Vec2, glmf_t, scale)
MAKE_PURE_2A(glmf_t, glmf_t, polar)

#undef PREFIX
#undef TYPE
#define PREFIX vec3
#define TYPE Vec3

Vec3 vec3new(glmf_t x, glmf_t y, glmf_t z) {
	Vec3 res = {};
	res.x = x;
	res.y = y;
	res.z = z;
	return res;
}
void vec3from_vec2_to(Vec2 v, Vec3 *dest) {
	dest->x = v.x;
	dest->y = v.y;
	dest->z = 0;
}
void vec3clone_to(Vec3 v, Vec3* dest) {
	dest->x = v.x;
	dest->y = v.y;
	dest->z = v.z;
}
void vec3fill_to(glmf_t v, Vec3* dest) {
	dest->x = v;
	dest->y = v;
	dest->z = v;
}
void vec3zero_to(Vec3* dest) {
	dest->x = 0;
	dest->y = 0;
	dest->z = 0;
}
void vec3add_to(Vec3 a, Vec3 b, Vec3* dest) {
	dest->x = a.x + b.x;
	dest->y = a.y + b.y;
	dest->z = a.z + b.z;
}
void vec3sub_to(Vec3 a, Vec3 b, Vec3* dest) {
	dest->x = a.x - b.x;
	dest->y = a.y - b.y;
	dest->z = a.z - b.z;
}
void vec3mul_to(Vec3 a, Vec3 b, Vec3* dest) {
	dest->x = a.x * b.x;
	dest->y = a.y * b.y;
	dest->z = a.z * b.z;
}
void vec3div_to(Vec3 a, Vec3 b, Vec3* dest) {
	dest->x = a.x / b.x;
	dest->y = a.y / b.y;
	dest->z = a.z / b.z;
}
void vec3scale_to(Vec3 a, glmf_t b, Vec3* dest) {
	dest->x = a.x * b;
	dest->y = a.y * b;
	dest->z = a.z * b;
}
void vec3normalize_to(Vec3 a, Vec3* dest) {
	vec3scale_to(a, (glmf_t) 1 / vec3length(a), dest);
}
void vec3cross_to(Vec3 a, Vec3 b, Vec3* dest) {
	dest->x = a.y * b.z - a.z * b.y;
	dest->y = a.z * b.x - a.x * b.x;
	dest->z = a.x * b.y - a.y * b.z;
}
glmf_t vec3dot(Vec3 a, Vec3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
glmf_t vec3squared_length(Vec3 v) {
	return vec3dot(v, v);
}
glmf_t vec3length(Vec3 v) {
	return sqrtg(vec3squared_length(v));
}

MAKE_PURE_NA(zero)
MAKE_PURE_1A(Vec2, from_vec2)
MAKE_PURE_1A(Vec3, clone)
MAKE_PURE_1A(glmf_t, fill)
MAKE_PURE_1A(Vec3, normalize)
MAKE_PURE_2A(Vec3, Vec3, add)
MAKE_PURE_2A(Vec3, Vec3, sub)
MAKE_PURE_2A(Vec3, Vec3, mul)
MAKE_PURE_2A(Vec3, Vec3, div)
MAKE_PURE_2A(Vec3, Vec3, cross)
MAKE_PURE_2A(Vec3, glmf_t, scale)

#undef PREFIX
#undef TYPE
#define PREFIX vec4
#define TYPE Vec4

Vec4 vec4new(glmf_t x, glmf_t y, glmf_t z, glmf_t w) {
	Vec4 res = {};
	res.x = x;
	res.y = y;
	res.z = z;
	res.w = w;
	return res;
}
void vec4zero_to(Vec4* dest) {
	dest->x = 0;
	dest->y = 0;
	dest->z = 0;
	dest->w = 0;
}
void vec4clone_to(Vec4 v, Vec4* dest) {
	dest->x = v.x;
	dest->y = v.y;
	dest->z = v.z;
	dest->w = v.w;
}
void vec4from_vec2_to(Vec2 v, Vec4* dest) {
	dest->x = v.x;
	dest->y = v.y;
	dest->z = 0;
	dest->w = 0;
}
void vec4from_vec3_to(Vec3 v, Vec4* dest) {
	dest->x = v.x;
	dest->y = v.y;
	dest->z = v.z;
	dest->w = 0;
}
void vec4fill_to(glmf_t v, Vec4* dest) {
	dest->x = v;
	dest->y = v;
	dest->z = v;
	dest->w = v;
}
void vec4add_to(Vec4 a, Vec4 b, Vec4* dest) {
	dest->x = a.x + b.x;
	dest->y = a.y + b.y;
	dest->z = a.z + b.z;
	dest->w = a.w + b.w;
}
void vec4sub_to(Vec4 a, Vec4 b, Vec4* dest) {
	dest->x = a.x - b.x;
	dest->y = a.y - b.y;
	dest->z = a.z - b.z;
	dest->w = a.w - b.w;
}
void vec4mul_to(Vec4 a, Vec4 b, Vec4* dest) {
	dest->x = a.x * b.x;
	dest->y = a.y * b.y;
	dest->z = a.z * b.z;
	dest->w = a.w * b.w;
}
void vec4div_to(Vec4 a, Vec4 b, Vec4* dest) {
	dest->x = a.x / b.x;
	dest->y = a.y / b.y;
	dest->z = a.z / b.z;
	dest->w = a.w / b.w;
}
void vec4scale_to(Vec4 a, glmf_t b, Vec4* dest) {
	dest->x = a.x * b;
	dest->y = a.y * b;
	dest->z = a.z * b;
	dest->w = a.w * b;
}
void vec4normalize_to(Vec4 a, Vec4* dest) {
	vec4scale_to(a, (glmf_t) 1 / vec4length(a), dest);
}
glmf_t vec4dot(Vec4 a, Vec4 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
glmf_t vec4squared_length(Vec4 v) {
	return vec4dot(v, v);
}
glmf_t vec4length(Vec4 v) {
	return sqrtg(vec4squared_length(v));
}

MAKE_PURE_NA(zero)
MAKE_PURE_1A(Vec2, from_vec2)
MAKE_PURE_1A(Vec3, from_vec3)
MAKE_PURE_1A(glmf_t, fill)
MAKE_PURE_1A(Vec4, clone)
MAKE_PURE_1A(Vec4, normalize)
MAKE_PURE_2A(Vec4, Vec4, add)
MAKE_PURE_2A(Vec4, Vec4, sub)
MAKE_PURE_2A(Vec4, Vec4, mul)
MAKE_PURE_2A(Vec4, Vec4, div)
MAKE_PURE_2A(Vec4, glmf_t, scale)

#undef PREFIX
#undef TYPE
#define PREFIX mat2
#define TYPE Mat2

Mat2 mat2new() {
	Mat2 res = {};
	return res;
}
void mat2clone_to(Mat2 v, Mat2* dest) {
	dest->a0 = v.a0; dest->a1 = v.a1;
	dest->b0 = v.b0; dest->b1 = v.b1;
}
void mat2identity_to(Mat2* dest) {
	dest->a0 = 1; dest->a1 = 0;
	dest->b0 = 0; dest->b1 = 1;
}
void mat2zero_to(Mat2* dest) {
	dest->a0 = 0; dest->a0 = 0;
	dest->b0 = 0; dest->b0 = 0;
}
void mat2mul_to(Mat2 a, Mat2 b, Mat2* dest) {
	dest->a0 = a.a0 * b.a0 + a.a1 * b.b0;
	dest->a1 = a.a0 * b.a1 + a.a1 * b.b1;
	dest->b0 = a.b0 * b.a0 + a.b1 * b.b0;
	dest->b1 = a.b0 * b.a1 + a.b1 * b.b1;
}
void mat2make_scale_to(Vec2 v, Mat2* dest) {
	dest->a0 = v.x; dest->a1 = 0;
	dest->b0 = 0; dest->b1 = v.y;
}
void mat2make_rotate_to(glmf_t v, Mat2* dest) {
	dest->a0 = cosg(v); dest->a1 = -sing(v);
	dest->b0 = sing(v); dest->b1 = cosg(v);
}
void mat2scale_to(Mat2 a, Vec2 b, Mat2* dest) {
	Mat2 scale;
	mat2make_scale_to(b, &scale);
	mat2mul_to(scale, a, dest);
}
void mat2rotate_to(Mat2 a, glmf_t b, Mat2* dest) {
	Mat2 rotate;
	mat2make_rotate_to(b, &rotate);
	mat2mul_to(rotate, a, dest);
}
void mat2apply_vec2_to(Mat2 m, Vec2 v, Vec2* dest) {
	dest->x = m.a0 * v.x + m.a1 * v.y;
	dest->y = m.b0 * v.x + m.b1 * v.y;
}
void mat2apply_scalar_to(Mat2 m, glmf_t s, glmf_t* dest) {
	Vec2 res;
	mat2apply_vec2_to(m, vec2new(s, 1), &res);
	*dest = res.x;
}
Vec2 mat2apply_vec2(Mat2 m, Vec2 v) {
	Vec2 res;
	mat2apply_vec2_to(m, v, &res);
	return res;
}
glmf_t mat2apply_scalar(Mat2 m, glmf_t s) {
	glmf_t res;
	mat2apply_scalar_to(m, s, &res);
	return res;
}
MAKE_PURE_NA(identity)
MAKE_PURE_NA(zero)
MAKE_PURE_1A(Mat2, clone)
MAKE_PURE_2A(Mat2, Mat2, mul)
MAKE_PURE_1A(Vec2, make_scale)
MAKE_PURE_1A(glmf_t, make_rotate)
MAKE_PURE_2A(Mat2, Vec2, scale)
MAKE_PURE_2A(Mat2, glmf_t, rotate)

#undef PREFIX
#undef TYPE
#define PREFIX mat3
#define TYPE Mat3

Mat3 mat3new() {
	Mat3 res = {};
	return res;
}
void mat3clone_to(Mat3 v, Mat3* dest) {
	dest->a0 = v.a0; dest->a1 = v.a1; dest->a2 = v.a2;
	dest->b0 = v.b0; dest->b1 = v.b1; dest->b2 = v.b2;
	dest->c0 = v.c0; dest->c1 = v.c1; dest->c2 = v.c2;
}
void mat3identity_to(Mat3* dest) {
	dest->a0 = 1; dest->a1 = 0; dest->a2 = 0;
	dest->b0 = 0; dest->b1 = 1; dest->b2 = 0;
	dest->c0 = 0; dest->c1 = 0; dest->c2 = 1;
}
void mat3zero_to(Mat3* dest) {
	dest->a0 = 0; dest->a1 = 0; dest->a2 = 0;
	dest->b0 = 0; dest->b1 = 0; dest->b2 = 0;
	dest->c0 = 0; dest->c1 = 0; dest->c2 = 0;
}
void mat3mul_to(Mat3 a, Mat3 b, Mat3* dest) {
	dest->a0 = a.a0 * b.a0 + a.a1 * b.b0 + a.a2 * b.c0;
	dest->a1 = a.a0 * b.a1 + a.a1 * b.b1 + a.a2 * b.c1;
	dest->a2 = a.a0 * b.a2 + a.a1 * b.b2 + a.a2 * b.c2;

	dest->b0 = a.b0 * b.a0 + a.b1 * b.b0 + a.b2 * b.c0;
	dest->b1 = a.b0 * b.a1 + a.b1 * b.b1 + a.b2 * b.c1;
	dest->b2 = a.b0 * b.a2 + a.b1 * b.b2 + a.b2 * b.c2;

	dest->c0 = a.c0 * b.a0 + a.c1 * b.b0 + a.c2 * b.c0;
	dest->c1 = a.c0 * b.a1 + a.c1 * b.b1 + a.c2 * b.c1;
	dest->c2 = a.c0 * b.a2 + a.c1 * b.b2 + a.c2 * b.c2;
}
void mat3make_translate2d_to(Vec2 v, Mat3* dest) {
	mat3identity_to(dest);
	dest->c0 = v.x;
	dest->c1 = v.y;
}
void mat3make_rotate2d_to(glmf_t a, Mat3* dest) {
	dest->a0 = cosg(a); dest->a1 = -sing(a); dest->a2 = 0;
	dest->b0 = sing(a); dest->b1 =  cosg(a); dest->b2 = 0;
	dest->c0 =       0; dest->c1 =        0; dest->c2 = 0;
}
void mat3make_scale2d_to(Vec2 v, Mat3* dest) {
	dest->a0 = v.x; dest->a1 =   0; dest->a2 = 0;
	dest->b0 =   0; dest->b1 = v.y; dest->b2 = 0;
	dest->c0 =   0; dest->c1 =   0; dest->c2 = 0;
}
void mat3make_scale_to(Vec3 v, Mat3* dest) {
	dest->a0 = v.x; dest->a1 =   0; dest->a2 =   0;
	dest->b0 =   0; dest->b1 = v.y; dest->b2 =   0;
	dest->c0 =   0; dest->c1 =   0; dest->c2 = v.z;
}
void mat3translate2d_to(Mat3 a, Vec2 v, Mat3* dest) { 
	Mat3 translate;
	mat3make_translate2d_to(v, &translate);
	mat3mul_to(translate, a, dest);
}
void mat3rotate2d_to(Mat3 a, glmf_t v, Mat3* dest) {
	Mat3 rotate;
	mat3make_rotate2d_to(v, &rotate);
	mat3mul_to(rotate, a, dest);
}
void mat3scale2d_to(Mat3 a, Vec2 v, Mat3* dest) {
	Mat3 scale;
	mat3make_scale2d_to(v, &scale);
	mat3mul_to(scale, a, dest);
}
void mat3scale_to(Mat3 a, Vec3 v, Mat3* dest) {
	Mat3 scale;
	mat3make_scale_to(v, &scale);
	mat3mul_to(scale, a, dest);
}
void mat3apply_vec3_to(Mat3 m, Vec3 v, Vec3* dest) {
	dest->x = m.a0 * v.x + m.a1 * v.y + m.a2 * v.z;
	dest->y = m.b0 * v.x + m.b1 * v.y + m.b2 * v.z;
	dest->z = m.c0 * v.x + m.c1 * v.y + m.c2 * v.z;
}
void mat3apply_vec2_to(Mat3 m, Vec2 v, Vec2* dest) {
	Vec3 res;
	mat3apply_vec3_to(m, vec3new(v.x, v.y, 1), &res);
	dest->x = res.x;
	dest->y = res.y;
}
Vec3 mat3apply_vec3(Mat3 m, Vec3 v) {
	Vec3 res;
	mat3apply_vec3_to(m, v, &res);
	return res;
}
Vec2 mat3apply_vec2(Mat3 m, Vec2 v) {
	Vec2 res;
	mat3apply_vec2_to(m, v, &res);
	return res;
}

MAKE_PURE_NA(identity)
MAKE_PURE_NA(zero)
MAKE_PURE_1A(Mat3, clone)
MAKE_PURE_1A(Vec3, make_scale)
MAKE_PURE_1A(glmf_t, make_rotate2d)
MAKE_PURE_1A(Vec2, make_scale2d)
MAKE_PURE_1A(Vec2, make_translate2d)
MAKE_PURE_2A(Mat3, Mat3, mul)
MAKE_PURE_2A(Mat3, Vec3, scale)
MAKE_PURE_2A(Mat3, Vec2, scale2d)
MAKE_PURE_2A(Mat3, Vec2, translate2d)
MAKE_PURE_2A(Mat3, glmf_t, rotate2d)

