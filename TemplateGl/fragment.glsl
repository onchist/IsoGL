#version 330 core

struct Material{
	sampler2D diffuse;
	float shininess;
	sampler2D specular;
};

struct LightProperties{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;
in vec2 TexCoords;

out vec4 color;

uniform Material material;
uniform LightProperties light;

void main()
{
	vec3 ambient =  vec3(texture(material.diffuse, TexCoords)) * light.ambient;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords)) * light.diffuse;

	vec3 viewDir = normalize(-FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * light.specular;

	vec3 result = ambient + diffuse + specular;

	color = vec4(result, 1.0f);
}