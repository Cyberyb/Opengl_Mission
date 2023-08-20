#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D depthMap[32];
uniform int floors;

uniform mat4 model;
uniform mat4 view[32];
uniform mat4 proj[32];
uniform int count;
uniform int layer;
uniform float radius;
uniform float sidelength;
uniform int sizeOfDep;

uniform int row;
uniform int col;
uniform int hei;

float near_plane = 0.1;
float far_plane = 3.0;

bool InSight(vec4 pos)//�жϵ��Ƿ�����Ұ��
{
    if(pos.x > -1 && pos.x < 1 && pos.y > -1 && pos.y < 1)
        return true;
    else
        return false;
}


void main()
{             
    int flag = 0;//���ڼ�����ÿ��һ����ʾû�б���ǰ�����������������16�����ʾû�б��κ�����������������������

    //float wx = mod(gl_FragCoord.x,col) * (5.5/row) + (-2.75);
    //float wz = floor(gl_FragCoord.x/row) * (5.5/col) + (-2.75);
    //float wy = gl_FragCoord.y * 0.11 + (-2.75);
    float wx = gl_FragCoord.x *(sidelength/row) + (-sidelength/2.0);
    float wz = gl_FragCoord.y * (sidelength/col) + (-sidelength/2.0);
    float wy = layer * (sidelength/hei) + (-sidelength/2.0);//���ݵ�ǰ��������yֵ
    vec3 worldpos = vec3(wx,wy,wz);//������ͼλ�ü���ÿ�������������

    for(int i = 0; i < sizeOfDep; i++)
    {
        vec4 pos = proj[i] * view[i] * model * vec4(worldpos,1.0);//��ȡÿ���������Ұ�ڵ�Ĳü���ͼ
        pos.xyz /= pos.w;//��͸�ӳ���

        //����UV���ڲ������ͼ
        float u = (pos.x) * 0.5 + 0.5;
        float v = (pos.y) * 0.5 + 0.5;
        float depthValue = texture(depthMap[i], vec2(u,v)).r;

        //���㵱ǰ������
        float pointdepth = (pos.z)*0.5+0.5;

        

        //
        if(pointdepth > depthValue && InSight(pos))
        {
            FragColor = vec4(0.0,0.0,0.0,0.0);
            break;
        }

        //�ж��Ƿ�����Ұ��
        if(!InSight(pos))
            flag+=1;
        else if(InSight(pos) && depthValue == 1.0)
            flag+=1;
    }


    


    if(flag == sizeOfDep)
        FragColor = vec4(1.0,1.0,1.0,1.0);
    else
        FragColor = vec4(0.0,0.0,0.0,0.0);

        //������ȫ�޳������հ뾶
    if(length(worldpos)>radius)
        FragColor = vec4(0.0,0.0,0.0,0.0);

    //if(pos.x > -1 && pos.x < 1 && pos.y > -1 && pos.y < 1)
        //FragColor = vec4(0.0,0.0,0.0,0.0);
    //else
        //FragColor = vec4(1.0,0.0,0.0,1.0);

}