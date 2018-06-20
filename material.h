#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include <stdbool.h>
typedef struct _Material Material;

struct _Material {
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emissive[3];
	float shininess;
	int index;
};

void material_apply(const Material * mat);
int material_manager_load(const char *filename);
void material_manager_clear_all(void);
Material *material_manager_get(int idx);
void material_manager_add(Material * mat);

extern const Material m_silver;
extern const Material m_jade;
extern const Material m_ruby;
extern const Material m_chrome;
extern const Material m_normal;
extern const Material m_blue;
extern const Material m_lightblue;

#endif
