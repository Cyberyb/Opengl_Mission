#pragma once
#include<glm/glm.hpp>
#include<vector>
#include<string>
#include <fstream>
#include <sstream>
#include <iostream>


class PointsMesh
{
public:
    //Vertex vertices;
    std::vector<glm::vec3> points;
    PointsMesh(const std::string& filename);
    std::vector<glm::vec3> readMeshPos(const std::string& filename);
private:
};

PointsMesh::PointsMesh(const std::string& filename)
{
    std::vector<glm::vec3> vertices = readMeshPos(filename);
    this->points = vertices;
}

std::vector<glm::vec3> PointsMesh::readMeshPos(const std::string& filename)
{
    std::vector<glm::vec3> outPos;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        float x, y, z;
        if (!(iss >> x >> y >> z))
        {
            std::cout << "Reading End" << std::endl;
            break;
        }
        outPos.emplace_back(x, y, z);
    }
    return outPos;
}

