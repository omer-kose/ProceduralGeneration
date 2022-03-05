#ifndef FALLOFFMAP_H
#define FALLOFFMAP_H

#include <vector>

class FalloffMap
{
public:
	FalloffMap();
	std::vector<std::vector<double> >generate(int W, int H);
private:
	double evaluate(double value);
};


#endif