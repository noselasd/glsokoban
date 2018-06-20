#include <math.h>
#include "vector.h"

vec3_t *vec3_new3(vec3_t * tmp, float X, float Y, float Z)
{
	tmp->x = X;
	tmp->y = Y;
	tmp->z = Z;

	return tmp;
}

vec3_t *vec3_add(const vec3_t * first, const vec3_t * second, vec3_t * result)
{
	result->x = first->x + second->x;
	result->y = first->y + second->y;
	result->z = first->z + second->z;

	return result;
}

vec3_t *vec3_sub(const vec3_t * first, const vec3_t * second, vec3_t * result)
{
	result->x = first->x - second->x;
	result->y = first->y - second->y;
	result->z = first->z - second->z;

	return result;
}

vec3_t *vec3_scale(const vec3_t * first, float scale, vec3_t * result)
{
	result->x = first->x * scale;
	result->y = first->y * scale;
	result->z = first->z * scale;

	return result;
}

vec3_t *vec3_mul(const vec3_t * first, const vec3_t * second, vec3_t * result)
{
	result->x = first->x * second->x;
	result->y = first->y * second->y;
	result->z = first->z * second->z;

	return result;
}

//really, this is vec3_scale with scale = 1.0/scale. Nice to have anyway..
vec3_t *vec3_del(const vec3_t * first, float num, vec3_t * result)
{
	result->x = first->x / num;
	result->y = first->y / num;
	result->z = first->z / num;

	return result;
}

vec3_t *vec3_cross(const vec3_t * vector1, const vec3_t * vector2, vec3_t * result)
{
	result->x = ((vector1->y * vector2->z) - (vector1->z * vector2->y));
	result->y = ((vector1->z * vector2->x) - (vector1->x * vector2->z));
	result->z = ((vector1->x * vector2->y) - (vector1->y * vector2->x));

	return result;
}

float vec3_dot(const vec3_t * vector1, const vec3_t * vector2)
{
	return vector1->x * vector2->x + vector1->y * vector2->y + vector1->z * vector2->z;
}

//sqrt(dot)
inline float vec3_length(const vec3_t * vector)
{

	return sqrtf((vector->x * vector->x + vector->y * vector->y + vector->z * vector->z));
}

vec3_t *vec3_rotate(const vec3_t * first, const vec3_t * normal, float angle, vec3_t * result)
{
	vec3_t tmp, tmp1;
	const float cosine = cosf(angle);
	const float sinus = sinf(angle);
	vec3_scale(first, cosine, result);
	vec3_mul(normal, first, &tmp);
	vec3_scale(&tmp, 1.0f - cosine, &tmp);
	vec3_mul(&tmp, normal, &tmp);
	vec3_cross(first, normal, &tmp1);
	vec3_scale(&tmp1, sinus, &tmp1);
	vec3_add(result, &tmp, result);
	vec3_add(result, &tmp1, result);

	return result;
}

vec3_t *vec3_normalize(vec3_t * vector)
{
	float length = vec3_length(vector);

	vector->x /= length;
	vector->y /= length;
	vector->z /= length;

	return vector;
}

vec3_t *vec3_normal(const vec3_t * vVector, vec3_t * result)
{
	float lenght = vec3_length(vVector);

	result->x = vVector->x / lenght;
	result->y = vVector->y / lenght;
	result->z = vVector->z / lenght;

	return result;
}

//Made by Nils O. Selåsdal
