#include<math.h>
class FluidSystem 
{
public:
private:
	float PAI = 3.141592f;
	float smmothRadius = 1.0f;
	
	float densityKernel = 315 / (64 * PAI * pow(smmothRadius, 9));
	float pressureKernel = 45 / (PAI * pow(smmothRadius, 6));
	float viscosityKernel = 45.0f / (PAI * pow(smmothRadius, 6));
};