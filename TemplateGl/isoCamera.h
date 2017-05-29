#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <SDL/SDL.h>

class isoCamera
{
public:
	isoCamera(glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f));
	~isoCamera();
	glm::mat4 getViewMatrix(float zoom);
	void setTarget(glm::vec3 newTarget);
	void move(glm::vec3 offset);

	void rotateTo(float rad);
	void rotate(float radOffset);
	glm::vec3 getViewPos();
private:
	glm::vec3 _target;
	float _angle;
};

