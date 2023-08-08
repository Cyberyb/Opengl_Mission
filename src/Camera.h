#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<vector>
#include<iostream>

enum Camera_Move
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = 90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;


class Camera
{
public:
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Front = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 Right = glm::vec3(1.0);
	glm::vec3 WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 Target = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 rotateaxisX = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 rotateaxisY = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 rotateaxisZ = glm::vec3(0.0f, 0.0f, 1.0f);

	float Yaw;
	float Pitch;
	float Fov;
	float MouseSpeed;
	float Sensitivity;
	double Xoff;
	double Yoff;

	float modelYaw = 0.0f;
	float modelPitch = 0.0f;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :Front(glm::vec3(0.0f,0.0f,1.0f)),MouseSpeed(SPEED),Sensitivity(SENSITIVITY),Fov(FOV)
	{
		Position = position;
		Up = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	Camera(float posX, float posY, float posZ,float upX,float upY,float upZ, float yaw = YAW, float pitch = PITCH) :Front(glm::vec3(0.0f, 0.0f, 1.0f)), MouseSpeed(SPEED), Sensitivity(SENSITIVITY), Fov(FOV)
	{
		Position = glm::vec3(posX,posY,posZ);
		Up = glm::vec3(upX,upY,upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	//观察矩阵/Lookat矩阵
	glm::mat4 GetviewMatrix()
	{
		return glm::lookAt(Position,Position+Front,Up);
	}

	//键盘操作
	void ProcessKeyboard(Camera_Move direction, float deltatime)
	{
		float velocity = MouseSpeed * deltatime;
		switch (direction)
		{
		case FORWARD:Position += Front * velocity;
			break;
		case BACKWARD:Position -= Front * velocity;
			break;
		case LEFT:Position -= Right * velocity;
			break; 
		case RIGHT:Position += Right * velocity;
			break;
		default:
			break;
		}
	}

	//鼠标移动
	void ProcessMouseMove(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= Sensitivity;
		yoffset *= Sensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void ProcessMousePressMove(float xoffset, float yoffset)
	{
		xoffset *= Sensitivity;
		yoffset *= Sensitivity;

		modelYaw += xoffset;
		modelPitch += yoffset;

		glm::mat4 mat(1.0f);
		
		rotateaxisY = glm::mat3(glm::rotate(mat, glm::radians(modelPitch), glm::vec3(1.0f, 0.0f, 0.0f))) * glm::vec3(0.0f,1.0f,0.0f);
		rotateaxisX = glm::mat3(glm::rotate(mat, glm::radians(modelYaw), glm::vec3(0.0f, 1.0f, 0.0f))) * glm::vec3(1.0f, 0.0f, 0.0f);
		std::cout << rotateaxisX.x << " " << rotateaxisX.y << " " << rotateaxisX.z << std::endl;
		 
		Xoff = xoffset;
		Yoff = yoffset;

		updateCameraPos();
	}

	//鼠标滚轮操作
	void ProcessMouseScoll(float yoffset)
	{
		Fov -= yoffset;
		if (Fov >= 89.0f)
			Fov = 89.0f;
		else if (Fov <= 1.0f)
			Fov - 1.0f;
	}
private:
	void updateCameraVectors()
	{
		//计算新的Front向量
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp)); 
		Up = glm::normalize(glm::cross(Right, Front));
	}

	void updateCameraPos()
	{
		//计算新的相机位置
		glm::mat4 mat(1.0);
		glm::mat4 posX = glm::rotate(mat, glm::radians((float)Xoff), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 posY = glm::rotate(mat, glm::radians((float)Yoff), rotateaxisX);
		glm::mat4 r = posY * posX;
		Position = glm::vec3(r * glm::vec4(Position,1.0));
		Front = Target - Position;
	}
};



















#endif

