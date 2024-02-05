#pragma once

#include<glm/glm.hpp>

struct BoneInfo
{
	int id;

	//模型空间到骨骼空间的偏移量
	glm::mat4 offset;

};
#pragma once
