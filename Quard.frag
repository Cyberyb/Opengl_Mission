#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
float near_plane = 0.1;
float far_plane = 3.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{             
    float depthValue = texture(depthMap, TexCoords).r;
    //float depthValue = texture(depthMap, vec2(gl_FragCoord.x/1600,gl_FragCoord.y/1200)).r;
    FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // orthographic
}