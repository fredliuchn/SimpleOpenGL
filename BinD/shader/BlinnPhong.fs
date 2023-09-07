#version 430

in vec2 tc;
out vec4 color;
in vec3 acolor;

in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec3 varyingHalfVector;

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

void main(void)
{	
	//从顶点到光源向量
	vec3 L = normalize(varyingLightDir);
	
	//将法向量转换到视觉空间
	vec3 N = normalize(varyingNormal);
	
	//将顶点位置转换到视觉空间取反
	vec3 V = normalize(-varyingVertPos);
	
	// get the angle between the light and surface normal:
	float cosTheta = dot(L,N);
	
	// halfway vector varyingHalfVector was computed in the vertex shader,
	// and interpolated prior to reaching the fragment shader.
	// It is copied into variable H here for convenience later.
	vec3 H = normalize(varyingHalfVector);
	
	// get angle between the normal and the halfway vector
	float cosPhi = dot(H,N);

	// compute ADS contributions (per pixel):
	vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
	vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta,0.0);
	vec3 specular = light.specular.xyz * material.specular.xyz * pow(max(cosPhi,0.0), material.shininess*3.0);
	color = vec4((ambient + diffuse + specular), 1.0);
    //color = vec4(1.0,0.0,0.0,1.0);

}
