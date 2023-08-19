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
	static GLuint createShaderProgram(const char *vp, const char *gp, const char *fp);
	static GLuint createShaderProgram(const char *vp, const char *tCS, const char* tES, const char *fp);
	static GLuint createShaderProgram(const char *vp, const char *tCS, const char* tES, char *gp, const char *fp);
	static GLuint loadTexture(const char *texImagePath);
	static GLuint loadCubeMap(const string mapDir);
	static GLuint loadCubeMap(vector<string> faces);

	static float* goldAmbient();
	static float* goldDiffuse();
	static float* goldSpecular();
	static float goldShininess();

	static float* silverAmbient();
	static float* silverDiffuse();
	static float* silverSpecular();
	static float silverShininess();

	static float* bronzeAmbient();
	static float* bronzeDiffuse();
	static float* bronzeSpecular();
	static float bronzeShininess();
};