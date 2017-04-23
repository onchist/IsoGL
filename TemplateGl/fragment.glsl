#version 330 core

struct Material{
	sampler2D diffuse;
	float shininess;
	sampler2D specular;
};

struct PointLight{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

#define NR_POINT_LIGHTS 1

uniform PointLight pointLights[NR_POINT_LIGHTS];


in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in mat4 viewFrag;

out vec4 color;

uniform Material material;
uniform PointLight light;

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, mat4 view);

void main()
{
	vec3 result = vec3(0.0);

	for(int i = 0; i < NR_POINT_LIGHTS; i++){
		result += calcPointLight(pointLights[i], Normal, FragPos, viewFrag);
	}

	color = vec4(result, 1.0f);
}



vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, mat4 view){
	vec3 lightPos = vec3(view * vec4(light.position,1.0));

	float distance = length(lightPos - fragPos);
	float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient =  vec3(texture(material.diffuse, TexCoords)) * light.ambient;
	ambient *= attenuation;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords)) * light.diffuse;
	diffuse *= attenuation;

	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * light.specular;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;


	return result;
}



