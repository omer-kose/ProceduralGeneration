#ifndef GRASS_h
#define GRASS_H

#include "Biome.h"

class Grass : public Biome
{
public:
	Grass(double l, double u, glm::vec3 color_in);
	~Grass();
private:
};


#endif