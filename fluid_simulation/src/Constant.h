#pragma once
#include "math.h"
class Constant 
{
public:

	//法线方向衰减系数
	float normalDragCoefficient = 0.2f;
	//切线方向衰减系数
	float tangentDragCoefficient = 0.8f;
	float PAI = 3.1415926f;
	float smoothRadius = 0.01f;
	//粒子间距
	float initDistance = 0.6f * smoothRadius;
	float mass = 0.0002f;

	float pressutr_k = 0.04f;
	//粘度
	float viscosityCoefficient = 1.0f;
	float densityKernel = 315 * mass / ((64 * PAI) * pow(smoothRadius, 9));
	float pressureKernel = 45.0f  / (PAI * pow(smoothRadius, 6));
	int gridSize = 7;
	//水的初始密度
	float initDensity = 1000.0f;

	float min_x = -((float)gridSize / 2) * initDistance;
	float min_y = -((float)gridSize / 2) * initDistance;
	float min_z = -((float)gridSize / 2) * initDistance;
	float max_x = ((float)gridSize / 2) * initDistance;
	float max_y = ((float)gridSize / 2) * initDistance;
	float max_z = ((float)gridSize / 2) * initDistance;

};