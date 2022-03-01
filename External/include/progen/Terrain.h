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
#include "curveEditor.h"

//Biomes
#include "Biome.h"
#include "Water.h"
#include "Land.h"
#include "Grass.h"
#include "Snow.h"



/*	
	Vertex data for OpenGL
*/
struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec3 color;
};



/*
	Necessary data needed for Terrain
*/

struct TerrainData
{
	int W, L;
	int numXVertices;
	int numZVertices;
	float heightMultiplier;
	float controlPoints[4]; //Bezier Curve Control Point Data (x1,y1,x2,y2)
};


static Water WATER(0.0, 0.3, glm::vec3(0.1, 0.4, 0.6));
static Grass GRASS(0.31, 0.6, glm::vec3(0.37, 0.502, 0.22));
static Land LAND(0.61, 0.89, glm::vec3(0.3, 0.2, 0.0));
static Snow SNOW(0.9, 1.0, glm::vec3(1.0, 1.0, 1.0));


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
	void generate(TerrainData& tData, const NoiseData& nData);
	void renderTerrain
	(Shader& shader, 
	 const Camera& camera,
	 const glm::vec3& lightDir,
	 const glm::vec3& lightColor
	) const;
private:
	void generateTerrain(TerrainData& tData, const std::vector<std::vector<double>>& heightMap);
	void createTerrainOpenGLInformation();
	void setupOpenGLBuffers();
	void computeNormals();
private:
	GLuint terrainVAO, terrainVBO, terrainEBO;
	GLuint triCount;
	std::vector<Vertex> vertexData; //Total drawing data in the form v1|v2|v3... 
	std::vector<glm::ivec3> tris;
	std::vector<Biome*> biomes;
	PerlinNoise noise;
};

#endif