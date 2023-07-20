#version 330 core
out vec4 FragColor;
in vec3 FragPos;
uniform mat4 WorldtoView[16];
uniform sampler2D depthMap;

void main()
{

	float depth = texture(depthMap,vec2(gl_FragCoord.x/1600,gl_FragCoord.y/1200)).r;
	//float depth = texture(depthMap,TexCoord).r;


	if(gl_FragCoord.z < depth && depth <0.99)
	{
		FragColor = vec4(FragPos,1.0);
	}
	else
	{
		discard;
	}
	//vec4 color = vec4(WorldtoView[0][0].xzy,1.0);

	//FragColor = vec4(FragPos,1.0);
}