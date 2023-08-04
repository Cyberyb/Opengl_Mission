#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap[16];
uniform int floors;

uniform mat4 model;
uniform mat4 view[16];
uniform mat4 proj[16];
uniform int count;
uniform int layer;

uniform int row;
uniform int col;
uniform int hei;

float near_plane = 0.1;
float far_plane = 3.0;

bool InSight(vec4 pos)//判断点是否在视野内
{
    if(pos.x > -1 && pos.x < 1 && pos.y > -1 && pos.y < 1)
        return true;
    else
        return false;
}


void main()
{             
    int flag = 0;//用于计数，每记一个表示没有被当前摄像机所看到，计满16个后表示没有被任何摄像机看到，满足输出条件

    //float wx = mod(gl_FragCoord.x,col) * (5.5/row) + (-2.75);
    //float wz = floor(gl_FragCoord.x/row) * (5.5/col) + (-2.75);
    //float wy = gl_FragCoord.y * 0.11 + (-2.75);
    float wx = gl_FragCoord.x *(5.5/row) + (-2.75);
    float wz = gl_FragCoord.y * (5.5/col) + (-2.75);
    float wy = layer * (5.5/hei) + (-2.75);//根据当前层数计算y值
    vec3 worldpos = vec3(wx,wy,wz);//根据贴图位置计算每个点的世界坐标

    for(int i = 0; i < 16; i++)
    {
        vec4 pos = proj[i] * view[i] * model * vec4(worldpos,1.0);//获取每个摄像机视野内点的裁剪视图
        pos.xyz /= pos.w;//做透视除法

        //计算UV用于采样深度图
        float u = (pos.x) * 0.5 + 0.5;
        float v = (pos.y) * 0.5 + 0.5;
        float depthValue = texture(depthMap[i], vec2(u,v)).r;

        //计算当前点的深度
        float pointdepth = (pos.z)*0.5+0.5;

        

        //
        if(pointdepth > depthValue && InSight(pos))
        {
            FragColor = vec4(0.0,0.0,0.0,0.0);
            break;
        }

        //判断是否在视野内
        if(!InSight(pos))
            flag+=1;
        else if(InSight(pos) && depthValue == 1.0)
            flag+=1;
    }


    


    if(flag == 16)
        FragColor = vec4(1.0,1.0,1.0,1.0);
    else
        FragColor = vec4(0.0,0.0,0.0,0.0);

        //球外完全剔除，按照半径
    if(length(worldpos)>2.75)
        FragColor = vec4(0.0,0.0,0.0,0.0);

    //if(pos.x > -1 && pos.x < 1 && pos.y > -1 && pos.y < 1)
        //FragColor = vec4(0.0,0.0,0.0,0.0);
    //else
        //FragColor = vec4(1.0,0.0,0.0,1.0);

}