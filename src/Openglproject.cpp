using namespace std;

/// Openglproject.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//#include<GL/glew.h>
#include"Shader.h"//因为包含了glad库，需要置顶
#include<ctime>
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
#include "FileDialog.h"
#include "Utility.h"
#include "RecentFile.h"

//#define POI_X 64
//#define POI_Y 64
//#define POI_Z 64

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
void RenderData(int x,int y,int z);



const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

const unsigned int VISI_WIDTH = 2500;
const unsigned int VISI_HEIGHT = 50;

int POI_X = 16;
int POI_Y = 16;
int POI_Z = 16;

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

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

SphereMesh sphereM(RecentFile::GetRecentSphere());
CameraMesh cameraM(RecentFile::GetRecentLight());

void LoadSphereAndLight(SphereMesh& sphereM, CameraMesh& cameraM, Shader& sphereShader, Shader& quardShader)
{

	//读取文件
	//SphereMesh sphereM(Sphere);
	//CameraMesh cameraM(Light);
	//球幕顶点以及索引
	std::vector<glm::vec3> vert = sphereM.vertices;
	std::vector<glm::uvec3> index = sphereM.index;
	//光源摄像机对象
	std::vector<cameraVertex> cameraVert = cameraM.cameraVer;

	//提前计算多个光源摄像机的变换矩阵
	vector<glm::mat4> worldtoview = cameraM.CalWorld2View();


	//提前计算多个光源摄像机的Frustum
	vector<glm::mat4> allfrustum = cameraM.CalFrustum();

	quardShader.use();
	glUniformMatrix4fv(glGetUniformLocation(quardShader.ID, "view"), cameraVert.size(), GL_FALSE, &worldtoview[0][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(quardShader.ID, "proj"), cameraVert.size(), GL_FALSE, &allfrustum[0][0][0]);
	sphereShader.use();
	glUniformMatrix4fv(glGetUniformLocation(sphereShader.ID, "view"), cameraVert.size(), GL_FALSE, &worldtoview[0][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(sphereShader.ID, "proj"), cameraVert.size(), GL_FALSE, &allfrustum[0][0][0]);

	sphereM.Bind();
	cameraM.Bind();

	/*---------------------------深度贴图------------------------------*/
	cameraM.CreateDepthMap();

	glm::mat4 model = glm::mat4(1.0f);
	glEnable(GL_DEPTH_TEST);
	/*----------------将球幕的深度贴图存入帧缓冲中-----------------*/
	sphereShader.use();
	sphereShader.setMat4("model", model);
	sphereShader.setVec3("lightPos", glm::vec3(5.0, 5.0, 5.0));//用于一点点diffuse光照
	for (int i = 0; i < cameraVert.size(); i++)
	{
		sphereShader.setInt("count", i);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, cameraM.depthFBO[i]);
		glClear(GL_DEPTH_BUFFER_BIT);

		glBindVertexArray(sphereM.VAO);

		glDrawElements(GL_TRIANGLES, vert.size() * sizeof(float) * 3, GL_UNSIGNED_INT, nullptr);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

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
	bool render_Sphere = true;
	bool firstRender = true;
	int render_count = 0;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 main_color = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
	float points_side_length = 5.5;
	std::string sphere_file(RecentFile::GetRecentSphere());
	std::string camera_file(RecentFile::GetRecentLight());
	bool Calspherecamera = true;

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
	
	//SphereMesh sphereM("mesh.txt");
	//CameraMesh cameraM("light.txt");
	LoadSphereAndLight(sphereM, cameraM, sphereShader, QuardShader);

	
	glm::mat4 model = glm::mat4(1.0f);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//准备数据
	//unsigned int* volumeVAO;
	vector<unsigned int> volumeVAO;
	//vector<VBOList> volumeVBO;
	vector<unsigned int> volumeVBO;
	vector<unsigned int> volumeEBO;

	vector<unsigned int> points_count;
	vector<unsigned int> elements_count;

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
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


		if (firstRender)
		{
			double begin = clock();
			//volumeVAO = new unsigned int[POI_Y];
			volumeVAO.clear();
			volumeVBO.clear();
			volumeEBO.clear();
			volumeVAO.assign(POI_Y,0);
			volumeVBO.assign(POI_Y, 0);
			//volumeVBO.assign(POI_Y, {0,0});
			volumeEBO.assign(POI_Y,0);

			points_count.clear();
			points_count.assign(POI_Y,0);
			elements_count.clear();
			elements_count.assign(POI_Y, 0);


			/*---------------------------创建用于保存信息的贴图------------------------------*/
			unsigned int* visiMapFBO = new unsigned int[POI_Y];
			glGenFramebuffers(POI_Y, visiMapFBO);

			unsigned int* visiMap = new unsigned int[POI_Y];//创建贴图
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




			//单次渲染，1.渲染16张深度贴图  2.渲染POI_Y层筛选后点的分布贴图


			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			/*------------获取POI_X * POI_Z个点的可视性情况纹理，共计POI_Y层---------*/
			glViewport(0, 0, POI_X, POI_Z);

			glClear(GL_COLOR_BUFFER_BIT);

			QuardShader.use();
			//QuardShader.setInt("count", viewMatCount);//传递摄像机计数
			QuardShader.setMat4("model", model);
			QuardShader.setInt("row", POI_X);
			QuardShader.setInt("col", POI_Z);
			QuardShader.setInt("hei", POI_Y);
			QuardShader.setFloat("radius", sphereM.Radius);
			QuardShader.setFloat("sidelength", points_side_length);
			QuardShader.setInt("sizeOfDep", cameraM.cameraVer.size());
			//QuardShader.setInt("floors",);

			vector<GLint> texIndex;
			for (int i = 0; i < cameraM.cameraVer.size(); i++)
			{
				texIndex.push_back(i);
			}
			//GLint texIndex[] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
			glUniform1iv(glGetUniformLocation(QuardShader.ID, "depthMap"), cameraM.cameraVer.size(), texIndex.data());

			for (int i = 0; i < cameraM.cameraVer.size(); i++)
			{
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, cameraM.depthFBO[i]);
			}

			//逐层渲染
			for (int layer = 0; layer < POI_Y; layer++)
			{
				QuardShader.setInt("layer", layer);
				
				glBindFramebuffer(GL_FRAMEBUFFER, visiMapFBO[layer]);
				glClear(GL_COLOR_BUFFER_BIT);
				renderQuad();
				glBindFramebuffer(GL_FRAMEBUFFER, 0);				
			}
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			/*--------------输出模块/挖空算法/网格点结构体生成/网格索引生成---------------*/
			setOutputFormat(POI_X, POI_Z, POI_Y);

			std::vector<glm::vec3> volumePoints;
			std::vector<glm::ivec3> volumeIndex;
			std::vector<glm::vec3> volumeNormal;

			glGenVertexArrays(POI_Y, volumeVAO.data());
			glGenBuffers(POI_Y, volumeVBO.data());
			//for (int k = 0; k < POI_Y; k++)
			//{
			//	glGenBuffers(1, &volumeVBO[k].pVBO);
			//	glGenBuffers(1, &volumeVBO[k].nVBO);
			//}
			glGenBuffers(POI_Y, volumeEBO.data());


			int textureWidth;
			setFormat_v(POI_X, POI_Y, POI_Z, points_side_length);
			setFormat(POI_X, POI_Y, POI_Z);
			for (int layer = 1; layer < POI_Y - 1; layer++)
			{
				std::vector<unsigned char> pixels(POI_X * POI_Z);
				glBindTexture(GL_TEXTURE_2D, visiMap[layer]);
				glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

				std::vector<unsigned char> pixels_down(POI_X * POI_Z);
				glBindTexture(GL_TEXTURE_2D, visiMap[layer - 1]);
				glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels_down.data());

				std::vector<unsigned char> pixels_up(POI_X * POI_Z);
				glBindTexture(GL_TEXTURE_2D, visiMap[layer + 1]);
				glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels_up.data());


				CullArea_Six(pixels, pixels_down, pixels_up);

				volumePoints = GenVolumePoints(pixels, layer, points_count[layer]);
				volumeNormal = GenVolumeNormal(points_count[layer]);
				volumeIndex = GenVolumeIndex(pixels, elements_count[layer]);

				glBindVertexArray(volumeVAO[layer]);
				glBindBuffer(GL_ARRAY_BUFFER, volumeVBO[layer]);
				//cout << (sizeof(volumePoints) == (volumePoints.size() * sizeof(glm::vec3) * 3) ? 1 : 0) << endl;
				//cout << "size()*float*3 : " << volumePoints.size() * sizeof(float) * 3 << "  size()*vec3 : " << volumePoints.size() * sizeof(glm::vec3) << endl;
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * volumePoints.size() + volumeNormal.size()*sizeof(glm::vec3), nullptr, GL_STATIC_DRAW);
				glBufferSubData(GL_ARRAY_BUFFER, 0, volumePoints.size() * sizeof(glm::vec3), volumePoints.data());
				glBufferSubData(GL_ARRAY_BUFFER, volumePoints.size() * sizeof(glm::vec3), volumeNormal.size() * sizeof(glm::vec3), volumeNormal.data());
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//传Position
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(volumeNormal.size() * sizeof(glm::vec3)));				
				glEnableVertexAttribArray(1);
				glBindBuffer(0, volumeVBO[layer]);

				//glBindBuffer(GL_ARRAY_BUFFER, volumeVBO[layer]);
				//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * volumePoints.size(), volumePoints.data(), GL_STATIC_DRAW);
				//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//传normal
				//glEnableVertexAttribArray(1);
				//glBindBuffer(0, volumeVBO[layer]);
				


				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, volumeEBO[layer]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, volumeIndex.size() * 3 * sizeof(unsigned int), volumeIndex.data(), GL_STATIC_DRAW);

				//savePixelsToTxt(pixels, "matchPoints.txt", layer);
			}
			firstRender = false;
			glDeleteFramebuffers(POI_Y, visiMapFBO);
			delete[] visiMap;
			delete[] visiMapFBO;
			double end = clock();
			double duration = end - begin;
			std::cout << "Complete! Time takes "<<duration<<" ms" << endl;
		}
		




		/*----------------画球幕-----------------*/
		/*glStencilMask允许我们设置一个位掩码(Bitmask)，它会与将要写入缓冲的模板值进行与(AND)运算。
		默认情况下设置的位掩码所有位都为1，不影响输出*/
		if(linemode_Sphere)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		sphereShader.use();
		glm::mat4 view = glm::mat4(1.0);
		view = camera.GetviewMatrix();
		glm::mat4 projection = glm::mat4(1.0);
		glm::mat4 model = glm::mat4(1.0);
		projection = glm::perspective(glm::radians(camera.Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		sphereShader.setMat4("camera_view", view);//定义lookat矩阵);
		sphereShader.setMat4("camera_proj", projection);
		sphereShader.setInt("free", 1);
		//模型矩阵，控制物体的旋转
		sphereShader.setMat4("model", model);
		sphereShader.setVec3("lightPos", lightposition);

		glBindVertexArray(sphereM.VAO);
		if(render_Sphere)
			glDrawElements(GL_TRIANGLES, sphereM.vertices.size() * sizeof(float) * 3, GL_UNSIGNED_INT, nullptr);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		/*----------------画摄像机-----------------*/

		cameraShader.use();


		cameraShader.setMat4("view", view);
		cameraShader.setMat4("proj", projection);
		//模型矩阵，控制物体的旋转
		//glm::mat4 model_c = glm::mat4(1.0f);
		cameraShader.setMat4("model", model);


		glBindVertexArray(cameraM.VAO);
		glPointSize(20.0f);
		glDrawArrays(GL_POINTS, 0, cameraM.cameraVer.size());

		/*----------------画采样点-----------------*/
		if (linemode_Main)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_CULL_FACE);
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
		glDisable(GL_CULL_FACE);




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

			ImGui::Begin((const char*)u8"设置");                          // Create a window called "Hello, world!" and append into it.
			ImGui::Text((const char*)u8"当前球幕网格半径：%.3f",sphereM.Radius);

			ImGui::Checkbox((const char*)u8"球幕显示", &render_Sphere);
			//ImGui::Text((const char*)u8"是否启用线框模式");               // Display some text (you can use a format strings too)
			ImGui::Checkbox((const char*)u8"球幕线框模式", &linemode_Sphere);      // Edit bools storing our window open/close state
			ImGui::Checkbox((const char*)u8"主体线框模式", &linemode_Main);

			//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3((const char*)u8"主体颜色", (float*)&main_color); // Edit 3 floats representing a color
			ImGui::SliderFloat((const char*)u8"设定计算点云的边长", &points_side_length, 5.0, 8.0);
			ImGui::SliderInt((const char*)u8"设定大小", &size, 16, 1024);
			if (ImGui::Button((const char*)u8"重置大小"))                        // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				POI_X = size;
				POI_Y = size;
				POI_Z = size;
				firstRender = true;
			}
			if (ImGui::Button((const char*)u8"选择球幕文件"))
			{
				sphere_file = FileDlg::GetFileDialog();				
			}
			ImGui::Text(sphere_file.c_str());
			if (ImGui::Button((const char*)u8"选择摄像机文件"))
			{
				camera_file = FileDlg::GetFileDialog();				
			}
			ImGui::Text(camera_file.c_str());
			if (ImGui::Button((const char*)u8"计算"))
			{
				sphereM.ReBuid(sphere_file);
				cameraM.ReBuid(camera_file);
				//这一步非常关键，需要删除之前的FBO
				glDeleteFramebuffers(cameraM.cameraVer.size(), cameraM.depthFBO.data());
				LoadSphereAndLight(sphereM, cameraM, sphereShader, QuardShader);
				firstRender = true;
				//size = 16;
				//POI_X = 16;
				//POI_Y = 16;
				//POI_Z = 16;

				RecentFile::refreshRecent(sphere_file, camera_file);
			}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}


		ImGui::Render();
		int display_w, display_h;
		//glfwGetFramebufferSize(window, &display_w, &display_h);
		//glViewport(0, 0, display_w, display_h);
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


	//for (int k = 0; k < POI_Y; k++)
	//{
	//	glDeleteBuffers(1, &volumeVBO[k].pVBO);
	//	glDeleteBuffers(1, &volumeVBO[k].nVBO);
	//}
	glDeleteVertexArrays(POI_Y, volumeVAO.data());
	glDeleteBuffers(POI_Y, volumeVBO.data());
	glDeleteBuffers(POI_Y, volumeEBO.data());
	//delete[] volumeVAO;
	//delete[] volumeVBO;
	//delete[] volumeEBO;
	//delete[] points_count;
	//delete[] elements_count;
	//for(int i = 0;i<16;i++)
	//	glDeleteFramebuffers(1, &depthMapFBO[i]);

	glfwTerminate();
	return 0;
}

void renderUI(bool &show_demo_window,bool &show_another_window)
{

}

void RenderData(int x, int y, int z)
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