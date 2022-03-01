#ifndef BIOME_H
#define BIOME_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
	Biome is a base class that will be the parent of all other biomes.
*/

class Biome
{
public:
	Biome(double l, double u, glm::vec3 color_in);
	Biome(const Biome& other);
	~Biome();
	bool inRange(double height) const;
	glm::vec3 getColor() const;
	double getLowerHeight() const;
	double getUpperHeight() const;
private:
	double lowerHeight, upperHeight;
	double range; // (upper-lower)
	glm::vec3 color; //For now every biome will have a solid color
};

#endif