#pragma once
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Shader.h"
#include <iostream>

enum LightType {
	point,
	directional,
	spot
};

struct LightMaterial {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class Light
{
public:
	Light();
	Light(LightMaterial lightMaterial, std::string type);
	~Light();
	virtual void processUniforms(Shader* program, int n) = 0;
	std::string getType();
	std::string _type;
protected:
	LightMaterial _lightMaterial;
};

class PointLight : public Light {
public:
	PointLight(LightMaterial lightMaterial, glm::vec3 position, float constant, float linear, float quadratic);
	virtual void processUniforms(Shader* program, int n);
private:
	glm::vec3 _position;

	float _constant;
	float _linear;
	float _quadratic;
};

class DirLight : public Light {
public:
	DirLight(LightMaterial lightMaterial, glm::vec3 direction);
	virtual void processUniforms(Shader* program, int n);
private:
	glm::vec3 _direction;
};
