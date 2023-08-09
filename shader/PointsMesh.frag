#version 330 core
out vec4 FragColor;
in vec3 FragPos;
uniform vec4 main_color;

void main()
{
	//FragColor = main_color;
	FragColor = vec4(FragPos,1.0);
}