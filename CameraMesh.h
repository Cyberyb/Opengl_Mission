#pragma once
#include<glm/glm.hpp>
#include<vector>
#include<string>
#include <fstream>
#include <sstream>
#include <iostream>

struct cameraVertex
{
    glm::vec3 camerapos;
    glm::vec3 camerarot;
    glm::vec4 camerafru;
};

class CameraMesh
{
public:
    //cameraVertex vertices;
    //std::vector<glm::vec3> vertices;
    //std::vector<glm::uvec3> index;
    vector<cameraVertex> cameraVer;
    
    CameraMesh(const std::string& filename);
    void readCameraMesh(const std::string& filename, vector<cameraVertex>& cameraVer);
    vector<glm::vec3> GetCameraPos();
    vector<glm::vec3> GetCameraRot();
    vector<glm::vec4> GetCameraFru();
private:
    std::string file;
};

CameraMesh::CameraMesh(const std::string& filename)
{
    this->file = filename;
    readCameraMesh(filename,this->cameraVer);
}

void CameraMesh::readCameraMesh(const std::string& filename, vector<cameraVertex>& cameraVer)
{
    if (!cameraVer.empty())
    {
        cameraVer.clear();
    }
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        float px, py, pz, rx, ry, rz, fleft, fright, fup, fdown;
        if (!(iss >> px >> py >> pz >> rx >> ry >> rz >> fleft >> fright >> fup >> fdown))
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
