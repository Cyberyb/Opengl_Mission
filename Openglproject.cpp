using namespace std;

/// Openglproject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//#include<GL/glew.h>
#include"Shader.h"//因为包含了glad库，需要置顶
#include <iostream>
#include<GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include"SOIL2/stb_image.h"
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SphereMesh.h"
#include "CameraMesh.h"
#include "PointsMesh.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void mousebutton_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
void processMouseButton(GLFWwindow* window, int& count,bool &changed);

//render函数
//void renderSphereMesh(const Shader &shader);
//void renderSphereCamera(const Shader& shader);
//void renderSpherePoints(const Shader& shader);
void renderQuad();

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

float fov = 60.0f;
float yaw = -90.0f;//设置偏航角
float pitch = 0.0f;//设置俯仰角
bool firstmouse = true;//判断鼠标是否初次进入画面
float lastX = (float)SCR_WIDTH / 2;//设置上一帧鼠标位置，默认为平面中央
float lastY = (float)SCR_HEIGHT / 2;

//时间用于设置鼠标在每一帧的位置变化属性
float deltatime = 0.0f;
float lasttime = 0.0f;

glm::vec3 cameraposition = glm::vec3(0.0f, 0.0f, -10.0f);//摄像机位置
glm::vec3 camerafront = glm::vec3(0.2f, 0.0f, 1.0f);//
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);//上向量
glm::vec3 lightposition = glm::vec3(5.0, 5.0, 5.0);



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

void GetDirRightUp(glm::vec3 r,glm::vec3 &dir,glm::vec3 &right,glm::vec3 &up)
{
	glm::mat4 mat = glm::mat4(1.0f);

	glm::mat4 rr = glm::rotate(mat, glm::radians(r.z), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 rh = glm::rotate(mat, glm::radians(180 - r.x), glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 rp = glm::rotate(mat, glm::radians(r.y), glm::vec3(1.0, 0.0, 0.0));
	//ZXY顺归
	glm::mat4 res = rr * rp * rh;
	dir = glm::vec3(glm::vec4(0.0,0.0,1.0,0.0) * res);
	right = glm::vec3(glm::vec4(1.0, 0.0, 0.0, 0.0) * res);
	up = glm::cross(dir, right);
}

//glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);


int main()
{
	//glfw的初始化以及指明版本号
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Texture", NULL, NULL);

	//连接上下文
	glfwMakeContextCurrent(window);

	//注册回调函数
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//设置捕捉光标
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//初始化glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	//创建并且编译Shader程序
	Shader sphereShader("SphereMesh.vert", "SphereMesh.frag");
	Shader cameraShader("CameraMesh.vert", "CameraMesh.frag");
	Shader pointsShader("PointsMesh.vert", "PointsMesh.frag");
	Shader depthShader("DepthShader.vert", "DepthShader.frag");
	Shader QuardShader("Quard.vert", "Quard.frag");
	
	//获取Mesh
	SphereMesh sphereM("mesh.txt");
	CameraMesh cameraM("light.txt");
	PointsMesh pointsM("box.txt");

	std::vector<glm::vec3> vert = sphereM.vertices;
	std::vector<glm::uvec3> index = sphereM.index;

	std::vector<glm::vec3> camerapos = cameraM.GetCameraPos();
	std::vector<glm::vec3> camerarot = cameraM.GetCameraRot();
	std::vector<glm::vec4> camerafru = cameraM.GetCameraFru();
	std::vector<cameraVertex> cameraVert = cameraM.cameraVer;

	std::vector<glm::vec3> pointsvert = pointsM.points;


	//输出测试
	std::cout << vert.size() << " " << sizeof(vert) << " " << sizeof(vert[0]) << std::endl;
	std::cout << index.size() << " " << sizeof(index) << " " << sizeof(index[0]) << std::endl;

	//16:摄像机个数 vector的当前容量 16:vector的size?   12:vec3  3*4
	std::cout << camerapos.size() << " " << sizeof(camerapos) << " " << sizeof(camerapos[0]) << std::endl;
	std::cout << cameraVert.data()->camerafru.x << endl;


	//提前计算16个点的变换矩阵
	vector<glm::mat4> worldtoview;
	for (int k = 0; k < cameraVert.size(); k++) {
		glm::vec3 r = camerarot[k];
		glm::vec3 camera = camerapos[k];
		glm::mat4 worldToView = viewMatrix(-camera, r);
		worldtoview.push_back(worldToView);
	}
	cout << worldtoview[0][0].x << worldtoview[0][0].y<< worldtoview[0][0].z << worldtoview[0][0].w <<endl;

	glm::mat4 cameraSpaceMat = worldtoview[0];
	//Test
	//pointsShader.use();
	//glUniformMatrix4fv(glGetUniformLocation(pointsShader.ID, "cameraSpaceMat"), 16, GL_FALSE, &worldtoview[0][0][0]);

	depthShader.use();
	//glUniformMatrix4fv(glGetUniformLocation(depthShader.ID, "cameraSpaceMat"), 1, GL_FALSE, &cameraSpaceMat[0][0]);
	depthShader.setMat4("perspective", glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f));
	depthShader.setMat4("model", glm::mat4(1.0f));
	//depthShader.setMat4("cameraSpaceMat", glm::lookAt(cameraposition, cameraposition + camerafront, up));
	glm::vec3 dir, right, up;
	GetDirRightUp(camerarot[0], dir, right, up);
	glm::mat4 view = glm::lookAt(camerapos[0], camerapos[0] + dir, up);
	depthShader.setMat4("cameraSpaceMat", glm::lookAt(camerapos[0], camerapos[0] + dir, up));

	/*球幕Mesh*/
	unsigned int VBO[3], VAO, EBO;//创建顶点缓冲对象、顶点数组对象
	//生成对应对象
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO[0]);//球幕VBO
	glGenBuffers(1, &VBO[1]);//摄像机VBO
	glGenBuffers(1, &VBO[2]);
	glGenBuffers(1, &EBO);

	//绑定VAO
	glBindVertexArray(VAO);
	//绑定VBO并传入数据
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * vert.size(), vert.data(), GL_STATIC_DRAW);
	//绑定EBO并传入数据
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 30000 * sizeof(unsigned int), index.data(), GL_STATIC_DRAW);

	//位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//传Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//传Normal
	glEnableVertexAttribArray(1);


	unsigned int cameraVAO;
	glGenVertexArrays(1, &cameraVAO);
	glBindVertexArray(cameraVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 10 * cameraVert.size(), cameraVert.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);//传Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3 * sizeof(float)));//传Rot
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6 * sizeof(float)));//传Fru
	glEnableVertexAttribArray(2);

	unsigned int pointsVAO;
	glGenVertexArrays(1, &pointsVAO);
	glBindVertexArray(pointsVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * pointsvert.size(), pointsvert.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//传Position
	glEnableVertexAttribArray(0);
	

	//sphereShader.use();

	/*---------------------------深度贴图------------------------------*/
	unsigned int depthMapFBO;//创建帧缓冲
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;//创建深度贴图
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// attach depth texture as FBO's depth buffer 将深度贴图与帧缓冲绑定
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);




	//启用深度测试，glEnable和glDisable可以启用或者关闭opengl的某个功能
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	int viewMatCount = 0;//用于16个摄像机的视角切换计数
	//渲染循环
	bool mouseButtonChanged = true;

	while (!glfwWindowShouldClose(window))
	{
		//时间设置
		float currenttime = glfwGetTime();
		deltatime = currenttime - lasttime;
		lasttime = currenttime;

		processInput(window);
		processMouseButton(window, viewMatCount, mouseButtonChanged);
		//更新view矩阵
		GetDirRightUp(camerarot[viewMatCount], dir, right, up);
		view = glm::lookAt(camerapos[viewMatCount], camerapos[viewMatCount] + dir, up);
		//view = worldtoview[viewMatCount];

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		depthShader.use();
		depthShader.setMat4("view", view);

		/*----------------画球幕-----------------*/
		//sphereShader.use();

		//MVP Matrix

		//摄像机空间
		//glm::mat4 view = glm::mat4(1.0f);

		//view = glm::lookAt(cameraposition, cameraposition + camerafront, up);//定义lookat矩阵


		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		sphereShader.setMat4("view", view);
		sphereShader.setMat4("proj", projection);
		//模型矩阵，控制物体的旋转
		glm::mat4 model = glm::mat4(1.0f);
		sphereShader.setMat4("model", model);
		sphereShader.setVec3("lightPos", lightposition);

		//传光源位置
		


		glBindVertexArray(VAO);


		//glPointSize(10.f);
		//glDrawArrays(GL_POINTS, 0, vert.size());
		
		glDrawElements(GL_TRIANGLES, vert.size() * sizeof(float) * 3, GL_UNSIGNED_INT, nullptr);

		/*----------------画摄像机-----------------*/
		//cameraShader.use();

		cameraShader.setMat4("view", view);
		cameraShader.setMat4("proj", projection);
		//模型矩阵，控制物体的旋转
		//glm::mat4 model_c = glm::mat4(1.0f);
		cameraShader.setMat4("model", model);


		glBindVertexArray(cameraVAO);
		glPointSize(20.0f);
		glDrawArrays(GL_POINTS, 0, cameraVert.size());

		/*----------------画采样点-----------------*/
		//pointsShader.use();

		pointsShader.setMat4("view", view);
		pointsShader.setMat4("proj", projection);
		//模型矩阵，控制物体的旋转
		pointsShader.setMat4("model", model);


		glBindVertexArray(pointsVAO);
		glPointSize(2.0f);
		glDrawArrays(GL_POINTS, 0, pointsvert.size());

		/*---------------画深度贴图---------------*/
		//depthShader.use();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		QuardShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderQuad();

		//收尾阶段
		glfwSwapBuffers(window);
		glfwPollEvents();

	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &cameraVAO);
	//glDeleteVertexArrays(1, &pointsVAO);
	glDeleteBuffers(1, &VBO[0]);
	glDeleteBuffers(1, &VBO[1]);
	glDeleteBuffers(1, &VBO[2]);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}


void mouse_callback(GLFWwindow*window, double xpos, double ypos)//鼠标坐标的调用，用于控制视角
{
	if (firstmouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstmouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;//设置鼠标灵敏度
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;//设置俯仰角和偏转角
	pitch += yoffset;

	if (pitch > 89.0f)//设置俯仰角的限制
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	//计算方向向量
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	camerafront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	else if (fov >= 45.0f)
		fov = 45.0f;
}

void processInput(GLFWwindow* window)
{
	//keybord
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	float cameraspeed = 2.5f *deltatime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraposition += cameraspeed * glm::vec3(camerafront.x,camerafront.y,camerafront.z);//视角移动，会改变y值
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraposition -= cameraspeed * glm::vec3(camerafront.x, camerafront.y, camerafront.z);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraposition -= glm::normalize(glm::cross(camerafront, up)) * cameraspeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraposition += glm::normalize(glm::cross(camerafront, up)) * cameraspeed;
}

void processMouseButton(GLFWwindow* window,int &count,bool &changed)
{
	//mouse
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && changed)
	{
		count = (count + 1) % 16;
		changed = false;
		cout << changed << endl;
	}
	else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && !changed )
	{
		changed = true;
		cout << changed << endl;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}