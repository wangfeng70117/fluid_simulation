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
	//�ܶ�
	float density = 1.0f;
	//ѹǿ
	float intensityPressure = 0.10f;
	//λ��
	glm::vec3 position;
	//�ٶ�
	glm::vec3 velocity;
	//������
	glm::vec3 normal;
	//���ٶ�
	glm::vec3 acceleration;
	//���ڵ�Ԫ����
	int boxIndex;
};
#pragma pack(pop)

#endif	//PARTICLE_H
