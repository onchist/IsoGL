#version 330 core

in vec2 texCoords;
out vec4 color;

uniform vec3 textColor;
uniform sampler2D textTexture;

void main(){
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(textTexture, texCoords).r);
	color = vec4(textColor, 1.0) * sampled;
}