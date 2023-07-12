#pragma once
#include<glm/glm.hpp>
#include<vector>
#include<string>
#include <fstream>
#include <sstream>
#include <iostream>

struct Vertex
{
	glm::vec3 Positions;
    glm::vec3 index;
};

class SphereMesh
{
public:
	//Vertex vertices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec3> index;
	SphereMesh(const std::string& filename);
    std::vector<glm::vec3> readMeshPos(const std::string &filename);
    std::vector<glm::uvec3> GenerateIndex();
private:
};

SphereMesh::SphereMesh(const std::string& filename)
{
    std::vector<glm::vec3> vertices = readMeshPos(filename);
    this->index = GenerateIndex();
    this->vertices = vertices;
}

std::vector<glm::vec3> SphereMesh::readMeshPos(const std::string& filename) 
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

std::vector<glm::uvec3> SphereMesh::GenerateIndex()
{
    std::vector<glm::uvec3> outPos;
    //生成正三角形  50*100个
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 100; j++) {
            //outPos.emplace_back(j + i * 101,(j+ i*101)+1,j+(i+1)*101);
            outPos.push_back(glm::uvec3(j + i * 101, (j + i * 101) + 1, j + (i + 1) * 101));
        }
    }

    //生成倒三角形  50*100个
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 100; j++) {
            //outPos.emplace_back(j + i * 101,(j+ i*101)+1,j+(i+1)*101);
            outPos.push_back(glm::uvec3(j + i * 101 + 1, j + (i + 1) * 101, j + (i + 1) * 101 + 1));
        }
    }

    return outPos;
}