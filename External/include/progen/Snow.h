#ifndef SNOW_H
#define SNOW_H

#include "Biome.h"

class Snow : public Biome
{
public:
	Snow(double l, double h, glm::vec3 color_in);
	~Snow();
private:
};


#endif // !SNOW_H
