#version 330

in vec3 vNormal_vs;

out vec3 fNormal;

void main()
{
	std::cout << vNormal_vs << std::endl;
   //fNormal = vNormal_vs;
}