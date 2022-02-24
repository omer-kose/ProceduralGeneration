#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H


#include <vector>

/*
	This is the improved Perlin Noise
	REFERENCE: https://cs.nyu.edu/~perlin/noise/
*/


class PerlinNoise
{
public:
	PerlinNoise();
	//in our case Z is not important 
	double noise(double x, double y, double z) const;
	std::vector<std::vector<double>> generateNoiseMap(int W, int H, double scale) const;
private:
	std::vector<int> p; //Permutation vector
private:
	double fade(double t) const;
	double lerp(double t, double a, double b) const;
	double grad(int hash, double x, double y, double z) const;
};

#endif