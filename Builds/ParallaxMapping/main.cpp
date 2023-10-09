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
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
//相机进退方向
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//相机升降方向
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//相机左右方向
glm::vec3 cameraAround = glm::vec3(1.0f, 0.0f, 0.0f);

GLuint renderingProgram;

GLuint bricksTex, normalMap, heightMap;

float heightScale = 0.01f;

GLuint vao[numVAOs];
GLuint vbo[numVBOs];

int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat;

//光源位置
glm::vec3 currentLightPos = glm::vec3(0.5f, 1.0f, 0.3f);

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

	// positions
	glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
	glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
	glm::vec3 pos3(1.0f, -1.0f, 0.0f);
	glm::vec3 pos4(1.0f, 1.0f, 0.0f);
	// texture coordinates
	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 uv4(1.0f, 1.0f);
	// normal vector
	glm::vec3 nm(0.0f, 0.0f, 1.0f);

	// calculate tangent/bitangent vectors of both triangles
	glm::vec3 tangent1, bitangent1;
	glm::vec3 tangent2, bitangent2;
	// triangle 1
	// ----------
	glm::vec3 edge1 = pos2 - pos1;
	glm::vec3 edge2 = pos3 - pos1;
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent1 = glm::normalize(tangent1);

	bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent1 = glm::normalize(bitangent1);

	// triangle 2
	// ----------
	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent2 = glm::normalize(tangent2);


	bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent2 = glm::normalize(bitangent2);


	float quadVertices[] = {
		// positions             // texcoords   // normal         // tangent                          // bitangent
		pos1.x, pos1.y, pos1.z,  uv1.x, uv1.y,  nm.x, nm.y, nm.z, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos2.x, pos2.y, pos2.z,  uv2.x, uv2.y,  nm.x, nm.y, nm.z, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
		pos3.x, pos3.y, pos3.z,  uv3.x, uv3.y,  nm.x, nm.y, nm.z, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
							     
		pos1.x, pos1.y, pos1.z,  uv1.x, uv1.y,  nm.x, nm.y, nm.z, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos3.x, pos3.y, pos3.z,  uv3.x, uv3.y,  nm.x, nm.y, nm.z, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
		pos4.x, pos4.y, pos4.z,  uv4.x, uv4.y,  nm.x, nm.y, nm.z, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
	};
	// configure plane VAO
	glGenVertexArrays(1, &vao[0]);
	glGenBuffers(1, &vbo[0]);
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
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

	renderingProgram = Utils::createShaderProgram(workpath + "\\shader\\parallax_mapping.vs", workpath + "\\shader\\parallax_mapping.fs");

	bricksTex = Utils::loadTexture(workpath + "\\..\\Resources\\Texture\\bricks.jpg");
	normalMap  = Utils::loadTexture(workpath + "\\..\\Resources\\Texture\\bricks_normal.jpg");
	heightMap  = Utils::loadTexture(workpath + "\\..\\Resources\\Texture\\bricks_height.jpg");

	glUseProgram(renderingProgram);
	GLuint bricksTexLoc = glGetUniformLocation(renderingProgram, "bricksTex");
	GLuint normalMapLoc = glGetUniformLocation(renderingProgram, "normalMap");
	GLuint depthMapLoc = glGetUniformLocation(renderingProgram, "depthMap");

	glUniform1i(bricksTexLoc, 0);
	glUniform1i(normalMapLoc, 1);
	glUniform1i(depthMapLoc, 2);

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();
}

void display(GLFWwindow* window, double currentTime)
{
	vMat = glm::translate(glm::mat4(1.0f), -cameraPos);
	mMat = glm::mat4(1.0f);

	float frequency = 1;
	mMat = glm::rotate(mMat, 60 * glm::radians((float)sin(frequency *glfwGetTime())), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUseProgram(renderingProgram);

	GLuint vLoc = glGetUniformLocation(renderingProgram, "v_matrix");
	GLuint projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	GLuint modelLoc = glGetUniformLocation(renderingProgram, "model_matrix");

	GLuint lightPosLoc = glGetUniformLocation(renderingProgram, "lightPos");
	GLuint viewPosLoc = glGetUniformLocation(renderingProgram, "viewPos");
	GLuint heightScaleLoc = glGetUniformLocation(renderingProgram, "heightScale");

	glUniform3fv(lightPosLoc, 1, glm::value_ptr(currentLightPos));
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(cameraPos));
	glUniform1f(heightScaleLoc, heightScale);

	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mMat));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bricksTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normalMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, heightMap);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glBindVertexArray(vao[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

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
	//else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	//	cameraPos += glm::normalize(glm::cross(cameraFront, cameraAround)) * cameraSpeed;
	//else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	//	cameraPos -= glm::normalize(glm::cross(cameraFront, cameraAround)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		if (heightScale > 0.0f)
			heightScale -= 0.0005f;
		else
			heightScale = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		if (heightScale < 1.0f)
			heightScale += 0.0005f;
		else
			heightScale = 1.0f;
	}
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
	GLFWwindow* window = glfwCreateWindow(600, 600, "ParallaxMapping", NULL, NULL);
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
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}