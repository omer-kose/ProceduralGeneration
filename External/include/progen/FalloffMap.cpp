#include "FalloffMap.h"

FalloffMap::FalloffMap()
{}

std::vector<std::vector<double>> FalloffMap::generate(int W, int H)
{
	std::vector<std::vector<double>> falloffMap(H, std::vector<double>(W));
	for (int i = 0; i < H; ++i)
	{
		for (int j = 0; j < W; ++j)
		{
			double x = (j / (double)W) * 2 - 1;
			double y = (i / (double)H) * 2 - 1;

			double value = evaluate(std::max(fabs(x), fabs(y)));
			falloffMap[i][j] = value;
		}
	}

	return falloffMap;
}

double FalloffMap::evaluate(double value)
{
	double a = 3;
	double b = 2.2;

	return pow(value, a) / (pow(value, a) + pow(b - b * value, a));
}
