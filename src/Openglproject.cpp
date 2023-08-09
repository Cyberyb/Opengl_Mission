using namespace std;

/// Openglproject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//#include<GL/glew.h>
#include"Shader.h"//因为包含了glad库，需要置顶
#include <iostream>
#include<GLFW/glfw3.h>
//#define STB_IMAGE_IMPLEMENTATION
//#include"SOIL2/stb_image.h"
//#include<SOIL2/SOIL2.h>
//#include<SOIL2/stb_image_write.h>
#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Camera.h"
#include "SphereMesh.h"
#include "CameraMesh.h"
#include "PointsMesh.h"
#include "OutputData.h"
#include "GenMesh.h"
#include "GenVolum.h"

#define POI_X 64
#define POI_Y 64
#define POI_Z 64

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
void processMouseButton(GLFWwindow* window, int& count,bool &changed);
void processMousePress(GLFWwindow* window);

//render函数
//void renderSphereMesh(const Shader &shader);
//void renderSphereCamera(const Shader& shader);
//void renderSpherePoints(const Shader& shader);
void renderQuad();
void renderUI(bool& show_demo_window, bool& show_another_window);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

const unsigned int VISI_WIDTH = 2500;
const unsigned int VISI_HEIGHT = 50;

//const int POI_X = 64;
//const int POI_Y = 64;
//const int POI_Z = 64;

const unsigned int POI_WIDTH = 50;
const unsigned int POI_HEIGHT = 50;

float fov = 60.0f;
float yaw = 90.0f;//设置偏航角
float pitch = 0.0f;//设置俯仰角
bool firstmouse = true;//判断鼠标是否初次进入画面
double lastX = (float)SCR_WIDTH / 2;//设置上一帧鼠标位置，默认为平面中央
double lastY = (float)SCR_HEIGHT / 2;

//时间用于设置鼠标在每一帧的位置变化属性
float deltatime = 0.0f;
float lasttime = 0.0f;

glm::vec3 cameraposition = glm::vec3(0.0f, 0.0f, -10.0f);//摄像机位置
glm::vec3 lightposition = glm::vec3(5.0, 5.0, 5.0);
Camera camera(cameraposition);


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

glm::mat4 GetFrustumbyangle(float left, float right, float bottom, float top,float near, float far)
{
	const float PI = 3.1415;
	float leftC = -near * std::tan(left * PI / 180.0);
	float rightC = near * std::tan(right * PI / 180.0);
	float topC = near * std::tan(top * PI / 180.0);
	float bottomC = -near * std::tan(bottom * PI / 180.0);
	//frustum需要通过near来计算参数
	return glm::frustum(leftC, rightC, bottomC, topC,near,far);
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

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}


int main()
{
	glfwSetErrorCallback(glfw_error_callback);
	//glfw的初始化以及指明版本号
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//创建窗口
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Missing Points", NULL, NULL);

	//连接上下文
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	//设置imgui上下文
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	//设置imgui风格
	ImGui::StyleColorsDark();

	//设置平台、渲染器后端
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	//imgui字体
	io.Fonts->AddFontFromFileTTF("imgui/YaHei.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	io.Fonts->Build();

	//imgui状态
	bool show_demo_window = true;
	bool show_another_window = false;
	bool linemode_Sphere = true;
	bool linemode_Main = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 main_color = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);

	//注册回调函数
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);

	//设置捕捉光标
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//初始化glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	//创建并且编译Shader程序
	Shader sphereShader("./shader/SphereMesh.vert", "./shader/SphereMesh.frag");
	Shader cameraShader("./shader/CameraMesh.vert", "./shader/CameraMesh.frag");
	Shader pointsShader("./shader/PointsMesh.vert", "./shader/PointsMesh.frag");
	Shader depthShader("./shader/DepthShader.vert", "./shader/DepthShader.frag");
	Shader QuardShader("./shader/Quard.vert", "./shader/Quard.frag");
	
	//获取Mesh
	SphereMesh sphereM("mesh.txt");
	CameraMesh cameraM("light.txt");
	//PointsMesh pointsM("box.txt");

	std::vector<glm::vec3> vert = sphereM.vertices;
	std::vector<glm::uvec3> index = sphereM.index;

	std::vector<glm::vec3> camerapos = cameraM.GetCameraPos();
	std::vector<glm::vec3> camerarot = cameraM.GetCameraRot();
	std::vector<glm::vec4> camerafru = cameraM.GetCameraFru();
	std::vector<cameraVertex> cameraVert = cameraM.cameraVer;

	//std::vector<glm::vec3> pointsvert = pointsM.points;


	//输出测试

	//16:摄像机个数 vector的当前容量 16:vector的size?   12:vec3  3*4
	//std::cout << camerapos.size() << " " << sizeof(camerapos) << " " << sizeof(camerapos[0]) << std::endl;
	//std::cout << cameraVert.data()->camerafru.x << endl;


	//提前计算16个摄像机的变换矩阵
	vector<glm::mat4> worldtoview;
	for (int k = 0; k < cameraVert.size(); k++) {
		glm::vec3 r = camerarot[k];
		glm::vec3 camera = camerapos[k];

		//方法一：使用自定义的viewMatrix函数（有问题，生成的View矩阵第一三行符号相反）
		//glm::mat4 worldToView = viewMatrix(-camera, r);

		//方法二：计算lookAt矩阵（正确的，中肯的）
		glm::vec3 dir, right, up;
		GetDirRightUp(camerarot[k], dir, right, up);
		glm::mat4 worldToView = glm::lookAt(camerapos[k], camerapos[k] + dir, up);
		worldtoview.push_back(worldToView);
	}

	//提前计算16个摄像机的Frustum
	vector<glm::mat4> allfrustum;
	for (int j = 0; j < cameraVert.size(); j++) {
		glm::mat4 fru = GetFrustumbyangle(camerafru[j].x, camerafru[j].y, camerafru[j].w, camerafru[j].z, 0.1f, 100.0f);
		allfrustum.push_back(fru);
	}

	//glm::mat4 fru = GetFrustumbyangle(camerafru[0].x, camerafru[0].y, camerafru[0].w, camerafru[0].z, 0.1f, 100.0f);
	QuardShader.use();
	glUniformMatrix4fv(glGetUniformLocation(QuardShader.ID, "view"), 16, GL_FALSE, &worldtoview[0][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(QuardShader.ID, "proj"), 16, GL_FALSE, &allfrustum[0][0][0]);
	sphereShader.use();
	glUniformMatrix4fv(glGetUniformLocation(sphereShader.ID, "view"), 16, GL_FALSE, &worldtoview[0][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(sphereShader.ID, "proj"), 16, GL_FALSE, &allfrustum[0][0][0]);

	/*球幕Mesh*/
	unsigned int VBO[3], VAO, EBO;//创建顶点缓冲对象、顶点数组对象
	//生成对应对象
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO[0]);//球幕VBO
	glGenBuffers(1, &VBO[1]);//摄像机VBO
	//glGenBuffers(1, &VBO[2]);
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

	//unsigned int pointsVAO;
	//glGenVertexArrays(1, &pointsVAO);
	//glBindVertexArray(pointsVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * pointsvert.size(), pointsvert.data(), GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//传Position
	//glEnableVertexAttribArray(0);
	

	//sphereShader.use();

	/*---------------------------深度贴图------------------------------*/
	unsigned int depthMapFBO[16];//创建帧缓冲
	for (int fboC = 0; fboC < 16; fboC++)
	{
		glGenFramebuffers(1, &depthMapFBO[fboC]);
	}
	
	// create depth texture
	unsigned int depthMap[16];//创建深度贴图
	for (int dmpC = 0; dmpC < 16; dmpC++)
	{
		glGenTextures(1, &depthMap[dmpC]);
		glBindTexture(GL_TEXTURE_2D, depthMap[dmpC]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[dmpC]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[dmpC], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
			cout << "Framebuffer complete!" << endl;
		else
			cout << "Framebuffer not complete! " << endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}



	// attach depth texture as FBO's depth buffer 将深度贴图与帧缓冲绑定
	

	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);



	/*---------------------------创建用于保存信息的贴图------------------------------*/
	//unsigned int visiMapFBO[POI_Y];//创建帧缓冲
	unsigned int *visiMapFBO = new unsigned int[POI_Y];
	glGenFramebuffers(POI_Y, visiMapFBO);

	unsigned int *visiMap = new unsigned int[POI_Y];//创建贴图
	glGenTextures(POI_Y, visiMap);

	for (int vmpC = 0; vmpC < POI_Y; vmpC++)
	{
		glBindTexture(GL_TEXTURE_2D, visiMap[vmpC]);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, POI_X, POI_Z, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, visiMapFBO[vmpC]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, visiMap[vmpC], 0);
	}

	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		cout << "Framebuffer complete!" << endl;
	else
		cout << "Framebuffer not complete! " << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//启用深度测试，glEnable和glDisable可以启用或者关闭opengl的某个功能
	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);


	//glEnable(GL_STENCIL_TEST);

	int viewMatCount = 0;//用于16个摄像机的视角切换计数  |   用于不同layer的切换
	//渲染循环
	bool mouseButtonChanged = true;


	//单次渲染，1.渲染16张深度贴图  2.渲染POI_Y层筛选后点的分布贴图

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, 500, 500);
	glm::mat4 model = glm::mat4(1.0f);
	/*----------------将球幕的深度贴图存入帧缓冲中-----------------*/
	/*glStencilMask允许我们设置一个位掩码(Bitmask)，它会与将要写入缓冲的模板值进行与(AND)运算。
	默认情况下设置的位掩码所有位都为1，不影响输出*/
	//glStencilMask(0xFF);
	//glStencilFunc(GL_ALWAYS, 1, 0xFF);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	sphereShader.use();
	sphereShader.setMat4("model", model);
	sphereShader.setVec3("lightPos", lightposition);//用于一点点diffuse光照
	for (int i = 0; i < 16; i++)
	{
		sphereShader.setInt("count", i);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
		glClear(GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, vert.size() * sizeof(float) * 3, GL_UNSIGNED_INT, nullptr);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 500, 500);
	}


	/*------------获取POI_X * POI_Z个点的可视性情况纹理，共计POI_Y层---------*/
	glViewport(0, 0, POI_X, POI_Z);

	glClear(GL_COLOR_BUFFER_BIT);

	QuardShader.use();
	QuardShader.setInt("count", viewMatCount);//传递摄像机计数
	QuardShader.setMat4("model", model);
	QuardShader.setInt("row", POI_X);
	QuardShader.setInt("col", POI_Z);
	QuardShader.setInt("hei", POI_Y);
	//QuardShader.setInt("floors",);
	GLint texIndex[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	glUniform1iv(glGetUniformLocation(QuardShader.ID, "depthMap"), 16, texIndex);

	for (int i = 0; i < 16; i++)
	{

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, depthMap[i]);
	}

	//逐层渲染
	for (int layer = 0; layer < POI_Y; layer++)
	{
		QuardShader.setInt("layer", layer);
		glBindFramebuffer(GL_FRAMEBUFFER, visiMapFBO[layer]);
		glClear(GL_COLOR_BUFFER_BIT);
		renderQuad();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, POI_X, POI_Z);
	}

	/*--------------输出模块/挖空算法/网格点结构体生成/网格索引生成---------------*/
	setOutputFormat(POI_X, POI_Z, POI_Y);

	std::vector<glm::vec3> volumePoints;
	std::vector<glm::ivec3> volumeIndex;
	unsigned int volumeVAO[POI_Y];
	unsigned int volumeVBO[POI_Y];
	unsigned int volumeEBO[POI_Y];
	glGenVertexArrays(POI_Y, volumeVAO);
	glGenBuffers(POI_Y, volumeVBO);
	glGenBuffers(POI_Y, volumeEBO);

	unsigned int points_count[POI_Y] = { 0 };
	unsigned int elements_count[POI_Y] = { 0 };

	int textureWidth;
	//int seq = 0;//用于生成序号
	//vector<vector<int>> pointSeq;
	//vector <vector<MeshPoint>> Meshp;
	setFormat_v(POI_X, POI_Y, POI_Z);
	setFormat(POI_X, POI_Y, POI_Z);
	for (int layer = 1; layer < POI_Y -1; layer++)
	{
		std::vector<unsigned char> pixels(POI_X * POI_Z);
		glBindTexture(GL_TEXTURE_2D, visiMap[layer]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

		std::vector<unsigned char> pixels_down(POI_X * POI_Z);
		glBindTexture(GL_TEXTURE_2D, visiMap[layer-1]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels_down.data());

		std::vector<unsigned char> pixels_up(POI_X * POI_Z);
		glBindTexture(GL_TEXTURE_2D, visiMap[layer+1]);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels_up.data());

		
		CullArea_Six(pixels,pixels_down,pixels_up);

		volumePoints = GenVolumePoints(pixels, layer, points_count[layer]);
		volumeIndex = GenVolumeIndex(pixels,elements_count[layer]);

		glBindVertexArray(volumeVAO[layer]);
		glBindBuffer(GL_ARRAY_BUFFER, volumeVBO[layer]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * volumePoints.size(), volumePoints.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//传Position
		glEnableVertexAttribArray(0);
		glBindBuffer(0, volumeVBO[layer]);
		
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, volumeEBO[layer]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, volumeIndex.size() * 3 * sizeof(unsigned int), volumeIndex.data(), GL_STATIC_DRAW);



		//Meshp.push_back(GetPosAndSeq(pixels, layer,seq_count));

		savePixelsToTxt(pixels, "matchPoints.txt", layer);
	}
	//cout << "points counts" << pointSeq.size() << endl;

	//获取处理后的点数据，绑定VAO、VBO
	//PointsMesh pointsM("matchPoints.txt");
	//std::vector<glm::vec3> pointsvert = pointsM.points;

	//unsigned int pointsVAO;
	//glGenVertexArrays(1, &pointsVAO);
	//glBindVertexArray(pointsVAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * pointsvert.size(), pointsvert.data(), GL_STATIC_DRAW);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//传Position
	//glEnableVertexAttribArray(0);


	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	while (!glfwWindowShouldClose(window))
	{
		//时间设置
		float currenttime = glfwGetTime();
		deltatime = currenttime - lasttime;
		lasttime = currenttime;

		processInput(window);
		//processMouseButton(window, viewMatCount, mouseButtonChanged);
		processMousePress(window);


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		




		/*----------------画球幕-----------------*/
		/*glStencilMask允许我们设置一个位掩码(Bitmask)，它会与将要写入缓冲的模板值进行与(AND)运算。
		默认情况下设置的位掩码所有位都为1，不影响输出*/
		if(linemode_Sphere)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		sphereShader.use();
		glm::mat4 view = glm::mat4(1.0);
		view = camera.GetviewMatrix();
		glm::mat4 projection = glm::mat4(1.0);
		projection = glm::perspective(glm::radians(camera.Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		sphereShader.setMat4("camera_view", view);//定义lookat矩阵);
		sphereShader.setMat4("camera_proj", projection);
		sphereShader.setInt("free", 1);
		//模型矩阵，控制物体的旋转
		sphereShader.setMat4("model", model);
		sphereShader.setVec3("lightPos", lightposition);

		glBindVertexArray(VAO);

		glDrawElements(GL_TRIANGLES, vert.size() * sizeof(float) * 3, GL_UNSIGNED_INT, nullptr);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		/*----------------画摄像机-----------------*/

		cameraShader.use();


		cameraShader.setMat4("view", view);
		cameraShader.setMat4("proj", projection);
		//模型矩阵，控制物体的旋转
		//glm::mat4 model_c = glm::mat4(1.0f);
		cameraShader.setMat4("model", model);


		glBindVertexArray(cameraVAO);
		glPointSize(20.0f);
		glDrawArrays(GL_POINTS, 0, cameraVert.size());

		/*----------------画采样点-----------------*/
		if (linemode_Main)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		pointsShader.use();

		pointsShader.setMat4("view", view);
		pointsShader.setMat4("proj", projection);
		pointsShader.setMat4("model", model);
		pointsShader.setVec4("main_color", glm::vec4(main_color.x,main_color.y,main_color.z,main_color.w));

		for (int i = 0; i < POI_Y; i++)
		{
			glBindVertexArray(volumeVAO[i]);
			//glPointSize(2.0f);
			//glDrawArrays(GL_POINTS, 0, points_count[i] * 8);
			glDrawElements(GL_TRIANGLES, elements_count[i] * 36, GL_UNSIGNED_INT, nullptr);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//





		/*-----------显示一层POI_X * POI_Z点的情况-------------*/

		//glViewport(0, 0,500, 500);
	 //   depthShader.use();
		////QuardShader.setInt("floors",);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, visiMap[viewMatCount]);
		//renderQuad();

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		/*-----------Imgui绘制-------------*/
		//renderUI(show_demo_window, show_another_window);

		glfwPollEvents();
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			static float f = 0.0f;
			static int counter = 0;
			static int size = POI_Y;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text((const char*)u8"是否启用线框模式");               // Display some text (you can use a format strings too)
			ImGui::Checkbox((const char*)u8"球幕", &linemode_Sphere);      // Edit bools storing our window open/close state
			ImGui::Checkbox((const char*)u8"主体", &linemode_Main);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3((const char*)u8"主体颜色", (float*)&main_color); // Edit 3 floats representing a color

			ImGui::SliderInt((const char*)u8"设定大小", &size, 16, 512);
			if (ImGui::Button("重置大小"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}


		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);

	}

	
	//输出PNG格式图像
	//int err = SOIL_save_image("Points_visiable.png", SOIL_SAVE_TYPE_PNG, 2500,50, SOIL_LOAD_RGBA, pixels.data());
	//if (err)
	//	cout << "Done" << endl;
	//else
	//	cout << "Failed" << endl;



	

	//std::vector<unsigned char> Depthpixels(SCR_WIDTH * SCR_HEIGHT);
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, Depthpixels.data());

	//int err_d = SOIL_save_image("Depth.png", SOIL_SAVE_TYPE_PNG, SCR_WIDTH ,SCR_HEIGHT, 1, Depthpixels.data());
	//if (err_d)
	//	cout << "Done" << endl;
	//else
	//	cout << "Failed" << endl;

	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &cameraVAO);
	glDeleteVertexArrays(POI_Y, volumeVAO);
	//glDeleteVertexArrays(1, &pointsVAO);
	glDeleteBuffers(1, &VBO[0]);
	glDeleteBuffers(1, &VBO[1]);
	//glDeleteBuffers(1, &VBO[2]);
	glDeleteBuffers(POI_Y, volumeVBO);
	//delete(volumeVBO);
	glDeleteBuffers(1, &EBO);
	for(int i = 0;i<16;i++)
		glDeleteFramebuffers(1, &depthMapFBO[i]);

	glfwTerminate();
	return 0;
}

void renderUI(bool &show_demo_window,bool &show_another_window)
{

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
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	//camera.ProcessMouseMove(xoffset, yoffset);
	camera.ProcessMousePressMove(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScoll(yoffset);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltatime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltatime);
}

void processMouseButton(GLFWwindow* window,int &count,bool &changed)
{
	//mouse
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && changed)
	{
		count = (count + 1) % POI_Y;
		changed = false;
	}
	else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && !changed )
	{
		changed = true;
	}
}

void processMousePress(GLFWwindow* window)
{
	//mouse
	double lx = lastX, ly = lastY;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		
		glfwGetCursorPos(window, &lastX, &lastY);
		double xoffset = lx - lastX;
		double yoffset = lastY - ly;
		//lastX = lx;
		//lastY = ly;
		camera.ProcessMousePressMove(xoffset, yoffset);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwGetCursorPos(window, &lastX, &lastY);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		if (action == GLFW_PRESS)
		{

		}
		else if (action == GLFW_RELEASE) {
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}