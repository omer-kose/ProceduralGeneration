#include "PerlinNoise.h"


PerlinNoise::PerlinNoise()
{
	//Permutation vector
	p = 
	{ 
		151,160,137,91,90,15,
	   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	//Duplicate the permutation vector
	p.insert(p.end(), p.begin(), p.end());
}

double PerlinNoise::noise(double x, double y, double z) const
{
	// Find the unit cube that contains the point
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;
	int Z = (int)floor(z) & 255;
	
	// Find relative x, y,z of point in cube
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	// Compute fade curves for each of x, y, z
	double u = fade(x);
	double v = fade(y);
	double w = fade(z);

	// Hash coordinates of the 8 cube corners
	int A = p[X] + Y;
	int AA = p[A] + Z;
	int AB = p[A + 1] + Z;
	int B = p[X + 1] + Y;
	int BA = p[B] + Z;
	int BB = p[B + 1] + Z;

	// Add blended results from 8 corners of cube
	return  lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)), lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))), lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)), lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

std::vector<std::vector<double>> PerlinNoise::generateNoiseMap
(	int W, //Width of the map 
	int H, //Height of the map
	int seed, //Seed of the MT Pseudo Random Engine 
	double scale, //Scale to tweak x and y integer values.
	int octaves, //How many octaves do we have ?  
	double persistence, 
	double lacunarity,
	glm::vec2 offset //total offset of octaves
) const
{
	if (scale <= 0)
	{
		scale = 0.001;
	}

	std::vector<std::vector<double>> noiseMap(H, std::vector<double>(W));
	std::mt19937 mt(seed);
	std::uniform_real_distribution<double> dist(-10000, 10000); 
	//We want to each octave to be sampled from a different location of the Perlin Noise Map
	//So each octave will use an offset
	std::vector<glm::vec2> octaveOffsets(octaves);

	double halfW = W / 2;
	double halfH = H / 2;


	for (int i = 0; i < octaves; ++i)
	{
		double offsetX = dist(mt);
		double offsetY = dist(mt);
		octaveOffsets[i].x = offsetX + offset.x;
		octaveOffsets[i].y = offsetY + offset.y;
	}

	//Will be used to normalize the map
	double minHeight, maxHeight; 
	minHeight = DBL_MAX;
	maxHeight = DBL_MIN;

	for (int y = 0; y < H; ++y)
	{
		for (int x = 0; x < W; ++x)
		{
			//Each noise value will consists of octaves whose frequencies and amplitudes
			//are increased/decreased by the effect of lacunarity and persistence
			double amplitude = 1.0;
			double frequency = 1.0;
			double noiseHeight = 0.0; //Will be accumulated from octaves
			for (int i = 0; i < octaves; ++i)
			{
				//Frequency increases the range we take our values from the Noise Map
				//Offset is a random seeded pseudo value so that we offset the octave we take
				//our value from.
				//Also we center the our samples at the center of the map
				double sampleX = ((x - halfW) / scale) * frequency + octaveOffsets[i].x;
				double sampleY = ((y - halfH) / scale) * frequency + octaveOffsets[i].y;

				//For now using 0 as Z value
				double noiseVal = noise(sampleX, sampleY, 0.0);
				noiseHeight += noiseVal * amplitude;
				amplitude *= persistence;
				frequency *= lacunarity;
			}

			noiseMap[y][x] = noiseHeight;
			maxHeight = std::max(noiseHeight, maxHeight);
			minHeight = std::min(noiseHeight, minHeight);

		}
	}

	//Normalize the map so that it is mapped between 0.0 and 1.0 
	for (int y = 0; y < H; ++y)
	{
		for (int x = 0; x < W; ++x)
			noiseMap[y][x] = inverseLerp(minHeight, maxHeight, noiseMap[y][x]);
	}

	return noiseMap;

}

std::vector<std::vector<double>> PerlinNoise::generateNoiseMap(const NoiseData& noiseData) const
{
	std::vector<std::vector<double>> noiseMap(noiseData.H, std::vector<double>(noiseData.W));
	std::mt19937 mt(noiseData.seed);
	std::uniform_real_distribution<double> dist(-10000, 10000);
	//We want to each octave to be sampled from a different location of the Perlin Noise Map
	//So each octave will use an offset
	std::vector<glm::vec2> octaveOffsets(noiseData.octaves);

	double halfW = noiseData.W / 2;
	double halfH = noiseData.H / 2;


	for (int i = 0; i < noiseData.octaves; ++i)
	{
		double offsetX = dist(mt);
		double offsetY = dist(mt);
		octaveOffsets[i].x = offsetX + noiseData.offset.x;
		octaveOffsets[i].y = offsetY + noiseData.offset.y;
	}

	//Will be used to normalize the map
	double minHeight, maxHeight;
	minHeight = DBL_MAX;
	maxHeight = DBL_MIN;

	for (int y = 0; y < noiseData.H; ++y)
	{
		for (int x = 0; x < noiseData.W; ++x)
		{
			//Each noise value will consists of octaves whose frequencies and amplitudes
			//are increased/decreased by the effect of lacunarity and persistence
			double amplitude = 1.0;
			double frequency = 1.0;
			double noiseHeight = 0.0; //Will be accumulated from octaves
			for (int i = 0; i < noiseData.octaves; ++i)
			{
				//Frequency increases the range we take our values from the Noise Map
				//Offset is a random seeded pseudo value so that we offset the octave we take
				//our value from.
				//Also we center the our samples at the center of the map
				double sampleX = ((x - halfW) / noiseData.scale) * frequency + octaveOffsets[i].x;
				double sampleY = ((y - halfH) / noiseData.scale) * frequency + octaveOffsets[i].y;

				//For now using 0 as Z value
				double noiseVal = noise(sampleX, sampleY, 0.0);
				noiseHeight += noiseVal * amplitude;
				amplitude *= noiseData.persistence;
				frequency *= noiseData.lacunarity;
			}

			noiseMap[y][x] = noiseHeight;
			maxHeight = std::max(noiseHeight, maxHeight);
			minHeight = std::min(noiseHeight, minHeight);

		}
	}

	//Normalize the map so that it is mapped between 0.0 and 1.0 
	for (int y = 0; y < noiseData.H; ++y)
	{
		for (int x = 0; x < noiseData.W; ++x)
			noiseMap[y][x] = inverseLerp(minHeight, maxHeight, noiseMap[y][x]);
	}

	return noiseMap;
}

double PerlinNoise::fade(double t) const
{
	return t * t * t * (t * (t * 6 - 15) + 10);;
}

double PerlinNoise::lerp(double t, double a, double b) const
{
	return a + t * (b - a);
}

double PerlinNoise::inverseLerp(double a, double b, double val) const
{
	return (val-a) / (b-a);
}

double PerlinNoise::grad(int hash, double x, double y, double z) const
{
	int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
	double u = h < 8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
