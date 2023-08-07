#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <queue>
using namespace std;

//�ṹ�壬���ڱ�ʾ�����
struct MeshPoint {
	glm::ivec3 pos = glm::ivec3(0);//λ��
	int seq = 0;//���
	int connect_count = 0;//���߸���
	int next_seq = 0;
	MeshPoint(glm::ivec3 position, int sq, int count,int nxsq)
	{
		pos = position;
		seq = sq;
		connect_count = count;
		next_seq = nxsq;
	}
};

struct PointPos {
	int x;
	int y;
	PointPos(int _x, int _y) : x(_x), y(_y) {}
};

static int WID = 64;   // Replace with your actual width
static int HEI = 64;  // Replace with your actual height
static int LAY = 64;

void setFormat(int width, int height, int layer)
{
	WID = width;
	HEI = height;
	LAY = layer;
}

//�����޳�ͬһƽ���ڵĵ㣬�������
void CullArea(vector<unsigned char>& pixels)
{
	//8������
	int x[] = { -1,0,1,-1,1,-1, 0, 1 };
	int y[] = { 1, 1,1, 0,0,-1,-1,-1 };
	//P���ڱ����޸�ǰ�����飬���ڽ���8�����ж�
	vector<unsigned char> p = pixels;
	for (int i = 0; i < HEI; i++)//��
	{
		for (int j = 0; j < WID; j++)//��
		{
			bool draw = false;
			for (int k = 0; k < 8; k++)
			{
				if ((i + y[k]) < 0 || (i + y[k]) >= LAY || (j + x[k]) < 0 || (j + x[k]) >= WID)
				{
					//��������Χ
					break;
				}

				if (p[(i + y[k]) * WID + (j + x[k])] == 0)
				{
					draw = true;
					break;
				}
			}
			if (!draw && pixels[i * WID + j] == 255)
				pixels[i * WID + j] = 0;
		}
	}
}

//��������������λ���Լ������
vector<MeshPoint> GetPosAndSeq(vector<unsigned char>& pixels,int layer,int &seq)
{
	vector<MeshPoint> PointsList;
	for (int i = 0; i<pixels.size();i++)
	{
		if (pixels[i] == 255)
		{
			PointsList.emplace_back(glm::ivec3(i % WID, layer, i / HEI), seq, 0, 0);
			seq++;
		}
	}
	return PointsList;
}


//���ڲ���ͬһ�����һ����   BFS�㷨
const int dx[] = {-1, -1, 0, 1, 1, 1, 0,-1};
const int dy[] = { 0, -1,-1,-1, 0, 1, 1, 1 };
int GetNextPoint(const vector<MeshPoint>& points,const vector<unsigned char>& pixels, MeshPoint& start,int layercount,int layer)
{
	int n = HEI;
	int m = WID;
	vector<vector<bool>> visited(n, vector<bool>(m, false));

	queue<MeshPoint> q;//���ڼ�¼λ����Ϣ
	q.push(start);

	//visited[start.pos.x][start.pos.y] = true;
	while (!q.empty()) {
		MeshPoint curr = q.front();
		q.pop();

		for (int i = 0; i < 8; ++i) {
			int new_x = curr.pos.x + dx[i];
			int new_y = curr.pos.z + dy[i];

			if (new_x >= 0 && new_x < n && new_y >= 0 && new_y < m && visited[new_x][new_y] == false) 
			{
				int seq = -1;
				visited[new_x][new_y] = true;
				if (pixels[new_y * WID + new_x] == 255) {
					//return findSeqByPos(points,glm::vec3(new_x,layer,new_y));
					return 0;
				}
				//q.push(MeshPoint());
			}
		}
	}

}

int findSeqByPos(const vector<MeshPoint>& points, glm::ivec3 pos)
{
	for (auto it = points.begin(); it != points.end(); it++)
	{
		if ((*it).pos == pos)
		{
			return (*it).seq;
		}
	}
	return -1;
}

