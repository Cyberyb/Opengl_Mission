#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shlobj.h>
#include <cstring>
#include <atlstr.h>

namespace Utility
{
	glm::mat4 viewMatrix(glm::vec3 p, glm::vec3 r)
	{
		//glm::vec3 temp = glm::vec3(-46,103,161);
		// 生成旋转矩阵
		glm::mat4 mat = glm::mat4(1.0f);

		glm::mat4 rr = glm::rotate(mat, glm::radians(r.z), glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 rh = glm::rotate(mat, glm::radians(180 - r.x), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 rp = glm::rotate(mat, glm::radians(r.y), glm::vec3(1.0, 0.0, 0.0));
		//ZXY顺归
		glm::mat4 res = rr * rp * rh;

		// 坐标轴不跟随转
		//mat = glm::rotate(mat,glm::radians(temp.x),glm::vec3(1.0,0.0,0.0));
		//mat = glm::rotate(mat,glm::radians(temp.y),glm::vec3(0.0,1.0,0.0));
		//mat = glm::rotate(mat,glm::radians(temp.z),glm::vec3(0.0,0.0,1.0));
		//glm::mat4 res = mat;



		// 旋转之后再平移
		mat = glm::translate(res, p);
		return mat;
	}

	void GetDirRightUp(glm::vec3 r, glm::vec3& dir, glm::vec3& right, glm::vec3& up)
	{
		glm::mat4 mat = glm::mat4(1.0f);

		glm::mat4 rr = glm::rotate(mat, glm::radians(r.z), glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 rh = glm::rotate(mat, glm::radians(180 - r.x), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 rp = glm::rotate(mat, glm::radians(r.y), glm::vec3(1.0, 0.0, 0.0));
		//ZXY顺归
		glm::mat4 res = rr * rp * rh;
		dir = glm::vec3(glm::vec4(0.0, 0.0, 1.0, 0.0) * res);
		right = glm::vec3(glm::vec4(1.0, 0.0, 0.0, 0.0) * res);
		up = glm::cross(dir, right);
	}

	glm::mat4 GetFrustumbyangle(float left, float right, float bottom, float top, float n, float f)
	{
		const float PI = 3.1415;
		float leftC = -n * std::tan(left * PI / 180.0);
		float rightC = n * std::tan(right * PI / 180.0);
		float topC = n * std::tan(top * PI / 180.0);
		float bottomC = -n * std::tan(bottom * PI / 180.0);
		//frustum需要通过near来计算参数
		return glm::frustum(leftC, rightC, bottomC, topC, n, f);
	}

	//glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
	#define ASSERT(x) if((!x)) __debugbreak();
	#define GLCall(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x,__FILE__,__LINE__))

	void GLClearError()
	{
		while (glGetError() != GL_NO_ERROR);
	}
	bool GLLogCall(const char* function, const char* file, int line)
	{
		while (GLenum err = glGetError())
		{
			std::cout << "[OpenGL Error] " << err <<
				file << " " << function << " : " << line << std::endl;
			return false;
		}
		return true;
	}

	std::string string_To_UTF8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

		wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 
		ZeroMemory(pwBuf, nwLen * 2 + 2);

		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);

		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

		std::string retStr(pBuf);

		delete[]pwBuf;
		delete[]pBuf;

		pwBuf = NULL;
		pBuf = NULL;

		return retStr;
	}

}

