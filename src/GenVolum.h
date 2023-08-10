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
void setFormat_v(int width, int height, int layer)
{
	WID_V = width;
	HEI_V = height;
	LAY_V = layer;
    lenwid = 5.5 / WID_V;
    lenhei = 5.5 / HEI_V;
    lenlay = 5.5 / LAY_V;
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
            float x = (i % WID_V) * (5.5 / WID_V) + (-2.75);
            float y = layer * (5.5 / LAY_V) + (-2.75);
            float z = (i / HEI_V) * (5.5 / HEI_V) + (-2.75);


            //生成8个点
            vp.push_back(glm::vec3(x, y, z));
            vp.push_back(glm::vec3(x+lenwid, y, z));
            vp.push_back(glm::vec3(x, y, z+lenhei));
            vp.push_back(glm::vec3(x + lenwid, y, z + lenhei));
            vp.push_back(glm::vec3(x, y+lenlay, z));
            vp.push_back(glm::vec3(x + lenwid, y+lenlay, z));
            vp.push_back(glm::vec3(x, y+lenlay, z + lenhei));
            vp.push_back(glm::vec3(x+lenwid, y+lenlay, z + lenhei));
        }
    }
    return vp;
}


vector<glm::ivec3> GenVolumeIndex(vector<unsigned char>& pixels,unsigned int& count)
{
    vector<glm::ivec3> vp;
    //int count = 0;
    unsigned int index = count * 8;
    for (int i = 0; i < WID_V * HEI_V; i++) {
        if (pixels[i] == 255)//该点为备选点
        {
            

            //生成6个面   逆时针生成以适应面剔除
            //底面
            vp.push_back(glm::vec3(index,     index + 1, index + 2));
            vp.push_back(glm::vec3(index + 1, index + 3, index + 2));
            //顶面
            vp.push_back(glm::vec3(index + 4, index + 6, index + 5));
            vp.push_back(glm::vec3(index + 5, index + 6, index + 7));
            //前面
            vp.push_back(glm::vec3(index,     index + 4, index + 1));
            vp.push_back(glm::vec3(index + 1, index + 4, index + 5));
            //后面
            vp.push_back(glm::vec3(index + 2, index + 3, index + 6));
            vp.push_back(glm::vec3(index + 3, index + 7, index + 6));
            //左面
            vp.push_back(glm::vec3(index,     index + 2, index + 4));
            vp.push_back(glm::vec3(index + 2, index + 6, index + 4));
            //右面
            vp.push_back(glm::vec3(index + 1, index + 5, index + 3));
            vp.push_back(glm::vec3(index + 3, index + 5, index + 7));


            index += 8;
            count++;
        }
    }
    return vp;
}


