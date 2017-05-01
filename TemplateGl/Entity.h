#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Model.h"

class Entity
{
public:
	Entity(Shader* program, Model* model);
	Entity(Shader* program, Model* model, glm::vec3 position);
	Entity(Shader* program, Model* model, glm::vec3 position, glm::vec3 scale);
	~Entity();

	void draw(glm::mat4 view, glm::mat4 projection);


	void offsetPos(glm::vec3 offsetPos);
	void offsetScale(glm::vec3 offsetScale);

	void setScale(glm::vec3 scale);
	void setPos(glm::vec3 pos);

	glm::vec3 getPos();
	glm::vec3 getScale();

private:
	Shader* _program;
	Model* _model;
	glm::vec3 _position;
	glm::vec3 _scale;
};

