#version 330

in vec3 vNormal_vs;
in vec2 vTexCoords;

out vec3 fColor;

void main()
{
   fColor = vec3(vTexCoords, 0);
}