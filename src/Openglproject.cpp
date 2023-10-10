using namespace std;

/// Openglproject.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������
//
//#include<GL/glew.h>
#include"Shader.h"//��Ϊ������glad�⣬��Ҫ�ö�
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

//render����
//void renderSphereMesh(const Shader &shader);
//void renderSphereCamera(const Shader& shader);
//void renderSpherePoints(const Shader& shader);
void renderQuad();
void renderUI(bool& show_demo_window, bool& show_another_window);
void RenderData(int x,int y,int z);


int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

const unsigned int VISI_WIDTH = 2500;
const unsigned int VISI_HEIGHT = 50;

int POI_X = 16;
int POI_Y = 16;
int POI_Z = 16;

const unsigned int POI_WIDTH = 50;
const unsigned int POI_HEIGHT = 50;

float fov = 60.0f;
float yaw = 90.0f;//����ƫ����
float pitch = 0.0f;//���ø�����
bool firstmouse = true;//�ж�����Ƿ���ν��뻭��
double lastX = (float)SCR_WIDTH / 2;//������һ֡���λ�ã�Ĭ��Ϊƽ������
double lastY = (float)SCR_HEIGHT / 2;

//ʱ���������������ÿһ֡��λ�ñ仯����
float deltatime = 0.0f;
float lasttime = 0.0f;

glm::vec3 cameraposition = glm::vec3(0.0f, 0.0f, -10.0f);//�����λ��
glm::vec3 lightposition = glm::vec3(5.0, 5.0, 5.0);
Camera camera(cameraposition);

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

SphereMesh sphereM(RecentFile::GetRecentSphere());
CameraMesh cameraM(RecentFile::GetRecentLight());

void LoadSphereAndLight(SphereMesh& sphereM, CameraMesh& cameraM, Shader& depthShader, Shader& quardShader)
{

	//��ȡ�ļ�
	//SphereMesh sphereM(Sphere);
	//CameraMesh cameraM(Light);
	//��Ļ�����Լ�����
	std::vector<glm::vec3> vert = sphereM.vertices;
	std::vector<glm::uvec3> index = sphereM.index;
	//��Դ���������
	std::vector<cameraVertex> cameraVert = cameraM.cameraVer;

	//��ǰ��������Դ������ı任����
	vector<glm::mat4> worldtoview = cameraM.CalWorld2View();


	//��ǰ��������Դ�������Frustum
	vector<glm::mat4> allfrustum = cameraM.CalFrustum();

	quardShader.use();
	glUniformMatrix4fv(glGetUniformLocation(quardShader.ID, "view"), cameraVert.size(), GL_FALSE, &worldtoview[0][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(quardShader.ID, "proj"), cameraVert.size(), GL_FALSE, &allfrustum[0][0][0]);
	depthShader.use();
	glUniformMatrix4fv(glGetUniformLocation(depthShader.ID, "view"), cameraVert.size(), GL_FALSE, &worldtoview[0][0][0]);
	glUniformMatrix4fv(glGetUniformLocation(depthShader.ID, "proj"), cameraVert.size(), GL_FALSE, &allfrustum[0][0][0]);

	sphereM.Bind();
	cameraM.Bind();

	/*---------------------------�����ͼ------------------------------*/
	cameraM.CreateDepthMap(SCR_WIDTH,SCR_HEIGHT);

	glm::mat4 model = glm::mat4(1.0f);
	glEnable(GL_DEPTH_TEST);
	/*----------------����Ļ�������ͼ����֡������-----------------*/
	depthShader.use();
	depthShader.setMat4("model", model);
	depthShader.setVec3("lightPos", glm::vec3(5.0, 5.0, 5.0));//����һ���diffuse����
	depthShader.setInt("free", 2);
	for (int i = 0; i < cameraVert.size(); i++)
	{
		depthShader.setInt("count", i);
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
	//glfw�ĳ�ʼ���Լ�ָ���汾��
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//��������
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Missing Points", NULL, NULL);

	//����������
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	//����imgui������
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	//����imgui���
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	//����ƽ̨����Ⱦ�����
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	//imgui����
	io.Fonts->AddFontFromFileTTF("imgui/YaHei.ttf", 16.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	io.Fonts->Build();


	

	//imgui״̬
	RecentFile::imguiSetting ims;


	bool show_demo_window = true;
	bool show_another_window = false;
	bool linemode_Sphere = true;
	bool linemode_Main = false;
	bool render_Sphere = true;
	bool render_Main = true;
	bool render_Camera = true;
	bool firstRender = true;
	int render_count = 0;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImVec4 main_color = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
	float points_side_length = 5.5;
	bool Calspherecamera = true;
	bool render_Attach = true;
	bool linemode_Attach = false;
	bool light_Main = true;
	bool write_txt = false;
	bool lightfollowcamera = true;

	SphereMesh attach(RecentFile::GetRecentAttach(render_Attach));

	std::string sphere_file(sphereM.filepath);
	std::string camera_file(cameraM.filepath);
	std::string attach_file(attach.filepath);

	float radius_cut = 5.5;
	float up_cut = 5.0;
	float down_cut = -5.0;
	float left_cut = -5.0;
	float right_cut = 5.0;
	float front_cut = -5.0;
	float back_cut = 5.0;

	float light_x = 30;
	float light_y = 0;
	float light_dep = 10;

	float light_rel_x = 30;
	float light_rel_y = 30;

	float ambientStrength = 0.3;
	float diffStrength = 0.7;

	//���ڴ��ļ��Ի�����ת��C++����Ŀ¼������ǰ���浱ǰ����Ŀ¼�µ�json�ļ�
	//RecentFile::init();

	//ע��ص�����
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	//glfwSetMouseButtonCallback(window, mouse_button_callback);

	//���ò�׽���
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//��ʼ��glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	//�������ұ���Shader����
	Shader sphereShader("./shader/SphereMesh.vert", "./shader/SphereMesh.frag");
	Shader cameraShader("./shader/CameraMesh.vert", "./shader/CameraMesh.frag");
	Shader pointsShader("./shader/PointsMesh.vert", "./shader/PointsMesh.frag");
	Shader depthShader("./shader/DepthShader.vert", "./shader/DepthShader.frag");
	Shader QuardShader("./shader/Quard.vert", "./shader/Quard.frag");
	
	//SphereMesh sphereM("mesh.txt");
	//CameraMesh cameraM("light.txt");


	attach.Bind();

	LoadSphereAndLight(sphereM, cameraM, depthShader, QuardShader);

	
	glm::mat4 model = glm::mat4(1.0f);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//׼������
	//unsigned int* volumeVAO;
	vector<unsigned int> volumeVAO;
	//vector<VBOList> volumeVBO;
	vector<unsigned int> volumeVBO;
	vector<unsigned int> volumeEBO;

	vector<unsigned int> visiMapFBO;
	vector<unsigned int> visiMap;

	vector<unsigned int> points_count;
	vector<unsigned int> elements_count;

	int win_wid;
	int win_hei;
	while (!glfwWindowShouldClose(window))
	{
		//ʱ������
		float currenttime = glfwGetTime();
		deltatime = currenttime - lasttime;
		lasttime = currenttime;

		processInput(window);
		//processMouseButton(window, viewMatCount, mouseButtonChanged);
		processMousePress(window);


		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
		//cout << win_wid << " " << win_hei << endl;
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);


		if (firstRender)
		{
			std::cout << "���ڼ���......" << std::endl;
			double begin = clock();
			//volumeVAO = new unsigned int[POI_Y];
			volumeVAO.clear();
			volumeVBO.clear();
			volumeEBO.clear();
			volumeVAO.assign(POI_Y,0);
			volumeVBO.assign(POI_Y, 0);
			//volumeVBO.assign(POI_Y, {0,0});
			volumeEBO.assign(POI_Y,0);
			visiMapFBO.clear();
			visiMapFBO.assign(POI_Y, 0);
			visiMap.clear();
			visiMap.assign(POI_Y, 0);


			points_count.clear();
			points_count.assign(POI_Y,0);
			elements_count.clear();
			elements_count.assign(POI_Y, 0);


			/*---------------------------�������ڱ�����Ϣ����ͼ------------------------------*/


			//unsigned int* visiMapFBO = new unsigned int[POI_Y];
			//vector<unsigned int> visiMapFBO(POI_Y,0);
			//glDeleteFramebuffers(POI_Y, visiMapFBO.data());			
			glGenFramebuffers(POI_Y, visiMapFBO.data());

			//unsigned int* visiMap = new unsigned int[POI_Y];//������ͼ
			//vector<unsigned int> visiMap(POI_Y,0);
			//glDeleteTextures(POI_Y, visiMap.data());
			glGenTextures(POI_Y, visiMap.data());

			for (int vmpC = 0; vmpC < POI_Y; vmpC++)
			{
				glBindTexture(GL_TEXTURE_2D, visiMap[vmpC]);
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, POI_X, POI_Z, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindFramebuffer(GL_FRAMEBUFFER, visiMapFBO[vmpC]);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, visiMap[vmpC], 0);
			}

			//glDrawBuffer(GL_NONE);
			//glReadBuffer(GL_NONE);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				cout << "Framebuffer not complete! " << endl;

			glBindFramebuffer(GL_FRAMEBUFFER, 0);


			//������Ȳ��ԣ�glEnable��glDisable�������û��߹ر�opengl��ĳ������




			//������Ⱦ��1.��Ⱦ16�������ͼ  2.��ȾPOI_Y��ɸѡ���ķֲ���ͼ


			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			/*------------��ȡPOI_X * POI_Z����Ŀ����������������POI_Y��---------*/
			glViewport(0, 0, POI_X, POI_Z);

			glClear(GL_COLOR_BUFFER_BIT);

			QuardShader.use();
			//QuardShader.setInt("count", viewMatCount);//�������������
			QuardShader.setMat4("model", model);
			QuardShader.setInt("row", POI_X);
			QuardShader.setInt("col", POI_Z);
			QuardShader.setInt("hei", POI_Y);
			QuardShader.setFloat("radius", sphereM.Radius);
			QuardShader.setFloat("sidelength", points_side_length);
			QuardShader.setInt("sizeOfDep", cameraM.cameraVer.size());

			QuardShader.setFloat("upCut", sphereM.up_cut);
			QuardShader.setFloat("downCut", sphereM.down_cut);
			QuardShader.setFloat("rightCut", sphereM.right_cut);
			QuardShader.setFloat("leftCut", sphereM.left_cut);
			QuardShader.setFloat("frontCut", sphereM.front_cut);
			QuardShader.setFloat("backCut", sphereM.back_cut);
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

			//�����Ⱦ
			for (int layer = 0; layer < POI_Y; layer++)
			{
				QuardShader.setInt("layer", layer);
				
				glBindFramebuffer(GL_FRAMEBUFFER, visiMapFBO[layer]);
				glClear(GL_COLOR_BUFFER_BIT);
				renderQuad();
				glBindFramebuffer(GL_FRAMEBUFFER, 0);				
			}
			glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
			/*--------------���ģ��/�ڿ��㷨/�����ṹ������/������������---------------*/
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
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//��Position
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(volumeNormal.size() * sizeof(glm::vec3)));				
				glEnableVertexAttribArray(1);
				glBindBuffer(0, volumeVBO[layer]);

				//glBindBuffer(GL_ARRAY_BUFFER, volumeVBO[layer]);
				//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * volumePoints.size(), volumePoints.data(), GL_STATIC_DRAW);
				//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);//��normal
				//glEnableVertexAttribArray(1);
				//glBindBuffer(0, volumeVBO[layer]);
				


				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, volumeEBO[layer]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, volumeIndex.size() * 3 * sizeof(unsigned int), volumeIndex.data(), GL_STATIC_DRAW);

				glBindVertexArray(0);

				if(write_txt)
					savePixelsToTxt(pixels, "matchPoints.txt", layer);
			}
			firstRender = false;
			glDeleteFramebuffers(POI_Y, visiMapFBO.data());
			glDeleteTextures(POI_Y, visiMap.data());
			//delete[] visiMap;
			//delete[] visiMapFBO;
			double end = clock();
			double duration = end - begin;
			std::cout << "Complete! Time takes "<<duration<<" ms" << endl;
		}
		


		/*----------���ù�Դ��ʾ��ʽ----------*/
		if (lightfollowcamera)
		{
			lightposition = camera.CalLightPos(camera.Position,light_rel_x,light_rel_y);
		}
		else
		{
			//camera.updateLightPos(lightposition,light_x,light_y);
			lightposition = camera.CalLightPos(glm::vec3(0,0,-light_dep),light_x,light_y);
		}

		/*----------------����Ļ-----------------*/
		/*glStencilMask������������һ��λ����(Bitmask)�������뽫Ҫд�뻺���ģ��ֵ������(AND)���㡣
		Ĭ����������õ�λ��������λ��Ϊ1����Ӱ�����*/
		if(linemode_Sphere)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		sphereShader.use();
		glm::mat4 view = glm::mat4(1.0);
		view = camera.GetviewMatrix();
		glm::mat4 projection = glm::mat4(1.0);
		glm::mat4 model = glm::mat4(1.0);
		projection = glm::perspective(glm::radians(camera.Fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		sphereShader.setMat4("camera_view", view);//����lookat����);
		sphereShader.setMat4("camera_proj", projection);
		sphereShader.setInt("free", 1);
		//ģ�;��󣬿����������ת
		sphereShader.setMat4("model", model);
		sphereShader.setVec3("lightPos", lightposition);

		glBindVertexArray(sphereM.VAO);
		if(render_Sphere)
			glDrawElements(GL_TRIANGLES, sphereM.vertices.size() * sizeof(float) * 3, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		/*----------------�������-----------------*/

		cameraShader.use();


		cameraShader.setMat4("view", view);
		cameraShader.setMat4("proj", projection);
		//ģ�;��󣬿����������ת
		//glm::mat4 model_c = glm::mat4(1.0f);
		cameraShader.setMat4("model", model);

		if (render_Camera)
		{
			glBindVertexArray(cameraM.VAO);
			glPointSize(20.0f);
			glDrawArrays(GL_POINTS, 0, cameraM.cameraVer.size());
			glBindVertexArray(0);
		}

		/*----------------��������-----------------*/
		if (linemode_Main)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glEnable(GL_CULL_FACE);
		pointsShader.use();

		pointsShader.setBool("light", light_Main);
		pointsShader.setMat4("view", view);
		pointsShader.setMat4("proj", projection);
		pointsShader.setMat4("model", model);
		pointsShader.setVec4("main_color", glm::vec4(main_color.x,main_color.y,main_color.z,main_color.w));
		pointsShader.setVec3("lightPos", lightposition);

		pointsShader.setFloat("upCut", up_cut);
		pointsShader.setFloat("downCut", down_cut);
		pointsShader.setFloat("rightCut", right_cut);
		pointsShader.setFloat("leftCut", left_cut);
		pointsShader.setFloat("frontCut", front_cut);
		pointsShader.setFloat("backCut", back_cut);
		pointsShader.setFloat("radiusCut", radius_cut);

		pointsShader.setFloat("ambientStrength", ambientStrength);
		pointsShader.setFloat("diffStrength", diffStrength);

		if (render_Main)
		{
			for (int i = 0; i < POI_Y; i++)
			{
				glBindVertexArray(volumeVAO[i]);
				//glPointSize(2.0f);
				//glDrawArrays(GL_POINTS, 0, points_count[i] * 8);
				glDrawElements(GL_TRIANGLES, elements_count[i] * 36, GL_UNSIGNED_INT, nullptr);
				glBindVertexArray(0);
			}
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//
		glDisable(GL_CULL_FACE);

		/*----------------����������-----------------*/
		if(linemode_Attach)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		sphereShader.use();
			
		glBindVertexArray(attach.VAO);
		if (render_Attach)
		{
			glDrawElements(GL_TRIANGLES, attach.vertices.size() * sizeof(float) * 3, GL_UNSIGNED_INT, nullptr);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindVertexArray(0);
			


		/*-----------��ʾһ��POI_X * POI_Z������-------------*/

		//glViewport(0, 0,500, 500);
	 //   depthShader.use();
		////QuardShader.setInt("floors",);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, visiMap[viewMatCount]);
		//renderQuad();

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		/*-----------Imgui����-------------*/
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
			//std::locale::global(std::locale("en_US.UTF-8"));

			ImGui::Begin((const char*)u8"����");                          // Create a window called "Hello, world!" and append into it.
			ImGui::Text((const char*)u8"��ǰ��Ļ����뾶��%.3f",sphereM.Radius);
			ImGui::Text((const char*)u8"ƽ��֡�� %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			if (ImGui::TreeNode((const char*)u8"�ļ�ѡ��"))
			{
				if (ImGui::Button((const char*)u8"ѡ����Ļ�ļ�"))
				{
					std::string temp = FileDlg::GetFileDialog();
					if (temp != "")
					{
						sphere_file = temp;
					}
				}
				ImGui::Text(Utility::string_To_UTF8(sphere_file).c_str());
				if (ImGui::Button((const char*)u8"ѡ��������ļ�"))
				{
					std::string temp = FileDlg::GetFileDialog();
					if (temp != "")
					{
						camera_file = temp;
					}
				}
				ImGui::Text(Utility::string_To_UTF8(camera_file).c_str());
				if (ImGui::Button((const char*)u8"����"))
				{

					//��һ���ǳ��ؼ�����Ҫɾ��֮ǰ��FBO
					glDeleteFramebuffers(32, cameraM.depthFBO.data());
					glDeleteTextures(32, cameraM.depthMap.data());
					/*glDeleteFramebuffers(cameraM.cameraVer.size(), cameraM.depthFBO.data());
					glDeleteTextures(cameraM.cameraVer.size(), cameraM.depthMap.data());*/
					glDeleteVertexArrays(1, &cameraM.VAO);
					glDeleteBuffers(1, &cameraM.VBO);
					glDeleteVertexArrays(1, &sphereM.VAO);
					glDeleteBuffers(1, &sphereM.VBO);
					glDeleteBuffers(1, &sphereM.EBO);
					glDeleteVertexArrays(POI_Y, volumeVAO.data());
					glDeleteBuffers(POI_Y, volumeVBO.data());
					glDeleteBuffers(POI_Y, volumeEBO.data());
					/*glDeleteFramebuffers(POI_Y, visiMapFBO.data());
					glDeleteTextures(POI_Y, visiMap.data());*/

					sphereM.ReBuid(sphere_file);
					cameraM.ReBuid(camera_file);
					LoadSphereAndLight(sphereM, cameraM, depthShader, QuardShader);
					firstRender = true;
					//size = 16;
					//POI_X = 16;
					//POI_Y = 16;
					//POI_Z = 16;				
					RecentFile::refreshRecent(sphere_file, camera_file);
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode((const char*)u8"��ʾ����"))
			{
				ImGui::Checkbox((const char*)u8"��Ļ��ʾ", &render_Sphere);
				ImGui::SameLine();
				//ImGui::Text((const char*)u8"�Ƿ������߿�ģʽ");               // Display some text (you can use a format strings too)
				ImGui::Checkbox((const char*)u8"��Ļ�߿�ģʽ", &linemode_Sphere);      // Edit bools storing our window open/close state

				ImGui::Checkbox((const char*)u8"������ʾ", &render_Main);
				ImGui::SameLine();
				ImGui::Checkbox((const char*)u8"�����߿�ģʽ", &linemode_Main);

				ImGui::Checkbox((const char*)u8"�������ʾ", &render_Camera);

				//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

				ImGui::Checkbox((const char*)u8"�������ù���", &light_Main);
				ImGui::Checkbox((const char*)u8"���ù�Դ�����ӽ�", &lightfollowcamera);
				if (!lightfollowcamera)
				{
					ImGui::SliderFloat((const char*)u8"��Դ�Ƕ�X", &light_x, 0, 359.0);
					ImGui::SliderFloat((const char*)u8"��Դ�Ƕ�Y", &light_y, 0, 359.0);
					ImGui::SliderFloat((const char*)u8"��Դ����", &light_dep, 0, 50);
				}
				else if(lightfollowcamera)
				{
					ImGui::SliderFloat((const char*)u8"��Դ��ԽǶ�X", &light_rel_x, 0, 359.0);
					ImGui::SliderFloat((const char*)u8"��Դ��ԽǶ�Y", &light_rel_y, 0, 359.0);
				}
				
				//ImGui::SliderFloat((const char*)u8"��Դ���������Ƕ�Y", &light_y, -90.0, 90.0);
				//ImGui::Text((const char*)u8"�Ƕ�x��", light_x);
				ImGui::SliderFloat((const char*)u8"������ϵ��", &ambientStrength, 0.1, 1.0);
				ImGui::SliderFloat((const char*)u8"������ϵ��", &diffStrength, 0.1, 1.0);
				ImGui::ColorEdit3((const char*)u8"������ɫ", (float*)&main_color); // Edit 3 floats representing a color

				ImGui::TreePop();
			}
			if (ImGui::TreeNode((const char*)u8"��������"))
			{
				if (ImGui::Button((const char*)u8"ѡ�񸽼������ļ�"))
				{
					std::string temp = FileDlg::GetFileDialog();
					if (temp != "")
					{
						attach_file = temp;
					}
					glDeleteVertexArrays(1, &attach.VAO);
					glDeleteBuffers(1, &attach.VBO);
					glDeleteBuffers(1, &attach.EBO);
					attach.ReBuid(attach_file);
					attach.Bind();
					if (!attach_file.empty())
						render_Attach = true;
					RecentFile::refreshAttach(attach_file, render_Attach);
				}
				ImGui::Text(Utility::string_To_UTF8(attach_file).c_str());
				if (render_Attach || attach_file !="")
				{
					if (ImGui::Button((const char*)u8"ɾ�����������ļ�"))
					{
						attach_file = "";
						render_Attach = false;
						RecentFile::refreshAttach(attach_file, render_Attach);
					}
					ImGui::Checkbox((const char*)u8"��ʾ��������", &render_Attach);
					ImGui::SameLine();
					ImGui::Checkbox((const char*)u8"���������߿�ģʽ", &linemode_Attach);
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode((const char*)u8"��������"))
			{
				ImGui::SliderFloat((const char*)u8"�趨������Ʊ߳�", &points_side_length, ims.points_side_length_low, ims.points_side_length_up);
				ImGui::SliderInt((const char*)u8"�趨��������ܶ�", &size, ims.points_density_low, ims.points_density_up);
				ImGui::Checkbox((const char*)u8"���������(matchPoints.txt)", & write_txt);
				if (ImGui::Button((const char*)u8"���ü���"))                        // Buttons return true when clicked (most widgets return true when edited/activated)
				{
					POI_X = size;
					POI_Y = size;
					POI_Z = size;
					firstRender = true;
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode((const char*)u8"�޳�����"))
			{
				ImGui::SliderFloat((const char*)u8"�����޳��뾶", &radius_cut, ims.radius_cut_low, ims.radius_cut_up);


				ImGui::SliderFloat((const char*)u8"���޳���", &up_cut, ims.up_cut_low, ims.up_cut_up);
				ImGui::SliderFloat((const char*)u8"���޳���", &down_cut, ims.down_cut_low, ims.down_cut_up);
				ImGui::SliderFloat((const char*)u8"���޳���", &right_cut, ims.right_cut_low, ims.right_cut_up);
				ImGui::SliderFloat((const char*)u8"���޳���", &left_cut, ims.left_cut_low, ims.left_cut_up);
				ImGui::SliderFloat((const char*)u8"���޳���", &back_cut, ims.back_cut_low, ims.back_cut_up);
				ImGui::SliderFloat((const char*)u8"ǰ�޳���", &front_cut, ims.front_cut_low, ims.front_cut_up);

				if (ImGui::Button((const char*)u8"Ӧ���޳����ý��м���"))
				{
					sphereM.Radius = radius_cut;
					sphereM.up_cut = up_cut;
					sphereM.down_cut = down_cut;
					sphereM.right_cut = right_cut;
					sphereM.left_cut = left_cut;
					sphereM.back_cut = back_cut;
					sphereM.front_cut = front_cut;

					firstRender = true;
				}
				ImGui::TreePop();
			}

			
			ImGui::End();
		}


		ImGui::Render();
		int display_w, display_h;
		//glfwGetFramebufferSize(window, &display_w, &display_h);
		//glViewport(0, 0, display_w, display_h);
		//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(window);

	}

	
	//���PNG��ʽͼ��
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



void mouse_callback(GLFWwindow*window, double xpos, double ypos)//�������ĵ��ã����ڿ����ӽ�
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
		//camera.updateLightPos(lightposition, light_x, light_y);
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