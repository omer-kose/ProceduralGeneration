#ifndef LAND_H
#define LAND_H

#include "Biome.h"

class Land : public Biome
{
public:
	Land(double l, double u, glm::vec3 color_in);
	~Land();
private:
};


#endif