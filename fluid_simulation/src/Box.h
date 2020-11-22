#pragma once
#include <stdlib.h>
#include<malloc.h>
#include <vector>
#include <list>
#include "Constant.h"
#include "Particle.h"

typedef struct Node 
{
	int index;
	struct Node* next;
};
class Box 
{
public:

	//���ӵ���С�߽�㣬���ڼ���������������
	//(position_x - min_x)/unitlength ��������x��ڼ������ӡ�
	//yzͬ��

	//���ֵ���ڼ��������ռ�ı䳤��
	//(max_x - min_x)/initlength ����ÿ���߷ָ������
	Constant constant;
	//��Ԫ���ڵ�����
	Node* particleIndex = NULL;
	// ��  ��   ��  �зָ������
	int number_x = (int)((constant.max_x - constant.min_x) / constant.initDistance);
	int number_y = (int)((constant.max_y - constant.min_y) / constant.initDistance);
	int number_z = (int)((constant.max_z - constant.min_z) / constant.initDistance);
	int gridNumber = number_x * number_y * number_z;

	void init() 
	{
		particleIndex = (Node*)malloc(sizeof(Node));
		particleIndex->next = NULL;
	}

	void insertParticle(int index) 
	{
		Node* node = (Node*)malloc(sizeof(Node));

		node->index = index;
		node->next = particleIndex->next;

		particleIndex->next = node;
	}

	void deleteParticle(int index)
	{
		Node* p = particleIndex;

		while (p ->next)
		{
			if (p->next->index == index)
			{
				Node* q = p->next;
				p->next = p->next->next;
				free(q);
			}
			if (p->next != NULL)
				p = p->next;
		}
	}

	void print() 
	{
		Node* p = particleIndex;
		while (p->next)
		{
			std::cout<< p->next->index;
			p = p->next;
		}
	}
	std::vector<int> getParticleIndex() 
	{
		std::vector<int>result;
		Node* p = particleIndex;
		while (p->next)
		{
			result.push_back(p->next->index);
			p = p->next;
		}
		return result;
	}
	int getBoxIndex(glm::vec3 position) 
	{
		int i, j, k;
		i = (int)((position.x - constant.min_x) / constant.initDistance);
		j = (int)((position.y - constant.min_y) / constant.initDistance);
		k = (int)((position.z - constant.min_z) / constant.initDistance);
		return i + j * number_x + (number_x * number_y) * k;
	}
	std::vector<int> getALLGridIndex(glm::vec3 position) 
	{
		std::vector<int> result;
		int x = round((position.x- constant.min_x) / constant.initDistance);
		int y = round((position.y- constant.min_y) / constant.initDistance);
		int z = round((position.z- constant.min_z) / constant.initDistance);

		result.push_back(x + number_x * y + z * (number_x * number_y));
		result.push_back(x + number_x * (y-1) + z * (number_x * number_y));
		result.push_back(x + number_x * y + (z-1) * (number_x * number_y));
		result.push_back(x + number_x * (y-1) + (z-1) * (number_x * number_y));
		result.push_back((x-1)+ number_x * y + z * (number_x * number_y));
		result.push_back((x-1) + number_x * (y - 1) + z * (number_x * number_y));
		result.push_back((x-1) + number_x * y + (z - 1) * (number_x * number_y));
		result.push_back((x-1) + number_x * (y - 1) + (z - 1) * (number_x * number_y));
		return result;
	}
};