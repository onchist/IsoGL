#version 330 core


in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;


out vec4 color;

uniform vec3 objectColor;
uniform vec3 lightColor;


void main()
{
	float ambientStrenght = 0.1f;
	vec3 ambient = ambientStrenght * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float specularStrength = 0.5f;	
	vec3 viewDir = normalize(-FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;  

	color = vec4(objectColor * (ambient + diffuse + specular), 1.0f);
}