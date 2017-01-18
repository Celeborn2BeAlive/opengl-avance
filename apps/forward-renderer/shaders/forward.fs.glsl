#version 330

in vec3 vNormal_vs;
in vec3 vPosition_vs;
in vec2 vTexCoords;

// lumière directionnelle
uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;
// lumière ponctuelle
uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;
// couleur diffuse
uniform vec3 uKd;
// texture
uniform sampler2D uKdSampler;

out vec3 fColor;

void main()
{
   	//float distToPointLight = length(uPointLightPosition - vPosition_vs);
	//vec3 dirToPointLight = (uPointLightPosition - vPosition_vs) / distToPointLight;
	//fColor = texture2D(uKdSampler, vTexCoords).bgr * uKd * (uDirectionalLightIntensity * max(0.0, dot(vNormal_vs, uDirectionalLightDir)) + uPointLightIntensity * max(0.0, dot(vNormal_vs, dirToPointLight)) / (distToPointLight * distToPointLight));
	fColor = vNormal_vs;

}