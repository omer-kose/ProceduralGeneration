#ifndef WATER_H
#define WATER_H

#include "Biome.h"

class Water : public Biome
{
public:
	Water(double l, double h, glm::vec3 color_in);
	~Water();
private:
};



#endif