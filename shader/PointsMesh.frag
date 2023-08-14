#version 330 core
out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;
uniform vec4 main_color;

vec3 lightPos = vec3(5.0, 5.0, 5.0);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float cosAngel = max(dot(lightDir,norm),0.0);
	//float cosAngel = dot(lightDir,norm);
	//vec3 diffuse = vec3(0.83,0.95,0.905) * cosAngel;
	FragColor = main_color * cosAngel;
	//FragColor = vec4(FragPos,1.0);
}