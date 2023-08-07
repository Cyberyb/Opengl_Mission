#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <queue>
using namespace std;

//结构体，用于表示网格点
struct MeshPoint {
	glm::ivec3 pos = glm::ivec3(0);//位置
	int seq = 0;//序号
	int connect_count = 0;//连线个数
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

//用于剔除同一平面内的点，保留外层
void CullArea(vector<unsigned char>& pixels)
{
	//8个方向
	int x[] = { -1,0,1,-1,1,-1, 0, 1 };
	int y[] = { 1, 1,1, 0,0,-1,-1,-1 };
	//P用于保存修改前的数组，用于进行8邻域判断
	vector<unsigned char> p = pixels;
	for (int i = 0; i < HEI; i++)//行
	{
		for (int j = 0; j < WID; j++)//列
		{
			bool draw = false;
			for (int k = 0; k < 8; k++)
			{
				if ((i + y[k]) < 0 || (i + y[k]) >= LAY || (j + x[k]) < 0 || (j + x[k]) >= WID)
				{
					//超出纹理范围
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

//用于生成网格点的位置以及其序号
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


//用于查找同一层的下一个点   BFS算法
const int dx[] = {-1, -1, 0, 1, 1, 1, 0,-1};
const int dy[] = { 0, -1,-1,-1, 0, 1, 1, 1 };
int GetNextPoint(const vector<MeshPoint>& points,const vector<unsigned char>& pixels, MeshPoint& start,int layercount,int layer)
{
	int n = HEI;
	int m = WID;
	vector<vector<bool>> visited(n, vector<bool>(m, false));

	queue<MeshPoint> q;//用于记录位置信息
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

