#include "terrainGenerator.h"

using namespace std;

TerrainGenerator::TerrainGenerator(float h)
{
	srand( time(NULL));
	h > 1.0 ? roughness = 1.0 : h < 0.0 ? roughness = 0.0 : roughness = h;
}


TerrainGenerator::~TerrainGenerator()
{
}

int TerrainGenerator::determineLength(int n) {
	return pow(pow((float)2, n) + 1, 2);
}

