#include "material.h"
#include <GL/gl.h>
#include <stdio.h>

const Material m_silver = { {0.19225, 0.19225, 0.19225, 1.0}, {0.50754, 0.50754, 0.50754, 1.0},
{0.508273, 0.508273, 0.508273, 1.0}, {0.0, 0.0, 0.0}, 51.2, 0
};
const Material m_jade = { {0.135, 0.2225, 0.1575, 0.95}, {0.54, 0.89, 0.63, 0.95},
{0.316228, 0.316228, 0.316228, 0.95}, {0.0, 0.0, 0.0}, 12.8, 1
};
const Material m_ruby = { {0.1745, 0.01175, 0.01175, 0.55}, {0.61424, 0.04136, 0.04136, 0.55},
{0.727811, 0.626959, 0.626959, 0.55}, {0.0, 0.0, 0.0}, 76.8, 2
};
const Material m_chrome = { {0.25, 0.25, 0.25, 1.0}, {0.4, 0.4, 0.4, 1.0},
{0.774597, 0.774597, 0.774597, 1.0}, {0.0, 0.0, 0.0}, 76.8, 3
};

const Material m_lightblue = { {0.1745, 0.01175, 0.02175, 0.55}, {0.5136, 0.7136, 0.99424, 0.55},
{0.627811, 0.726959, 0.726959, 0.55}, {0.0, 0.0, 0.04}, 76.8, 4
};
const Material m_blue = { {0.1745, 0.01175, 0.01175, 0.55}, {0.04136, 0.04136, 0.61424, 0.55},
{0.727811, 0.626959, 0.626959, 0.55}, {0.0, 0.0, 0.04}, 76.8, 5
};
const Material m_normal = { {0.5, 0.5, 0.5, 1.0}, {0.90, 0.90, 0.90, 1.0},
{0.01, 0.01, 0.01, 1.0}, {0.0, 0.0, 0.2}, 1.0, 6
};

static Material **materials;
static int num_materials = 0;
static int current_mat = -1;
//this material manager is a big todo :-)
void material_apply(const Material * mat)
{
	if (current_mat == mat->index)
		return;

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat->ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat->emissive);
	glMaterialfv(GL_FRONT, GL_SHININESS, &mat->shininess);
	current_mat = mat->index;
}

Material *material_manager_get(int idx)
{
	if (idx >= num_materials) {
		return NULL;
	}
	return materials[idx];
}

//Made by Nils O. Selåsdal
