#version 330 core

in vec4 vPosition_ViewSpace;
in vec4 vNormale_ViewSpace;
//in vec4 vTextureCoords;
/* Auto: in vec4 gl_Position;*/

out vec3 FragColor;

void main() {
	FragColor = vec3((normalize(vNormale_ViewSpace)).xyz);
}
