#pragma once
#include "Entity.h"


class Cell : public Entity {
public:
	Cell(Shader* program, GLuint diffuseMap, GLuint specularMap, glm::vec3 position);
	virtual void draw();
	static GLuint vaoCube;
protected:
	GLuint _diffuseMap;
	GLuint _specularMap;
	static float unity;
};