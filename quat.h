#ifndef __QUAT__H_
#define __QUAT__H_

#include <string.h>
#include "vector.h"

typedef struct quat_t quat_t;

struct quat_t {
	float angle;
	float x, y, z;
};


static inline quat_t *quat_copy(quat_t * from, quat_t * to)
{
	memcpy(to, from, sizeof(quat_t));
	return to;
}

quat_t *quat_set(quat_t * quat, float angle, float x, float y, float z);
quat_t *quat_init(quat_t * quat);
quat_t *quat_set3(quat_t * quat, float x, float y, float z);
quat_t *quat_postmult(quat_t * quat, quat_t * first);
quat_t *quat_multandset(quat_t * quat, const quat_t * first, const quat_t * second);
void quat_getmatrix(quat_t * quat, float matrix[16]);
quat_t *quat_normalize(quat_t * quat);
quat_t *quat_normal(const quat_t * quat, quat_t * result);
void quat_direction(quat_t * quat, vec3_t * dirout);

#endif
