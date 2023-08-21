#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

//uniform float alpha;
uniform vec3 lightPos;

float near_plane = 0.1;
float far_plane = 10.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float cosAngel = max(abs(dot(lightDir,norm)),0.0);
	vec3 diffuse = vec3(0.83,0.95,0.905) * cosAngel;
	FragColor = vec4(diffuse,0.3);//212,242,231

     //float Depth = LinearizeDepth(gl_FragCoord.z);
     //FragColor = vec4(vec3(Depth),1.0);

}