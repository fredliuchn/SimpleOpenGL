#include "Utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <windows.h>
#include <iostream>

using namespace std;

#define numVAOs 1
#define numVBOs 10

//相机位置
glm::vec3 cameraPos = glm::vec3(3.0f, 6.0f, 17.0f);
//相机进退方向
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//相机升降方向
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//相机左右方向
glm::vec3 cameraAround = glm::vec3(1.0f, 0.0f, 0.0f);

GLuint renderingProgram1, renderingProgram2, RedProgram, SkyProgram, OffScreenProgram;

//抗锯齿
GLuint MSAAFramebuffer, MSAAIntermediateFBO;
//离屏渲染纹理
GLuint OffScreenTexture;

GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint skyboxTexture;
GLuint mvLoc, projLoc, nLoc, sLoc, vLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, rMat, invTrMat;

//光源位置
glm::vec3 currentLightPos = glm::vec3(-10.0f, 35.0f, 10.0f);

//白光
//全局环境光照
float globalAmbient[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
//环境光照
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
//漫反射光照
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
//镜面光照
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// gold material
float* gMatAmb = Utils::goldAmbient();
float* gMatDif = Utils::goldDiffuse();
float* gMatSpe = Utils::goldSpecular();
float gMatShi = Utils::goldShininess();

// bronze material
float* bMatAmb = Utils::bronzeAmbient();
float* bMatDif = Utils::bronzeDiffuse();
float* bMatSpe = Utils::bronzeSpecular();
float bMatShi = Utils::bronzeShininess();

GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mambLoc, mdiffLoc, mspecLoc, mshiLoc;

float thisAmb[4], thisDif[4], thisSpe[4], matAmb[4], matDif[4], matSpe[4];
float thisShi, matShi;

//阴影相关变量
GLuint shadowTex, shadowRenderbuffer, shadowFrameBuffer;
glm::mat4 lightVmatrix;
glm::mat4 lightPmatrix;
glm::mat4 shadowMVP1;
glm::mat4 shadowMVP2;
glm::mat4 b;

glm::vec3 origin(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);

string WCharToMByte(LPCWSTR lpcwszStr)
{
	string str;
	DWORD dwMinSize = 0;
	LPSTR lpszStr = NULL;
	dwMinSize = WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, NULL, 0, NULL, FALSE);
	if (0 == dwMinSize)
	{
		return "";
	}
	lpszStr = new char[dwMinSize];
	WideCharToMultiByte(CP_OEMCP, NULL, lpcwszStr, -1, lpszStr, dwMinSize, NULL, FALSE);
	str = lpszStr;
	delete[] lpszStr;
	return str;
}

void setupVertices(void) {

	float cubeVertexPositions[108] =
	{ -20.0f,  20.0f, -20.0f, -20.0f, -20.0f, -20.0f, 20.0f, -20.0f, -20.0f,
		20.0f, -20.0f, -20.0f, 20.0f,  20.0f, -20.0f, -20.0f,  20.0f, -20.0f,
		20.0f, -20.0f, -20.0f, 20.0f, -20.0f,  20.0f, 20.0f,  20.0f, -20.0f,
		20.0f, -20.0f,  20.0f, 20.0f,  20.0f,  20.0f, 20.0f,  20.0f, -20.0f,
		20.0f, -20.0f,  20.0f, -20.0f, -20.0f,  20.0f, 20.0f,  20.0f,  20.0f,
		-20.0f, -20.0f,  20.0f, -20.0f,  20.0f,  20.0f, 20.0f,  20.0f,  20.0f,
		-20.0f, -20.0f,  20.0f, -20.0f, -20.0f, -20.0f, -20.0f,  20.0f,  20.0f,
		-20.0f, -20.0f, -20.0f, -20.0f,  20.0f, -20.0f, -20.0f,  20.0f,  20.0f,
		-20.0f, -20.0f,  20.0f,  20.0f, -20.0f,  20.0f,  20.0f, -20.0f, -20.0f,
		20.0f, -20.0f, -20.0f, -20.0f, -20.0f, -20.0f, -20.0f, -20.0f,  20.0f,
		-20.0f,  20.0f, -20.0f, 20.0f,  20.0f, -20.0f, 20.0f,  20.0f,  20.0f,
		20.0f,  20.0f,  20.0f, -20.0f,  20.0f,  20.0f, -20.0f,  20.0f, -20.0f
	};
	float cubeTextureCoord[72] =
	{ 1.00f, 0.6666666f, 1.00f, 0.3333333f, 0.75f, 0.3333333f,	// back face lower right
		0.75f, 0.3333333f, 0.75f, 0.6666666f, 1.00f, 0.6666666f,	// back face upper left
		0.75f, 0.3333333f, 0.50f, 0.3333333f, 0.75f, 0.6666666f,	// right face lower right
		0.50f, 0.3333333f, 0.50f, 0.6666666f, 0.75f, 0.6666666f,	// right face upper left
		0.50f, 0.3333333f, 0.25f, 0.3333333f, 0.50f, 0.6666666f,	// front face lower right
		0.25f, 0.3333333f, 0.25f, 0.6666666f, 0.50f, 0.6666666f,	// front face upper left
		0.25f, 0.3333333f, 0.00f, 0.3333333f, 0.25f, 0.6666666f,	// left face lower right
		0.00f, 0.3333333f, 0.00f, 0.6666666f, 0.25f, 0.6666666f,	// left face upper left
		0.25f, 0.3333333f, 0.50f, 0.3333333f, 0.50f, 0.0000000f,	// bottom face upper right
		0.50f, 0.0000000f, 0.25f, 0.0000000f, 0.25f, 0.3333333f,	// bottom face lower left
		0.25f, 1.0000000f, 0.50f, 1.0000000f, 0.50f, 0.6666666f,	// top face upper right
		0.50f, 0.6666666f, 0.25f, 0.6666666f, 0.25f, 1.0000000f		// top face lower left
	};

	float bigvert[72] = {
		//后
		0.0,0.0,0.0,
		5.0,0.0,0.0,
		5.0,5.0,0.0,
		0.0,5.0,0.0,

		//前
		0.0,5.0,5.0,
		5.0,5.0,5.0,
		5.0,0.0,5.0,
		0.0,0.0,5.0,

		//左
		0.0,5.0,5.0,
		0.0,0.0,5.0,
		0.0,0.0,0.0,
		0.0,5.0,0.0,

		//右
		5.0,0.0,5.0,
		5.0,5.0,5.0,
		5.0,5.0,0.0,
		5.0,0.0,0.0,

		//下
		0.0,0.0,5.0,
		5.0,0.0,5.0,
		5.0,0.0,0.0,
		0.0,0.0,0.0,

		//上
		5.0,5.0,5.0,
		0.0,5.0,5.0,
		0.0,5.0,0.0,
		5.0,5.0,0.0
	};
	float smallvert[72] = {
		//后
		0.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,1.0,0.0,
		0.0,1.0,0.0,

		//前
		0.0,1.0,1.0,
		1.0,1.0,1.0,
		1.0,0.0,1.0,
		0.0,0.0,1.0,

		//左
		0.0,1.0,1.0,
		0.0,0.0,1.0,
		0.0,0.0,0.0,
		0.0,1.0,0.0,

		//右
		1.0,0.0,1.0,
		1.0,1.0,1.0,
		1.0,1.0,0.0,
		1.0,0.0,0.0,

		//下
		0.0,0.0,1.0,
		1.0,0.0,1.0,
		1.0,0.0,0.0,
		0.0,0.0,0.0,

		//上
		1.0,1.0,1.0,
		0.0,1.0,1.0,
		0.0,1.0,0.0,
		1.0,1.0,0.0
	};
	float colorvert[72] =
	{
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0,
		1.0,0.0,0.0
	};

	float quadVertices[] = {
	// positions   // texCoords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	};

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	float textureCoord[72] =
	{
		0.0,0.0,1.0,0.0,1.0,1.0,
		0.0,0.0,1.0,1.0,0.0,1.0,

		0.0,0.0,1.0,0.0,1.0,1.0,
		0.0,0.0,1.0,1.0,0.0,1.0,

		0.0,0.0,1.0,0.0,1.0,1.0,
		0.0,0.0,1.0,1.0,0.0,1.0,

		0.0,0.0,1.0,0.0,1.0,1.0,
		0.0,0.0,1.0,1.0,0.0,1.0,

		0.0,0.0,1.0,0.0,1.0,1.0,
		0.0,0.0,1.0,1.0,0.0,1.0,

		0.0,0.0,1.0,0.0,1.0,1.0,
		0.0,0.0,1.0,1.0,0.0,1.0,
	};

	float cudenormal[72] =
	{
		0.0, 0.0,-1.0,
		0.0, 0.0,-1.0,
		0.0, 0.0,-1.0,
		0.0, 0.0,-1.0,

		0.0, 0.0,1.0,
		0.0, 0.0,1.0,
		0.0, 0.0,1.0,
		0.0, 0.0,1.0,

		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,

		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,

		0.0,-1.0, 0.0,
		0.0,-1.0, 0.0,
		0.0,-1.0, 0.0,
		0.0,-1.0, 0.0,

		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0
	};
	int index[36] = {
		0,3,2,
		0,2,1,

		4,7,6,
		4,6,5,

		8,11,10,
		8,10,9,

		12,15,14,
		12,14,13,

		16,19,18,
		16,18,17,

		20,23,22,
		20,22,21,
	};

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bigvert), bigvert, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorvert), colorvert, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smallvert), smallvert, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cudenormal), cudenormal, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexPositions), cubeVertexPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTextureCoord), cubeTextureCoord, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
}

void installLights(GLuint program, glm::mat4 vMatrix)
{
	glm::vec3 transformed = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	float lightPos[3];
	lightPos[0] = transformed.x;
	lightPos[1] = transformed.y;
	lightPos[2] = transformed.z;

	matAmb[0] = thisAmb[0]; matAmb[1] = thisAmb[1]; matAmb[2] = thisAmb[2]; matAmb[3] = thisAmb[3];
	matDif[0] = thisDif[0]; matDif[1] = thisDif[1]; matDif[2] = thisDif[2]; matDif[3] = thisDif[3];
	matSpe[0] = thisSpe[0]; matSpe[1] = thisSpe[1]; matSpe[2] = thisSpe[2]; matSpe[3] = thisSpe[3];
	matShi = thisShi;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(program, "globalAmbient");
	ambLoc = glGetUniformLocation(program, "light.ambient");
	diffLoc = glGetUniformLocation(program, "light.diffuse");
	specLoc = glGetUniformLocation(program, "light.specular");
	posLoc = glGetUniformLocation(program, "light.position");
	mambLoc = glGetUniformLocation(program, "material.ambient");
	mdiffLoc = glGetUniformLocation(program, "material.diffuse");
	mspecLoc = glGetUniformLocation(program, "material.specular");
	mshiLoc = glGetUniformLocation(program, "material.shininess");

	//  set the uniform light and material values in the shader
	glProgramUniform4fv(program, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(program, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(program, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(program, specLoc, 1, lightSpecular);
	glProgramUniform3fv(program, posLoc, 1, lightPos);
	glProgramUniform4fv(program, mambLoc, 1, matAmb);
	glProgramUniform4fv(program, mdiffLoc, 1, matDif);
	glProgramUniform4fv(program, mspecLoc, 1, matSpe);
	glProgramUniform1f(program, mshiLoc, matShi);
}

void setupMSAA(GLFWwindow* window)
{
	glGenFramebuffers(1, &MSAAFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, MSAAFramebuffer);
	
	GLuint textureColorBufferMultiSampled;
	glGenTextures(1, &textureColorBufferMultiSampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
	
	GLuint MSAARenderbuffer;
	glGenRenderbuffers(1, &MSAARenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, MSAARenderbuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, MSAARenderbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &MSAAIntermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, MSAAIntermediateFBO);

	glGenTextures(1, &OffScreenTexture);
	glBindTexture(GL_TEXTURE_2D, OffScreenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, OffScreenTexture, 0);	// we only need a color buffer

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void setupShadowBuffers(GLFWwindow* window) 
{
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
	
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowTex, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void init(GLFWwindow* window) {

	LPWSTR exeFullPath = new WCHAR[MAX_PATH];
	string strPath = "";

	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	strPath = WCharToMByte(exeFullPath);
	delete[]exeFullPath;
	exeFullPath = NULL;
	int pos = strPath.find_last_of('\\', strPath.length());
	string workpath = strPath.substr(0, pos);

	SkyProgram = Utils::createShaderProgram(workpath + "\\shader\\SkyBox.vs", workpath + "\\shader\\SkyBox.fs");
	RedProgram = Utils::createShaderProgram(workpath + "\\shader\\RedColor.vs", workpath + "\\shader\\RedColor.fs");

	renderingProgram1 = Utils::createShaderProgram(workpath + "\\shader\\shadow1.vs", workpath + "\\shader\\shadow1.fs");
	renderingProgram2 = Utils::createShaderProgram(workpath + "\\shader\\shadow2.vs", workpath + "\\shader\\shadow2.fs");

	OffScreenProgram = Utils::createShaderProgram(workpath + "\\shader\\offscreen.vs", workpath + "\\shader\\offscreen.fs");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	skyboxTexture = Utils::loadCubeMap(workpath+"\\..\\Resources\\skybox\\");

	setupVertices();
	setupShadowBuffers(window);
	setupMSAA(window);

	b = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f);
}
void passone()
{
	glUseProgram(renderingProgram1);
	//小的
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(6.0, 0.0, 0.0));

	//mMat = glm::translate(glm::mat4(1.0f), glm::vec3(2.0, 0.0, 0.0));
	//mMat = glm::scale(mMat, glm::vec3(0.5, 0.5, 0.5));

	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	sLoc = glGetUniformLocation(renderingProgram1, "shadowMVP");
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));
	//顶点
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//大的
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
	//mMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));

	shadowMVP1 = lightPmatrix * lightVmatrix * mMat;
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

	//顶点
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void passtwo()
{
	glUseProgram(renderingProgram2);

	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");
	sLoc = glGetUniformLocation(renderingProgram2, "shadowMVP");

	//大的

	//对大正方形写入模板缓冲区
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilMask(0xFF);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	thisAmb[0] = gMatAmb[0]; thisAmb[1] = gMatAmb[1]; thisAmb[2] = gMatAmb[2];  // gold
	thisDif[0] = gMatDif[0]; thisDif[1] = gMatDif[1]; thisDif[2] = gMatDif[2];
	thisSpe[0] = gMatSpe[0]; thisSpe[1] = gMatSpe[1]; thisSpe[2] = gMatSpe[2];
	thisShi = gMatShi;

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
	//mMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.2, 0.2, 0.2));

	installLights(renderingProgram2, vMat);


	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));
	shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

	//顶点
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//颜色
	//glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(1);

	//法向量
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//利用模板缓冲区对大正方形进行描边
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(RedProgram);
	float fscale = 1.1;

	mvLoc = glGetUniformLocation(RedProgram, "mv_matrix");
	projLoc = glGetUniformLocation(RedProgram, "proj_matrix");

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
	mMat = glm::scale(mMat, glm::vec3(fscale, fscale, fscale));
	mMat = glm::translate(mMat, glm::vec3(-0.05, -0.05, -0.05));
	mvMat = vMat * mMat;
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glEnable(GL_DEPTH_TEST);
	//glStencilMask(0xFF);// 再次允许写入模板缓冲区 以便下次迭代时清除
	//小的
	glUseProgram(renderingProgram2);

	mvLoc = glGetUniformLocation(renderingProgram2, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram2, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram2, "norm_matrix");
	sLoc = glGetUniformLocation(renderingProgram2, "shadowMVP");

	//小正方形

	thisAmb[0] = bMatAmb[0]; thisAmb[1] = bMatAmb[1]; thisAmb[2] = bMatAmb[2];  // bronze
	thisDif[0] = bMatDif[0]; thisDif[1] = bMatDif[1]; thisDif[2] = bMatDif[2];
	thisSpe[0] = bMatSpe[0]; thisSpe[1] = bMatSpe[1]; thisSpe[2] = bMatSpe[2];
	thisShi = bMatShi;

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(6.0, 0.0, 0.0));
	//mMat = glm::translate(glm::mat4(1.0f), glm::vec3(2.0, 0.0, 0.0));
	//mMat = glm::scale(mMat, glm::vec3(0.5, 0.5, 0.5));
	installLights(renderingProgram2, vMat);

	mvMat = vMat * mMat;
	invTrMat = glm::transpose(glm::inverse(mvMat));
	shadowMVP2 = b * lightPmatrix * lightVmatrix * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
	glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

	//顶点
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//颜色
	/*glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);*/

	//法向量
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LESS);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void display(GLFWwindow* window, double currentTime)
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
	vMat = glm::translate(glm::mat4(1.0f), -cameraPos);

	//从光源视角初始化视觉矩阵以及透视矩阵，以便在第一轮使用
	lightVmatrix = glm::lookAt(currentLightPos, origin, up);
	lightPmatrix = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
	//使用自定义帧缓冲区，将阴影纹理附着到其上
	
	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0f, 4.0f);
	passone();
	glDisable(GL_POLYGON_OFFSET_FILL);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	//使用显示缓冲区，并重新开始绘制
	glBindFramebuffer(GL_FRAMEBUFFER, MSAAFramebuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//显示天空盒
	glUseProgram(SkyProgram);
	vLoc = glGetUniformLocation(SkyProgram, "v_matrix");
	projLoc = glGetUniformLocation(SkyProgram, "proj_matrix");

	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glDepthFunc(GL_LEQUAL);
	//glDepthMask(GL_FALSE);
	//glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthFunc(GL_LESS);
	//glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	//重新开启绘制颜色
	glStencilMask(0xFF);
	passtwo();

 
	//将离屏渲染的帧保存到MSAAIntermediateFBO
	glBindFramebuffer(GL_READ_FRAMEBUFFER, MSAAFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, MSAAIntermediateFBO);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//正常渲染
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	
	glUseProgram(OffScreenProgram);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[8]);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, OffScreenTexture);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDrawBuffer(GL_FRONT);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	Utils::checkOpenGLError();
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

void processInput(GLFWwindow *window)
{
	float cameraSpeed = 0.01f; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraAround)) * cameraSpeed;
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraAround)) * cameraSpeed;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cameraPos.z += yoffset;
}

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	(void)source;
	(void)id;
	(void)length;
	(void)userParam;

	fprintf(
		stderr,
		"GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message
	);
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(600, 600, "TwoBoxes", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	init(window);

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, nullptr);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteRenderbuffers(1, &shadowRenderbuffer);
	glDeleteFramebuffers(1, &shadowFrameBuffer);
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}