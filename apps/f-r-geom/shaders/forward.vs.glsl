#version 330 core

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

/* On ne peut pas inclure la lib glimac --> ShapeVertex pas reconnu*/
layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
//layout(location = 2) in vec2 aVertexTexCoords;

out vec4 vPosition_ViewSpace;
out vec4 vNormale_ViewSpace;
//out vec2 vTextureCoords;
/* gl_Position est un out par defaut*/

void main() {
	vPosition_ViewSpace = uMVMatrix*vec4(aVertexPosition, 1);
	vNormale_ViewSpace = uNormalMatrix*vec4(aVertexNormal, 0);
	//vTextureCoords = aVertexTexCoords;
	gl_Position = uMVPMatrix*vec4(aVertexPosition, 1);
}
