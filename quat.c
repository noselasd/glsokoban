#include <string.h>
#include <math.h>
#include "quat.h"

quat_t *quat_init(quat_t * quat)
{
	quat->angle = 1.0f;
	quat->x = 0.0f;
	quat->y = 0.0f;
	quat->z = 0.0f;
	return quat;
}

quat_t *quat_set(quat_t * quat, float angle, float x, float y, float z)
{

	float factor, scale, half;
	// Convert to radians.
	angle *= (M_PI / 180.0f);

	// Normalize the axis.
	factor = x * x + y * y + z * z;

	scale = 1.0f / sqrtf(factor);
	x = x * scale;
	y = y * scale;
	z = z * scale;

	// Build a quaternion!
	quat->angle = cosf(angle * 0.5f);

	half = sinf(angle * 0.5f);
	quat->x = x * half;
	quat->y = y * half;
	quat->z = z * half;

	return quat;


}
inline quat_t *quat_postmult(quat_t * quat, quat_t * first)
{
	quat_t tmp;
	tmp = *quat;

	quat_multandset(quat, &tmp, first);

	return quat;
}

quat_t *quat_set3(quat_t * quat, float x, float y, float z)
{
	quat_t q1, q2, q3;

	quat_set(&q1, x, 1.0f, 0.0f, 0.0f);
	quat_set(&q2, y, 0.0f, 1.0f, 0.0f);
	quat_set(&q3, z, 0.0f, 0.0f, 1.0f);
	*quat = q1;
	quat_postmult(quat, &q2);
	quat_postmult(quat, &q3);

	return quat;

}


quat_t *quat_multandset(quat_t * quat, const quat_t * first, const quat_t * second)
{
	quat->angle = second->angle * first->angle - second->x * first->x - second->y * first->y - second->z * first->z;

	quat->x = second->angle * first->x + second->x * first->angle + second->y * first->z - second->z * first->y;

	quat->y = second->angle * first->y - second->x * first->z + second->y * first->angle + second->z * first->x;

	quat->z = second->angle * first->z + second->x * first->y - second->y * first->x + second->z * first->angle;

	return quat;

}

void quat_getmatrix(quat_t * quat, float matrix[16])
{
	float w, x, y, z;
	float xx, yy, zz;

	quat_normalize(quat);
	w = quat->angle;
	x = quat->x;
	y = quat->y;
	z = quat->z;

	xx = x * x;
	yy = y * y;
	zz = z * z;

	matrix[0 + 0 * 4] = 1.0f - 2.0f * (yy + zz);
	matrix[1 + 0 * 4] = 2.0f * (x * y + w * z);
	matrix[2 + 0 * 4] = 2.0f * (x * z - w * y);
	matrix[3 + 0 * 4] = 0.0f;

	matrix[0 + 1 * 4] = 2.0f * (x * y - w * z);
	matrix[1 + 1 * 4] = 1.0f - 2.0f * (xx + zz);
	matrix[2 + 1 * 4] = 2.0f * (y * z + w * x);
	matrix[3 + 1 * 4] = 0.0f;

	matrix[0 + 2 * 4] = 2.0f * (x * z + w * y);
	matrix[1 + 2 * 4] = 2.0f * (y * z - w * x);
	matrix[2 + 2 * 4] = 1.0f - 2.0f * (xx + yy);
	matrix[3 + 2 * 4] = 0.0f;

	matrix[0 + 3 * 4] = 0.0f;
	matrix[1 + 3 * 4] = 0.0f;
	matrix[2 + 3 * 4] = 0.0f;
	matrix[3 + 3 * 4] = 1.0f;


}

quat_t *quat_normalize(quat_t * quat)
{
	float factor = quat->angle * quat->angle + quat->x * quat->x + quat->y * quat->y + quat->z * quat->z;

	if (factor == 0.0f) {
		return quat;
	}
	float scale = 1.0f / sqrtf(factor);

	quat->angle = quat->angle * scale;
	quat->x = quat->x * scale;
	quat->y = quat->y * scale;
	quat->z = quat->z * scale;

	return quat;

}

quat_t *quat_normal(const quat_t * quat, quat_t * result)
{
	float factor = quat->angle * quat->angle + quat->x * quat->x + quat->y * quat->y + quat->z * quat->z;

	float scale = 1.0f / sqrtf(factor);

	result->angle = quat->angle * scale;
	result->x = quat->x * scale;
	result->y = quat->y * scale;
	result->z = quat->z * scale;

	return result;
}

void quat_direction(quat_t * quat, vec3_t * dirout)
{
	float x, y, z, w;
	quat_normalize(quat);
	x = quat->x;
	y = quat->y;
	z = quat->z;
	w = quat->angle;

	dirout->x = 2.0f * (x * z - w * y);
	dirout->y = 2.0f * (y * z + w * x);
	dirout->z = 1.0f - 2.0f * (x * x + y * y);
}

//Made by Nils O. Selåsdal
