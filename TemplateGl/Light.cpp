#include "Light.h"



Light::Light()
{
}


Light::Light(LightMaterial lightMaterial, std::string type) : _lightMaterial(lightMaterial), _type(type)
{
}

Light::~Light()
{
}

std::string Light::getType()
{
	return _type;
}

PointLight::PointLight(LightMaterial lightMaterial, glm::vec3 position, float constant, float linear, float quadratic) : Light(lightMaterial, "pointLights"), 
_position(position), _constant(constant), _linear(linear), _quadratic(quadratic)
{
}

void PointLight::processUniforms(Shader* program, int n)
{
	std::string base = _type + "[" + std::to_string(n) + "].";
	
	std::string posString = base + "position";

	std::string ambientString = base + "ambient";
	std::string diffuseString = base + "diffuse";
	std::string specularString = base + "specular";

	std::string constantString = base + "constant";
	std::string linearString = base + "linear";
	std::string quadraticString = base + "quadratic";

	GLuint lightPositionLoc = glGetUniformLocation(program->getID(), posString.c_str());

	GLuint lightAmbientLoc = glGetUniformLocation(program->getID(), ambientString.c_str());
	GLuint lightDiffuseLoc = glGetUniformLocation(program->getID(), diffuseString.c_str());
	GLuint lightSpecularLoc = glGetUniformLocation(program->getID(), specularString.c_str());
	
	GLuint lightConstantLoc = glGetUniformLocation(program->getID(), constantString.c_str());
	GLuint lightLinearLoc = glGetUniformLocation(program->getID(), linearString.c_str());
	GLuint lightQuadraticLoc = glGetUniformLocation(program->getID(), quadraticString.c_str());

	glUniform3f(lightPositionLoc, _position.x, _position.y, _position.z);

	glUniform3f(lightAmbientLoc, _lightMaterial.ambient.x, _lightMaterial.ambient.y, _lightMaterial.ambient.z);
	glUniform3f(lightDiffuseLoc, _lightMaterial.diffuse.x, _lightMaterial.diffuse.y, _lightMaterial.diffuse.z);
	glUniform3f(lightSpecularLoc, _lightMaterial.specular.x, _lightMaterial.specular.y, _lightMaterial.specular.z);

	glUniform1f(lightConstantLoc, _constant);
	glUniform1f(lightLinearLoc, _linear);
	glUniform1f(lightQuadraticLoc, _quadratic);
}
