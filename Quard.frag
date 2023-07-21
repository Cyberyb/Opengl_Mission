#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap;
uniform int floors;

uniform mat4 model;
uniform mat4 view[16];
uniform mat4 proj[16];
uniform int count;

uniform int row;
uniform int col;
uniform int hei;

float near_plane = 0.1;
float far_plane = 3.0;

bool InSight(vec4 pos)
{
    if(pos.x > -1 && pos.x < 1 && pos.y > -1 && pos.y < 1)
        return true;
    else
        return false;
}

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{             
    //float depthValue = texture(depthMap, vec2(gl_FragCoord.x/1600,gl_FragCoord.y/1200)).r;
    //FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // orthographic


    float wx = mod(gl_FragCoord.x,col) * 0.11 + (-2.75);
    float wz = floor(gl_FragCoord.x/row) * 0.11 + (-2.75);
    float wy = gl_FragCoord.y * 0.11 + (-2.75);
    vec3 worldpos = vec3(wx,wy,wz);//根据贴图位置计算每个点的世界坐标
    vec4 pos = proj[count] * view[count] * model * vec4(worldpos,1.0);
    pos.xyz /= pos.w;

    float u = (pos.x) * 0.5 + 0.5;
    float v = (pos.y) * 0.5 + 0.5;
    float depthValue = texture(depthMap, vec2(u,v)).r;
    float pointdepth = (pos.z)*0.5+0.5;

    if(pointdepth < depthValue && depthValue < 0.99 && pos.x > -1 && pos.x < 1 && pos.y > -1 && pos.y < 1)
        FragColor = vec4(1.0,0.0,0.0,1.0);
    else
        FragColor = vec4(0.0,0.0,0.0,0.0);

    //if(pos.x > -1 && pos.x < 1 && pos.y > -1 && pos.y < 1)
        //FragColor = vec4(0.0,0.0,0.0,0.0);
    //else
        //FragColor = vec4(1.0,0.0,0.0,1.0);

}