#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Utilities.h"
#include "PerlinNoise.h"
#include "Shader.h"


/*
	Necessary data needed for Terrain
*/

struct TerrainData
{
	int W, L;
	int numXVertices;
	int numZVertices;
};



/*
	Class that encapsulates the procedurally generated terrain.
	Outsiders will only use the class in the following fashion:
	terrain.generate(...);
	terrain.render(...);
	
	Everything including generation, OpenGL buffer management and rendering is handled here.


	Terrain and Noise data are not stored inside the Terrain class since they are volatile. 
	Also, I see no point in storing them.
*/

class Terrain
{
public:	
	Terrain();
	~Terrain();
	void generate(const TerrainData& tData, const NoiseData& nData);
	void renderTerrain(Shader& shader, const Camera& camera) const;
private:
	void generateTerrain(const TerrainData& tData, const std::vector<std::vector<double>>& heightMap);
	void createTerrainOpenGLInformation();
private:
	GLuint terrainVAO, terrainVBO, terrainEBO;
	GLuint triCount;
	PerlinNoise noise;
};

#endif