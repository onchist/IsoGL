#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

class Entity
{
public:
	Entity(Shader* program, glm::vec3 position);
	~Entity();

	virtual void draw() = 0;

	void offsetPos(glm::vec3 offsetPos);

	void setPos(glm::vec3 pos);

	glm::vec3 getPos() const;

protected:
	Shader* _program;
	glm::vec3 _position;
};

