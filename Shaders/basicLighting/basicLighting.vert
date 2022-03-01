#version 330 core
layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec3 norm_in;
layout (location = 2) in vec3 color_in;


out vec3 norm;
out vec3 fragPos; //World position of the Fragment
out vec3 color;


uniform mat4 PVM;
uniform mat4 modelMat;
uniform mat3 normalTransformation;

void main()
{
	gl_Position = PVM * vec4(pos_in, 1.0);
	fragPos = vec3(modelMat * vec4(pos_in, 1.0));
	norm = normalTransformation * norm_in;
	color = color_in;
}