#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

//uniform float alpha;
//uniform sampler2D texture1;
//uniform sampler2D texture2;
uniform vec3 lightPos;
void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float cosAngel = max(abs(dot(lightDir,norm)),0.0);
	vec3 diffuse = vec3(0.83,0.95,0.905) * cosAngel;
	FragColor = vec4(diffuse,1.0);//212,242,231
	//FragColor = mix(texture(texture1,TexCoord),texture(texture2,TexCoord),0.2);
	//FragColor = texture(texture2,vec2(TexCoord.x,TexCoord.y),alpha);

}