#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>

using namespace std;

class Utils
{
private:
	static std::string readShaderFile(const string& filePath);
	static void printShaderLog(GLuint shader);
	static void printProgramLog(GLuint prog);
	static GLuint prepareShader(int shaderTYPE, const string& shaderPath);
	static GLint finalizeShaderProgram(GLuint sprogram, GLuint vShader = 0, GLuint fShader = 0, GLuint gShader = 0, GLuint tcShader = 0, GLuint teShader = 0);

public:
	static bool checkOpenGLError();
	static GLuint createShaderProgram(const string& vp, const string& fp);
	static GLuint createShaderProgram(const string& vp, const string& gp,  const string& fp);
	static GLuint createShaderProgram(const string& vp, const string& tCS, const string& tES, const string& fp);
	static GLuint createShaderProgram(const string& vp, const string& tCS, const string& tES, const string& gp, const string& fp);
	static GLuint loadTexture(const string& texImagePath);
	static GLuint loadTextureUseDSA(const string& texImagePath);
	static GLuint loadTextureNoDSA(const string& texImagePath);
	static GLuint loadCubeMap(const string& mapDir);
	static GLuint loadCubeMap(const vector<string>& faces);

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