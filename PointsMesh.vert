#version 330 core
layout(location = 9) in vec3 aPos;

out vec3 FragPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
out mat4[16] WorldtoView;

void main()
{
	FragPos = vec3(model * vec4(aPos,1.0));//¹Û²ì×ø±êÏÂ

	gl_Position = proj * view * model * vec4(aPos, 1.0);
}