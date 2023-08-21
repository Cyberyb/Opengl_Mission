#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
uniform int count;
uniform mat4 model;

uniform int free;
uniform mat4 camera_view;
uniform mat4 camera_proj;
void main()
{
	
	Normal = mat3(transpose(inverse(model))) * aNormal;
	FragPos = vec3(model * vec4(aPos,1.0));//�۲�������
	//TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	gl_Position = camera_proj * camera_view * model * vec4(aPos, 1.0);
}