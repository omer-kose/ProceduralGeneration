#version 330 core

out vec4 FragColor;

in vec3 fragPos;
in vec3 norm;
in vec3 color;


uniform vec3 lightColor;
uniform vec3 lightDir;

void main()
{
	//Ambient Lighting
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;
	//Diffuse
	vec3 lDir = normalize(-lightDir);
	float diffCoefficient = max(dot(norm, lDir), 0.0f);
	vec3 diffuse = diffCoefficient * lightColor;

	vec3 result = (ambient + diffuse) * color;
	FragColor = vec4(result, 1.0); //Constant white for now
}