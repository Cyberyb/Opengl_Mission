#pragma once
#include<glm/glm.hpp>
#include<vector>
#include<string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Utility.h"

struct cameraVertex
{
    glm::vec3 camerapos;
    glm::vec3 camerarot;
    glm::vec4 camerafru;
};

class CameraMesh
{
public:

    int LightCount = 16;
    vector<cameraVertex> cameraVer;
    unsigned int VAO;
    unsigned int VBO;

    std::vector<glm::vec3> camerapos;
    std::vector<glm::vec3> camerarot;
    std::vector<glm::vec4> camerafru;

    std::vector<unsigned int> depthFBO;
    std::vector<unsigned int> depthMap;

    
    CameraMesh(const std::string& filename);
    ~CameraMesh();
    void readCameraMesh(const std::string& filename, vector<cameraVertex>& cameraVer);
    vector<glm::vec3> GetCameraPos();
    vector<glm::vec3> GetCameraRot();
    vector<glm::vec4> GetCameraFru();
    void Bind();
    vector<glm::mat4> CalWorld2View();
    vector<glm::mat4> CalFrustum();
    void CreateDepthMap(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT);
    void ReBuid(const std::string& filename);
private:
    std::string file;
};

CameraMesh::CameraMesh(const std::string& filename)
{
    this->file = filename;
    readCameraMesh(filename,this->cameraVer);

    this->camerapos = GetCameraPos();
    this->camerarot = GetCameraRot();
    this->camerafru = GetCameraFru();

    this->LightCount = this->cameraVer.size();

    this->depthFBO.assign(32, 0);
    this->depthMap.assign(32, 0);
}

CameraMesh::~CameraMesh()
{
    glDeleteBuffers(1, &(this->VBO));
    glDeleteVertexArrays(1, &(this->VAO));
    for(int i = 0;i<cameraVer.size();i++)
        glDeleteFramebuffers(1, &depthFBO[i]);
}

void CameraMesh::ReBuid(const std::string& filename)
{
    //CameraMesh::~CameraMesh();
    //CameraMesh::CameraMesh(filename);
    this->file = filename;
    readCameraMesh(filename, this->cameraVer);
    this->camerapos = GetCameraPos();
    this->camerarot = GetCameraRot();
    this->camerafru = GetCameraFru();
    this->LightCount = this->cameraVer.size();
}

void CameraMesh::readCameraMesh(const std::string& filename, vector<cameraVertex>& cameraVer)
{
    //std::locale::global(std::locale("en_US.UTF-8"));
    if (!cameraVer.empty())
    {
        cameraVer.clear();
    }

    std::ifstream file(filename);
    std::string line;

    int light_count = 0;
    if (std::getline(file, line))
    {
        std::istringstream iss(line);
        int count = 0;
        float tempValue;
        while (iss >> tempValue) {
            count++;
            if (count > 1)
                break;
        }

        iss.clear();//清除状态位
        iss.seekg(0);//将流指针重置到开头

        if (count == 1)
        {
            iss >> light_count;;
            this->LightCount = light_count;
        }
        else
            file.seekg(0);
    }


    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        float px, py, pz, rx, ry, rz, fleft, fright, fup, fdown,temp1,temp2;
        if (!(iss >> px >> py >> pz >> rx >> ry >> rz >> fleft >> fright >> fup >> fdown >> temp1 >> temp2))
        {
            std::cout << "Reading End" << std::endl;
            break;
        }
        //cameraVer.emplace_back({px, py, pz}, {rx, ry, rz}, {fleft, fright, fup, fdown});
        cameraVer.push_back(cameraVertex{ glm::vec3(px, py, pz),glm::vec3(rx, ry, rz),glm::vec4(fleft, fright, fup, fdown) });
    }
    return;
}

vector<glm::vec3> CameraMesh::GetCameraPos()
{
    if (cameraVer.empty())
    {
        readCameraMesh(this->file, cameraVer);
    }

    vector<glm::vec3> outPos;
    for (auto it = cameraVer.begin(); it != cameraVer.end();it++)
    {
        outPos.push_back(it->camerapos);
    }
    return outPos;
}

vector<glm::vec3> CameraMesh::GetCameraRot()
{
    if (cameraVer.empty())
    {
        readCameraMesh(this->file, cameraVer);
    }

    vector<glm::vec3> outPos;
    for (auto it = cameraVer.begin(); it != cameraVer.end(); it++)
    {
        outPos.push_back(it->camerarot);
    }
    return outPos;
}

vector<glm::vec4> CameraMesh::GetCameraFru()
{
    if (cameraVer.empty())
    {
        readCameraMesh(this->file, cameraVer);
    }

    vector<glm::vec4> outPos;
    for (auto it = cameraVer.begin(); it != cameraVer.end(); it++)
    {
        outPos.push_back(it->camerafru);
    }
    return outPos;
}

void CameraMesh::Bind()
{
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 10 * cameraVer.size(), cameraVer.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);//传Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));//传Rot
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));//传Fru
    glEnableVertexAttribArray(2);

    glBindBuffer(0, this->VBO);
}

vector<glm::mat4> CameraMesh::CalWorld2View()
{
    vector<glm::mat4> worldtoview;
    for (int k = 0; k < this->cameraVer.size(); k++) {
        glm::vec3 r = this->camerarot[k];
        glm::vec3 camera = this->camerapos[k];

        //方法一：使用自定义的viewMatrix函数（有问题，生成的View矩阵第一三行符号相反）
        //glm::mat4 worldToView = viewMatrix(-camera, r);

        //方法二：计算lookAt矩阵（正确的，中肯的）
        glm::vec3 dir, right, up;
        Utility::GetDirRightUp(this->camerarot[k], dir, right, up);
        glm::mat4 worldToView = glm::lookAt(this->camerapos[k], this->camerapos[k] + dir, up);
        worldtoview.push_back(worldToView);
    }
    return worldtoview;
}

vector<glm::mat4> CameraMesh::CalFrustum()
{
    vector<glm::mat4> allfrustum;
    for (int j = 0; j < this->cameraVer.size(); j++) {
        glm::mat4 fru = Utility::GetFrustumbyangle(this->camerafru[j].x, this->camerafru[j].y, this->camerafru[j].w, this->camerafru[j].z, 0.1f, 100.0f);
        allfrustum.push_back(fru);
    }
    return allfrustum;
}

void CameraMesh::CreateDepthMap(const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT)
{
    if (cameraVer.empty())
    {
        std::cerr << "Light Camera not init" << std::endl;
        return;
    }
    int mapsize = cameraVer.size();
    this->depthFBO.clear();
    this->depthMap.clear();
    //this->depthFBO.shrink_to_fit();
    //this->depthMap.shrink_to_fit();
    this->depthFBO.assign(32, 0);
    this->depthMap.assign(32, 0);

    //glDeleteFramebuffers(mapsize, this->depthFBO.data());
    //glDeleteTextures(mapsize, this->depthMap.data());

    glGenFramebuffers(32, this->depthFBO.data());
    glGenTextures(32, this->depthMap.data());
    for (int dmpC = 0; dmpC < 32; dmpC++)
    {
        glBindTexture(GL_TEXTURE_2D, this->depthMap[dmpC]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1280, 720, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindFramebuffer(GL_FRAMEBUFFER, this->depthFBO[dmpC]);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMap[dmpC], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "Framebuffer not complete! " << endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}