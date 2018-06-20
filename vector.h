#ifndef __VECTOR__H_
#define __VECTOR__H_

typedef struct vec3_t vec3_t;

struct vec3_t {
	float x, y, z;
};

vec3_t *vec3_new3(vec3_t * tmp, float X, float Y, float Z);
vec3_t *vec3_add(const vec3_t * first, const vec3_t * second, vec3_t * result);
vec3_t *vec3_sub(const vec3_t * first, const vec3_t * second, vec3_t * result);
vec3_t *vec3_mul(const vec3_t * first, const vec3_t * second, vec3_t * result);
vec3_t *vec3_del(const vec3_t * first, float num, vec3_t * result);
vec3_t *vec3_scale(const vec3_t * first, float scale, vec3_t * result);
vec3_t *vec3_cross(const vec3_t * vector1, const vec3_t * vector2, vec3_t * result);
vec3_t *vec3_rotate(const vec3_t * first, const vec3_t * normal, float angle, vec3_t * result);
float vec3_dot(const vec3_t * vector1, const vec3_t * vector2);
float vec3_length(const vec3_t * vector);
vec3_t *vec3_normalize(vec3_t * vector);
vec3_t *vec3_normal(const vec3_t * vVector, vec3_t * result);

#endif
