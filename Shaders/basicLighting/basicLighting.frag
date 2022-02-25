#version 330 core

out vec4 FragColor;

in vec3 norm;
in vec3 color;

void main()
{
	FragColor = vec4(color, 1.0); //Constant white for now
}