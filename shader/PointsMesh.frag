#version 330 core
out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;
uniform vec4 main_color;
uniform bool light;

uniform float upCut;
uniform float downCut;
uniform float leftCut;
uniform float rightCut;
uniform float frontCut;
uniform float backCut;
uniform float radiusCut;
uniform vec3 lightPos;
uniform float diffStrength;
uniform float ambientStrength;



//vec3 lightPos = vec3(5.0, 5.0, 5.0);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
    float distence = length(lightPos - FragPos);
    float atten = distence;



	float cosAngel = max(dot(lightDir,norm),0.0);
	//float cosAngel = dot(lightDir,norm);
	//vec3 diffuse = vec3(0.83,0.95,0.905) * cosAngel;
	if(light == true)
		FragColor = (main_color * cosAngel * 10 * diffStrength)/atten + main_color * ambientStrength;
	else
		FragColor = vec4(main_color);	

    if(length(FragPos) > radiusCut)
        discard;
	if(FragPos.x > rightCut)
        discard;
    if(FragPos.x < leftCut)
        discard;
    if(FragPos.y > upCut)
        discard;
    if(FragPos.y < downCut)
        discard;
    if(FragPos.z > backCut)
        discard;
    if(FragPos.z < frontCut)
        discard;
}