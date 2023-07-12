#version 330 core
out vec4 FragColor;
in vec3 FragPos;
in mat4 WorldtoView[16];

void main()
{
	vec4 color = vec4(WorldtoView[0][0].xzy,1.0);
	FragColor = color;
}