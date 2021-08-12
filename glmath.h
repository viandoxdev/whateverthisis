#ifndef __GLMATH_H
#define __GLMATH_H
#define GLMATH_FLOATING_POINT_TYPE float
// set this to f if above is float, set it to nothing
// if it is double, and l if it is long double
#define GLMATH_FLOATING_POINT_MATHH_SUFFIX f
#define glmf_t GLMATH_FLOATING_POINT_TYPE
// math
// the double concat is needed, see https://stackoverflow.com/a/1489985/10289343
#define _CONCAT(x, y) x##y
#define CO(x, y) _CONCAT(x, y)
#define S GLMATH_FLOATING_POINT_MATHH_SUFFIX
// math functions adapted to GLMATH_FLOATING_POINT_TYPE
#define sqrtg CO(sqrt, S)
#define atan2g CO(atan2, S)
#define cosg CO(cos, S)
#define sing CO(sin, S)

#undef C
#undef CONCAT
#undef _CONCAT
//
struct Vec2 {
	glmf_t x;
	glmf_t y;
};
struct Vec3 {
	glmf_t x;
	glmf_t y;
	glmf_t z;
};
struct Vec4 {
	glmf_t x;
	glmf_t y;
	glmf_t z;
	glmf_t w;
};
typedef struct Vec2 Vec2;
typedef struct Vec3 Vec3;
typedef struct Vec4 Vec4;
struct Mat2 {
	glmf_t a0; glmf_t a1;
	glmf_t b0; glmf_t b1;
};
struct Mat3 {
	glmf_t a0; glmf_t a1; glmf_t a2;
	glmf_t b0; glmf_t b1; glmf_t b2;
	glmf_t c0; glmf_t c1; glmf_t c2;
};
//TODO: implement Mat4
//WARN: this is not implemented yet
struct Mat4 {
	glmf_t a0; glmf_t a1; glmf_t a2; glmf_t a3;
	glmf_t b0; glmf_t b1; glmf_t b2; glmf_t b3;
	glmf_t c0; glmf_t c1; glmf_t c2; glmf_t c3;
	glmf_t d0; glmf_t d1; glmf_t d2; glmf_t d3;
};
typedef struct Mat2 Mat2;
typedef struct Mat3 Mat3;
typedef struct Mat4 Mat4;

Vec2 vec2new(glmf_t x, glmf_t y);
void vec2zero_to(Vec2* dest);
void vec2clone_to(Vec2 v, Vec2* dest);
void vec2add_to(Vec2 a, Vec2 b, Vec2* dest);
void vec2sub_to(Vec2 a, Vec2 b, Vec2* dest);
void vec2mul_to(Vec2 a, Vec2 b, Vec2* dest);
void vec2div_to(Vec2 a, Vec2 b, Vec2 *dest);
void vec2scale_to(Vec2 a, glmf_t b, Vec2 *dest);
void vec2normalize_to(Vec2 v, Vec2* dest);
void vec2polar_to(glmf_t angle, glmf_t lenght, Vec2* dest);
void vec2perpendicular_cw_to(Vec2 v, Vec2* dest);
void vec2perpendicular_ccw_to(Vec2 v, Vec2* dest);
glmf_t vec2dot(Vec2 a, Vec2 b);
glmf_t vec2squared_length(Vec2 v);
glmf_t vec2angle(Vec2 v);
glmf_t vec2length(Vec2 v);
Vec2 vec2zero();
Vec2 vec2clone(Vec2 v);
Vec2 vec2normalize(Vec2 v);
Vec2 vec2perpendicular_cw(Vec2 v);
Vec2 vec2perpendicular_ccw(Vec2 v);
Vec2 vec2add(Vec2 a, Vec2 b);
Vec2 vec2sub(Vec2 a, Vec2 b);
Vec2 vec2mul(Vec2 a, Vec2 b);
Vec2 vec2div(Vec2 a, Vec2 b);
Vec2 vec2scale(Vec2 a, glmf_t b);
Vec2 vec2polar(glmf_t a, glmf_t b);

Vec3 vec3new(glmf_t x, glmf_t y, glmf_t z);
void vec3from_vec2_to(Vec2 v, Vec3 *dest);
void vec3clone_to(Vec3 v, Vec3* dest);
void vec3zero_to(Vec3* dest);
void vec3add_to(Vec3 a, Vec3 b, Vec3* dest);
void vec3sub_to(Vec3 a, Vec3 b, Vec3* dest);
void vec3mul_to(Vec3 a, Vec3 b, Vec3* dest);
void vec3div_to(Vec3 a, Vec3 b, Vec3* dest);
void vec3scale_to(Vec3 a, glmf_t b, Vec3* dest);
void vec3normalize_to(Vec3 a, Vec3* dest);
void vec3cross_to(Vec3 a, Vec3 b, Vec3* dest);
glmf_t vec3dot(Vec3 a, Vec3 b);
glmf_t vec3squared_length(Vec3 v);
glmf_t vec3length(Vec3 v);
Vec3 vec3zero();
Vec3 vec3from_vec2(Vec2 v);
Vec3 vec3clone(Vec3 v);
Vec3 vec3normalize(Vec3 v);
Vec3 vec3add(Vec3 a, Vec3 b);
Vec3 vec3sub(Vec3 a, Vec3 b);
Vec3 vec3mul(Vec3 a, Vec3 b);
Vec3 vec3div(Vec3 a, Vec3 b);
Vec3 vec3cross(Vec3 a, Vec3 b);
Vec3 vec3scale(Vec3 a, glmf_t b);

Vec4 vec4new(glmf_t x, glmf_t y, glmf_t z, glmf_t w);
void vec4zero_to(Vec4* dest);
void vec4clone_to(Vec4 v, Vec4* dest);
void vec4from_vec2_to(Vec2 v, Vec4* dest);
void vec4from_vec3_to(Vec3 v, Vec4* dest);
void vec4fill_to(glmf_t v, Vec4* dest);
void vec4add_to(Vec4 a, Vec4 b, Vec4* dest);
void vec4sub_to(Vec4 a, Vec4 b, Vec4* dest);
void vec4mul_to(Vec4 a, Vec4 b, Vec4* dest);
void vec4div_to(Vec4 a, Vec4 b, Vec4* dest);
void vec4scale_to(Vec4 a, glmf_t b, Vec4* dest);
void vec4normalize_to(Vec4 a, Vec4* dest);
glmf_t vec4dot(Vec4 a, Vec4 b);
glmf_t vec4squared_length(Vec4 v);
glmf_t vec4length(Vec4 v);
Vec4 vec4zero();
Vec4 vec4from_vec2(Vec2 v);
Vec4 vec4from_vec3(Vec3 v);
Vec4 vec4fill(glmf_t v);
Vec4 vec4clone(Vec4 v);
Vec4 vec4normalize(Vec4 v);
Vec4 vec4add(Vec4 a, Vec4 b);
Vec4 vec4sub(Vec4 a, Vec4 b);
Vec4 vec4mul(Vec4 a, Vec4 b);
Vec4 vec4div(Vec4 a, Vec4 b);
Vec4 vec4scale(Vec4 a, glmf_t b);

Mat2 mat2new();
void mat2clone_to(Mat2 v, Mat2* dest);
void mat2identity_to(Mat2* dest);
void mat2zero_to(Mat2* dest);
void mat2mul_to(Mat2 a, Mat2 b, Mat2* dest);
void mat2make_scale_to(Vec2 v, Mat2* dest);
void mat2make_rotate_to(glmf_t v, Mat2* dest);
void mat2scale_to(Mat2 a, Vec2 b, Mat2* dest);
void mat2rotate_to(Mat2 a, glmf_t b, Mat2* dest);
void mat2apply_vec2_to(Mat2 m, Vec2 v, Vec2* dest);
void mat2apply_scalar_to(Mat2 m, glmf_t s, float* dest);
Vec2 mat2apply_vec2(Mat2 m, Vec2 v);
glmf_t mat2apply_scalar(Mat2 m, float s);
Mat2 mat2identity();
Mat2 mat2zero();
Mat2 mat2clone(Mat2 v);
Mat2 mat2mul(Mat2 a, Mat2 b);
Mat2 mat2make_scale(Vec2 v);
Mat2 mat2make_rotate(glmf_t v);
Mat2 mat2scale(Mat2 a, Vec2 b);
Mat2 mat2rotate(Mat2 a, glmf_t b);

Mat3 mat3new();
void mat3clone_to(Mat3 v, Mat3* dest);
void mat3identity_to(Mat3* dest);
void mat3zero_to(Mat3* dest);
void mat3mul_to(Mat3 a, Mat3 b, Mat3* dest);
void mat3make_translate2d_to(Vec2 v, Mat3* dest);
void mat3make_rotate2d_to(glmf_t a, Mat3* dest);
void mat3make_scale2d_to(Vec2 v, Mat3* dest);
void mat3make_scale_to(Vec3 v, Mat3* dest);
void mat3translate2d_to(Mat3 a, Vec2 v, Mat3* dest);
void mat3rotate2d_to(Mat3 a, glmf_t v, Mat3* dest);
void mat3scale2d_to(Mat3 a, Vec2 v, Mat3* dest);
void mat3scale_to(Mat3 a, Vec3 v, Mat3* dest);
void mat3apply_vec3_to(Mat3 m, Vec3 v, Vec3* dest);
void mat3apply_vec2_to(Mat3 m, Vec2 v, Vec2* dest);
Vec3 mat3apply_vec3(Mat3 m, Vec3 v);
Vec2 mat3apply_vec2(Mat3 m, Vec2 v);
Mat3 mat3identity();
Mat3 mat3zero();
Mat3 mat3clone(Mat3 v);
Mat3 mat3make_scale(Vec3 v);
Mat3 mat3make_rotate2d(glmf_t v);
Mat3 mat3make_scale2d(Vec2 v);
Mat3 mat3make_translate2d(Vec2 v);
Mat3 mat3mul(Mat3 a, Mat3 b);
Mat3 mat3scale(Mat3 a, Vec3 b);
Mat3 mat3scale2d(Mat3 a, Vec2 b);
Mat3 mat3translate2d(Mat3 a, Vec2 b);
Mat3 mat3rotate2d(Mat3 a, glmf_t b);

#endif
