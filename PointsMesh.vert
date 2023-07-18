#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 FragPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	FragPos = aPos;//世界坐标下

	gl_Position = proj * view * model * vec4(aPos, 1.0);
}