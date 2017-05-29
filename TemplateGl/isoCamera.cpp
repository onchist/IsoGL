#include "isoCamera.h"



isoCamera::isoCamera(glm::vec3 target): _target(target), _angle(0)
{
}


isoCamera::~isoCamera()
{
}

void isoCamera::setTarget(glm::vec3 newTarget) {
	_target = newTarget;
}

void isoCamera::move(glm::vec3 offset)
{
	_target += offset;
}

void isoCamera::rotateTo(float rad)
{
	_angle = rad;
}

void isoCamera::rotate(float radOffset)
{
	_angle += radOffset;
}

glm::mat4 isoCamera::getViewMatrix(float zoom) {
	glm::mat4 tm0, tm1, rm0, rm1, ret;
	/*tm0 = glm::translate(tm0, -_target);
	rm0 = glm::rotate(rm0, glm::radians(-45.0f) + _angle, glm::vec3(0.0f, 0.0f, 1.0f));
	tm1 = glm::translate(tm1, glm::vec3(0.0f, 10.0f * zoom, -10.0f * zoom));
	rm1 = glm::rotate(rm1, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f ));
	ret = rm1 * tm1 * rm0 * tm0;*/
	
	tm0 = glm::translate(tm0, -_target);
	tm1 = glm::translate(tm1, glm::vec3(0.0f, 0.0f, -15.0f));
	glm::vec3 euler = glm::vec3(glm::radians(-45.0f), glm::radians(0.0f), glm::radians(45.0f));

	glm::quat quatX = glm::angleAxis(euler.x, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat quatY = glm::angleAxis(euler.y, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat quatZ = glm::angleAxis(euler.z, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::quat quat = quatX * quatY * quatZ;

	rm0 = glm::toMat4(quat);

	ret = tm1 * rm0  * tm0;
	
	return ret;
}


glm::vec3 isoCamera::getViewPos() {
	return _target;
}