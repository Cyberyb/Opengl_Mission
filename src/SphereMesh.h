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

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    float Radius;

	//Vertex vertices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::uvec3> index;
	SphereMesh(const std::string& filename);
    ~SphereMesh();
    std::vector<glm::vec3> readMeshPos(const std::string &filename);
    std::vector<glm::uvec3> GenerateIndex(int row,int col);
    void Bind();
    void ReBuid(const std::string& filename);
private:
};

SphereMesh::SphereMesh(const std::string& filename)
{
    std::vector<glm::vec3> vertices = readMeshPos(filename);
    this->index = GenerateIndex(Row,Col);
    this->vertices = vertices;
}

SphereMesh::~SphereMesh()
{
    glDeleteBuffers(1, &(this->VBO));
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
}

void SphereMesh::ReBuid(const std::string& filename)
{
    this->vertices = readMeshPos(filename);
    this->index = GenerateIndex(Row, Col);
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


    float maxlength = 0;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        float x, y, z;
        
        if (!(iss >> x >> y >> z))
        {
            std::cout << "Reading End" << std::endl;
            if (glm::length(glm::vec3(x,y,z)) > maxlength)
            {
                maxlength = glm::length(glm::vec3(x, y, z));
            }
            break;
        }
        outPos.emplace_back(x, y, z);
    }
    this->Radius = maxlength;
    cout << "Sphere Mesh Radius: " << maxlength << endl;
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

void SphereMesh::Bind()
{
    //生成对应对象
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);//球幕VBO
    glGenBuffers(1, &this->EBO);

    //绑定VAO
    glBindVertexArray(this->VAO);
    //绑定VBO并传入数据
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);
    //绑定EBO并传入数据
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * this->index.size(), this->index.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//传Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//传Normal
    glEnableVertexAttribArray(1);

    glBindBuffer(0, this->VBO);
    glBindBuffer(0, this->EBO);
}