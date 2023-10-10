#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <queue>
using namespace std;

static int WID_V = 64;   // Replace with your actual width
static int HEI_V = 64;  // Replace with your actual height
static int LAY_V = 64;

static float lenwid = 5.5 / WID_V;
static float lenhei = 5.5 / HEI_V;
static float lenlay = 5.5 / LAY_V;
static float startpos = -2.75;
void setFormat_v(int width, int height, int layer, float sidelength)
{
	WID_V = width;
	HEI_V = height;
	LAY_V = layer;
    lenwid = sidelength / WID_V;
    lenhei = sidelength / HEI_V;
    lenlay = sidelength / LAY_V;
    startpos = -sidelength/2.0;
}

vector<glm::vec3> GenVolumePoints(vector<unsigned char>& pixels,int layer,unsigned int &count)
{
    vector<glm::vec3> vp;
    //int count = 0;
    for (int i = 0; i < WID_V * HEI_V; i++) {
        if (pixels[i] == 255)//该点为备选点
        {
            count++;
            //float num = i / 4.0;
            //float x = (i % WID_V) * (5.5 / WID_V) + (-2.75);
            //float y = layer * (5.5 / LAY_V) + (-2.75);
            //float z = (i / HEI_V) * (5.5 / HEI_V) + (-2.75);

            float x = (i % WID_V) * lenwid + startpos;
            float y = layer * lenlay + startpos;
            float z = (i / HEI_V) * lenhei + startpos;


            //生成8个点
/*            vp.push_back(glm::vec3(x, y, z));                            
            vp.push_back(glm::vec3(x+lenwid, y, z)),                     
            vp.push_back(glm::vec3(x, y, z+lenhei));                     
            vp.push_back(glm::vec3(x + lenwid, y, z + lenhei));          
            vp.push_back(glm::vec3(x, y+lenlay, z));                     
            vp.push_back(glm::vec3(x + lenwid, y+lenlay, z));            
            vp.push_back(glm::vec3(x, y+lenlay, z + lenhei));            
            vp.push_back(glm::vec3(x+lenwid, y+lenlay, z + lenhei));    */ 

            //生成24个点
            //底面
            vp.push_back(glm::vec3(x, y, z));
            vp.push_back(glm::vec3(x + lenwid, y, z));
            vp.push_back(glm::vec3(x, y, z + lenhei));
            vp.push_back(glm::vec3(x + lenwid, y, z + lenhei));
            //顶面
            vp.push_back(glm::vec3(x, y + lenlay, z));
            vp.push_back(glm::vec3(x + lenwid, y + lenlay, z));
            vp.push_back(glm::vec3(x, y + lenlay, z + lenhei));
            vp.push_back(glm::vec3(x + lenwid, y + lenlay, z + lenhei));
            //前面
            vp.push_back(glm::vec3(x, y, z));
            vp.push_back(glm::vec3(x + lenwid, y, z)),
            vp.push_back(glm::vec3(x, y + lenlay, z));
            vp.push_back(glm::vec3(x + lenwid, y + lenlay, z));
            //后面
            vp.push_back(glm::vec3(x, y, z + lenhei));
            vp.push_back(glm::vec3(x + lenwid, y, z + lenhei));
            vp.push_back(glm::vec3(x, y + lenlay, z + lenhei));
            vp.push_back(glm::vec3(x + lenwid, y + lenlay, z + lenhei));
            //左面
            vp.push_back(glm::vec3(x, y, z));
            vp.push_back(glm::vec3(x, y, z + lenhei));
            vp.push_back(glm::vec3(x, y + lenlay, z));
            vp.push_back(glm::vec3(x, y + lenlay, z + lenhei));
            //右面
            vp.push_back(glm::vec3(x + lenwid, y, z));
            vp.push_back(glm::vec3(x + lenwid, y, z + lenhei));
            vp.push_back(glm::vec3(x + lenwid, y + lenlay, z));
            vp.push_back(glm::vec3(x + lenwid, y + lenlay, z + lenhei));
        }
    }
    return vp;
}

vector<glm::vec3> GenVolumeNormal(const unsigned int& count)
{
    //glm::vec3 nor1 = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    //glm::vec3 nor2 = glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f));
    //glm::vec3 nor3 = glm::normalize(glm::vec3(-1.0f, -1.0f, 1.0f));
    //glm::vec3 nor4 = glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f));
    //glm::vec3 nor5 = glm::normalize(glm::vec3(-1.0f, 1.0f, -1.0f));
    //glm::vec3 nor6 = glm::normalize(glm::vec3(1.0f, 1.0f, -1.0f));
    //glm::vec3 nor7 = glm::normalize(glm::vec3(-1.0f, 1.0f, 1.0f));
    //glm::vec3 nor8 = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));

    glm::vec3 norbot1 = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
    glm::vec3 norbot2 = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
    glm::vec3 norbot3 = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
    glm::vec3 norbot4 = glm::normalize(glm::vec3(0.0f, -1.0f, 0.0f));
    glm::vec3 norup1 = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 norup2 = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 norup3 = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 norup4 = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 norfro1 = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
    glm::vec3 norfro2 = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
    glm::vec3 norfro3 = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
    glm::vec3 norfro4 = glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f));
    glm::vec3 norback1 = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 norback2 = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 norback3 = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 norback4 = glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 norleft1 = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::vec3 norleft2 = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::vec3 norleft3 = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::vec3 norleft4 = glm::normalize(glm::vec3(-1.0f, 0.0f, 0.0f));
    glm::vec3 norrgt1 = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 norrgt2 = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 norrgt3 = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 norrgt4 = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));


    vector<glm::vec3> vn;
    //int count = 0;
    for (int i = 0; i < count; i++) {
            //float num = i / 4.0;
            //vn.push_back(nor1);
            //vn.push_back(nor2);
            //vn.push_back(nor3);
            //vn.push_back(nor4);
            //vn.push_back(nor5);
            //vn.push_back(nor6);
            //vn.push_back(nor7);
            //vn.push_back(nor8);
            //生成8个点    
            // 
            
        vn.push_back(norbot1);
        vn.push_back(norbot2);
        vn.push_back(norbot3);
        vn.push_back(norbot4);
        vn.push_back(norup1);
        vn.push_back(norup2);
        vn.push_back(norup3);
        vn.push_back(norup4);
        vn.push_back(norfro1);
        vn.push_back(norfro2);
        vn.push_back(norfro3);
        vn.push_back(norfro4);
        vn.push_back(norback1);
        vn.push_back(norback2);
        vn.push_back(norback3);
        vn.push_back(norback4);
        vn.push_back(norleft1);
        vn.push_back(norleft2);
        vn.push_back(norleft3);
        vn.push_back(norleft4);
        vn.push_back(norrgt1);
        vn.push_back(norrgt2);
        vn.push_back(norrgt3);
        vn.push_back(norrgt4);

    }
    return vn;
}


vector<glm::ivec3> GenVolumeIndex(vector<unsigned char>& pixels,unsigned int& count)
{
    vector<glm::ivec3> vp;
    //int count = 0;
    unsigned int index = count * 24;
    for (int i = 0; i < WID_V * HEI_V; i++) {
        if (pixels[i] == 255)//该点为备选点
        {
            

            //生成6个面   逆时针生成以适应面剔除
            ////底面
            //vp.push_back(glm::vec3(index,     index + 1, index + 2));
            //vp.push_back(glm::vec3(index + 1, index + 3, index + 2));
            ////顶面
            //vp.push_back(glm::vec3(index + 4, index + 6, index + 5));
            //vp.push_back(glm::vec3(index + 5, index + 6, index + 7));
            ////前面
            //vp.push_back(glm::vec3(index,     index + 4, index + 1));
            //vp.push_back(glm::vec3(index + 1, index + 4, index + 5));
            ////后面
            //vp.push_back(glm::vec3(index + 2, index + 3, index + 6));
            //vp.push_back(glm::vec3(index + 3, index + 7, index + 6));
            ////左面
            //vp.push_back(glm::vec3(index,     index + 2, index + 4));
            //vp.push_back(glm::vec3(index + 2, index + 6, index + 4));
            ////右面
            //vp.push_back(glm::vec3(index + 1, index + 5, index + 3));
            //vp.push_back(glm::vec3(index + 3, index + 5, index + 7));

            //底面
            vp.push_back(glm::vec3(index, index + 1, index + 2));
            vp.push_back(glm::vec3(index + 1, index + 3, index + 2));
            //顶面
            vp.push_back(glm::vec3(index + 4, index + 6, index + 5));
            vp.push_back(glm::vec3(index + 5, index + 6, index + 7));
            //前面
            vp.push_back(glm::vec3(index + 8, index + 10, index + 9));
            vp.push_back(glm::vec3(index + 9, index + 10, index + 11));
            //后面
            vp.push_back(glm::vec3(index + 12, index + 13, index + 14));
            vp.push_back(glm::vec3(index + 13, index + 15, index + 14));
            //左面
            vp.push_back(glm::vec3(index + 16, index + 17, index + 18));
            vp.push_back(glm::vec3(index + 17, index + 19, index + 18));
            //右面
            vp.push_back(glm::vec3(index + 20, index + 22, index + 21));
            vp.push_back(glm::vec3(index + 21, index + 22, index + 23));


            index += 24;
            count++;
        }
    }
    return vp;
}


