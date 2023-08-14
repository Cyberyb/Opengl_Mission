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
    int Row = 101;
    int Col = 51;

	//Vertex vertices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec3> index;
	SphereMesh(const std::string& filename);
    std::vector<glm::vec3> readMeshPos(const std::string &filename);
    std::vector<glm::uvec3> GenerateIndex(int row,int col);
private:
};

SphereMesh::SphereMesh(const std::string& filename)
{
    std::vector<glm::vec3> vertices = readMeshPos(filename);
    this->index = GenerateIndex(Row,Col);
    this->vertices = vertices;
}

std::vector<glm::vec3> SphereMesh::readMeshPos(const std::string& filename) 
{
    std::vector<glm::vec3> outPos;
	std::ifstream file(filename);
    std::string line;
    int row, col;

    if (std::getline(file, line))
    {
        std::istringstream iss(line);
        int count = 0;
        float tempValue;
        while (iss >> tempValue) {
            count++;
        }

        iss.clear();//清除状态位
        iss.seekg(0);//将流指针重置到开头

        if (count == 2)
        {
            iss >> row >> col;
            this->Row = row;
            this->Col = col;
        }
        else 
            file.seekg(0);
    }



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

std::vector<glm::uvec3> SphereMesh::GenerateIndex(const int row,const int col)
{
    std::vector<glm::uvec3> outPos;
    //生成正三角形  50*100个
    //for (int i = 0; i < 50; i++) {
    //    for (int j = 0; j < 100; j++) {
    //        //outPos.emplace_back(j + i * 101,(j+ i*101)+1,j+(i+1)*101);
    //        outPos.push_back(glm::uvec3(j + i * 101, (j + i * 101) + 1, j + (i + 1) * 101));
    //    }
    //}

    //生成倒三角形  50*100个
    //for (int i = 0; i < 50; i++) {
    //    for (int j = 0; j < 100; j++) {
    //        //outPos.emplace_back(j + i * 101,(j+ i*101)+1,j+(i+1)*101);
    //        outPos.push_back(glm::uvec3(j + i * 101 + 1, j + (i + 1) * 101, j + (i + 1) * 101 + 1));
    //    }
    //}

    //生成正三角形  50*100个
    for (int i = 0; i < (col - 1); i++) {
        for (int j = 0; j < (row -1); j++) {
            outPos.push_back(glm::uvec3(j + i * row, (j + i * row) + 1, j + (i + 1) * row));
        }
    }

    //生成倒三角形  50*100个
    for (int i = 0; i < (col - 1); i++) {
        for (int j = 0; j < (row - 1); j++) {
            outPos.push_back(glm::uvec3(j + i * row + 1, j + (i + 1) * row, j + (i + 1) * row + 1));
        }
    }

    return outPos;
}