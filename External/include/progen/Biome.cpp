#include "Biome.h"


Biome::Biome(double l, double u, glm::vec3 color_in)
	:
	lowerHeight(l),
	upperHeight(u),
	range(u-l),
	color(color_in)
{}

Biome::Biome(const Biome & other)
	:
	lowerHeight(other.lowerHeight),
	upperHeight(other.upperHeight),
	range(other.range),
	color(other.color)
{
}

Biome::~Biome()
{
}

/*
	Range is inclusive [l, u]
*/
bool Biome::inRange(double height) const
{
	return (height - lowerHeight) <= range;
}

glm::vec3 Biome::getColor() const
{
	return color;
}

double Biome::getLowerHeight() const
{
	return lowerHeight;
}

double Biome::getUpperHeight() const
{
	return upperHeight;
}
