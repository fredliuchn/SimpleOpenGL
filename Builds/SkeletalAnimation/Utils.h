#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

using namespace std;

class Utils
{
private:
	static std::string readShaderFile(string filePath);
	static void printShaderLog(GLuint shader);
	static void printProgramLog(int prog);
	static GLuint prepareShader(int shaderTYPE, string shaderPath);
	static int finalizeShaderProgram(GLuint sprogram);

public:
	static bool checkOpenGLError();
	static GLuint createShaderProgram(string vp, string fp);
};