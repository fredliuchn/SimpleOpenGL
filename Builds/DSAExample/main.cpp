#include "Utils.h"

#include <GL/glew.h>
#include"soil2.h"
#include"GLFW/glfw3.h"

#include <windows.h>
#include <iostream>

using namespace std;
#define UseDSA
const unsigned int SCREEN_WIDTH = 720;
const unsigned int SCREEN_HEIGHT = 720;

GLuint shaderProgram, framebufferShaderProgram;
GLuint VAO, VBO, EBO, FBO;;

GLuint PictureTex, framebufferTex;

GLfloat vertices[] =
{
	-0.5f, -0.5f , 0.0f, 0.0f, 0.0f,
	-0.5f,  0.5f , 0.0f, 0.0f, 1.0f,
	 0.5f,  0.5f , 0.0f, 1.0f, 1.0f,
	 0.5f, -0.5f , 0.0f, 1.0f, 0.0f,
};

GLuint indices[] =
{
	0, 2, 1,
	0, 3, 2
};

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

void init(GLFWwindow* window)
{
	LPWSTR exeFullPath = new WCHAR[MAX_PATH];
	string strPath = "";

	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	strPath = WCharToMByte(exeFullPath);
	delete[]exeFullPath;
	exeFullPath = NULL;
	int pos = strPath.find_last_of('\\', strPath.length());
	string workpath = strPath.substr(0, pos);


	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	shaderProgram = Utils::createShaderProgram(workpath + "\\shader\\Simple.vs", workpath + "\\shader\\Simple.fs");
	framebufferShaderProgram = Utils::createShaderProgram(workpath + "\\shader\\framebuffer.vs", workpath + "\\shader\\framebuffer.fs");

#ifdef UseDSA
	PictureTex = Utils::loadTextureUseDSA(workpath + "\\..\\Resources\\Texture\\hamster.png");
#else
	PictureTex = Utils::loadTextureNoDSA(workpath + "\\..\\Resources\\Texture\\hamster.png");
#endif // UseDSA
	
#ifdef UseDSA
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);
#else
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
#endif // UseDSA

#ifdef UseDSA
	glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexArrayAttrib(VAO, 0);
	glVertexArrayAttribBinding(VAO, 0, 0);
	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

	glEnableVertexArrayAttrib(VAO, 1);
	glVertexArrayAttribBinding(VAO, 1, 0);
	glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
	glVertexArrayElementBuffer(VAO, EBO);
#else
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif // UseDSA

#ifdef UseDSA
	glCreateFramebuffers(1, &FBO);
#else
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
#endif // UseDSA

#ifdef UseDSA
	glCreateTextures(GL_TEXTURE_2D, 1, &framebufferTex);
#else
	glGenTextures(1, &framebufferTex);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, framebufferTex);
#endif // UseDSA

#ifdef UseDSA
	glTextureParameteri(framebufferTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(framebufferTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(framebufferTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(framebufferTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureStorage2D(framebufferTex, 1, GL_RGB8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glNamedFramebufferTexture(FBO, GL_COLOR_ATTACHMENT0, framebufferTex, 0);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTex, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
#endif // UseDSA
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << "\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void display(GLFWwindow* window, double currentTime)
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
#ifdef UseDSA
	GLfloat backgroundColor[] = { 19.0f / 255.0f, 34.0f / 255.0f, 44.0f / 255.0f, 1.0f };
	glClearNamedFramebufferfv(FBO, GL_COLOR, 0, backgroundColor);
#else
	glClearColor(19.0f / 255.0f, 34.0f / 255.0f, 44.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
#endif // UseDSA
	glUseProgram(shaderProgram);
#ifdef UseDSA
	glBindTextureUnit(0, PictureTex);
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
#else
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, PictureTex);
#endif // UseDSA
	glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(framebufferShaderProgram);
#ifdef UseDSA
	glBindTextureUnit(0, framebufferTex);
#else
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, framebufferTex);
#endif // UseDSA
	glUniform1i(glGetUniformLocation(framebufferShaderProgram, "screen"), 0);
	glBindVertexArray(VAO); // NO framebuffer VAO because I simply double the size of the rectangle to cover the whole screen
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
}

int main()
{
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "DSAExample", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create the GLFW window\n";
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	init(window);
	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}