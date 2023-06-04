#version 430

layout (location=0) in vec3 pos;
layout (location=1 ) in vec3 color;
layout (location=2 ) in vec2 texCoord;
layout (location=3 ) in vec3 normal;

out vec3 acolor;
out vec3 varyingNormal;
out vec3 varyingLightDir;
out vec3 varyingVertPos;
out vec3 varyingHalfVector;

struct PositionalLight
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};
struct Material
{	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform PositionalLight light;
uniform Material material;
uniform vec4 globalAmbient;
uniform mat4 norm_matrix;

void main(void)
{
	//将顶点位置转换到视觉空间
	varyingVertPos = (mv_matrix * vec4(pos,1.0)).xyz;
	
	//计算视觉空间光照向量(从顶点到光源)
	varyingLightDir = normalize(light.position - varyingVertPos);
	
	//将法向量转换到视觉空间
	varyingNormal = (norm_matrix * vec4(normal,1.0)).xyz;
	
	varyingHalfVector = normalize(varyingLightDir) + normalize(-varyingVertPos).xyz;
	
	gl_Position = proj_matrix * mv_matrix * vec4(pos,1.0);
	
	acolor=color;
} 
