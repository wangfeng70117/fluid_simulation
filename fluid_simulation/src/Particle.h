#ifndef PARTICLE_H
#define PARTICLE_H
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
//Make sure these structures are packed to a multiple of 4 bytes (sizeof(float)),
//since glMap2 takes strides as integer multiples of 4 bytes.
//How consistent...
#pragma pack(push)
#pragma pack(4)
class Particle {
public:
	//密度
	float density = 1.0f;
	//压强
	float intensityPressure = 0.10f;
	//位置
	glm::vec3 position;
	//速度
	glm::vec3 velocity;
	//法向量
	glm::vec3 normal;
	//加速度
	glm::vec3 acceleration;
	//所在单元格编号
	int boxIndex;
};
#pragma pack(pop)

#endif	//PARTICLE_H
