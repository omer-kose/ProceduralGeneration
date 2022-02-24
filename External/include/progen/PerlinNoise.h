#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H


#include <vector>
#include <random>
#include <glm/glm.hpp>

/*
	This is the improved Perlin Noise
	REFERENCE: https://cs.nyu.edu/~perlin/noise/

	Generating the Noise Map
	REFERENCE: Sebastian Lague's Procedural Terrain Generation Series
	Youtube Channel: https://www.youtube.com/channel/UCmtyQOKKmrMVaKuRXz02jbQ
*/


class PerlinNoise
{
public:
	PerlinNoise();
	//in our case Z is not important 
	double noise(double x, double y, double z) const;
	std::vector<std::vector<double>> generateNoiseMap(
		int W, 
		int H, 
		int seed,
		double scale, 
		int octaves, 
		double persistence, 
		double lacunarity,
		glm::vec2 offset
													) const;
private:
	std::vector<int> p; //Permutation vector
private:
	double fade(double t) const;
	double lerp(double t, double a, double b) const;
	double inverseLerp(double a, double b, double val) const;
	double grad(int hash, double x, double y, double z) const;
};

#endif