#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

class Entity
{
public:
	Entity(Shader* programID, GLuint diffuseMap, GLuint specularMap, GLuint vaoID, glm::vec3 position);
	~Entity();

	void draw(glm::mat4 view, glm::mat4 projection);
	void move(glm::vec3 offset);
	void rotate(glm::vec3 axis, GLfloat angle);
	glm::vec3 getPos();
private:
	Shader* _programID;
	GLuint _diffuseMap;
	GLuint _specularMap;
	glm::vec3 _position;
	GLuint _vaoID;
};

