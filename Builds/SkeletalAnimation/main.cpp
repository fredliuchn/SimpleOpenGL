#include "Utils.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <windows.h>

#include "animator.h"
#include "model_animation.h"
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, rMat, invTrMat;

GLuint mvLoc, projLoc, nLoc, sLoc, vLoc, mLoc;

GLuint renderingProgram;

int width, height;

//相机位置
glm::vec3 cameraPos = glm::vec3(0.199998230f, 0.0999952555f, 1.99997163f);
//相机进退方向
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//相机升降方向
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//相机左右方向
glm::vec3 cameraAround = glm::vec3(1.0f, 0.0f, 0.0f);

float lastFrame;
Model ourModel;
Animator animator;

void init(GLFWwindow* window) 
{
	char exeFullPath[MAX_PATH];
	string strPath = "";

	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	strPath = (string)exeFullPath;
	int pos = strPath.find_last_of('\\', strPath.length());
	string workpath = strPath.substr(0, pos);

	renderingProgram = Utils::createShaderProgram(workpath + "\\shader\\vanim_model.glsl", workpath + "\\shader\\fanim_model.glsl");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	// load models
	// -----------
	ourModel = Model(workpath +"\\..\\Resources\\vampire\\dancing_vampire.dae");
	string strAnimation = workpath + "\\..\\Resources\\vampire\\dancing_vampire.dae";
	Animation *danceAnimation=new Animation(strAnimation, &ourModel);
	animator = Animator(danceAnimation);
}

void display(GLFWwindow* window, double currentTime)
{
	float deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;
	animator.UpdateAnimation(deltaTime);

	vMat = glm::translate(glm::mat4(1.0f), -cameraPos);

	// render
	// ------
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// don't forget to enable shader before setting uniforms
	glUseProgram(renderingProgram);

	// view/projection transformations
	vLoc = glGetUniformLocation(renderingProgram, "view");
	projLoc = glGetUniformLocation(renderingProgram, "projection");

	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	std::vector<glm::mat4> transforms = animator.GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i)
	{
		string struniform = "finalBonesMatrices[" + std::to_string(i) + "]";
		GLuint uniform = glGetUniformLocation(renderingProgram, struniform.c_str());
		glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(transforms[i]));
	}

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

	mLoc = glGetUniformLocation(renderingProgram, "model");
	glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(model));
	ourModel.Draw(renderingProgram);
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) 
{
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

void processInput(GLFWwindow *window)
{
	float cameraSpeed = 0.05f;
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cameraFront.y += yoffset;
	cameraPos += cameraFront;
}

int main()
{
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(600, 600, "SkeletalAnimation", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	init(window);
	lastFrame = glfwGetTime();
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


