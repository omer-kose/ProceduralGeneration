#include "Terrain.h"

Terrain::Terrain()
{
	createTerrainOpenGLInformation();
}


void Terrain::generate(const TerrainData& tData, const NoiseData& nData)
{
	std::vector<std::vector<double>> noiseMap = noise.generateNoiseMap(nData);
	generateTerrain(tData, noiseMap);
}

void Terrain::generateTerrain(const TerrainData& tData, const std::vector<std::vector<double>>& heightMap)
{
	//I am lazy
	using namespace std;
	using namespace glm;

	vector<ivec3> tris;
	vector<vec3> data; //Total drawing data in the form pos1|norm1|pos2|norm2...

	int vi = 0; //index of the currently created vertex

	//Generate from top-left to bottom-right. (If thinked in 2D)
	for (int z = 0; z < tData.numZVertices; ++z)
	{
		for (int x = 0; x < tData.numXVertices; ++x)
		{
			//Generate the normalized point
			vec3 p = vec3(x / (float)(tData.numXVertices - 1), 0.0, z / (float)(tData.numZVertices - 1));
			//Cast it back in range [-W/2,-L/2:W/2,L/2] range	
			p.x *= tData.W;
			p.z *= tData.L;
			p.x -= tData.W / 2.0;
			p.z -= tData.L / 2.0;

			//Pick the height value from the given height map
			p.y = heightMap[z][x];

			vec3 n = vec3(0.0, 1.0, 0.0);

			//Now generate quads (2 triangles) in the following fashion:
			/*
					 i  i+1
					 ^___^
					 |\  |
					 | \ |
			(i+numXVertices)>|__\|
			*/
			if (((vi + 1) % tData.numXVertices != 0) && ((z + 1) < tData.numZVertices))
			{
				ivec3 tri1 = ivec3(vi, vi + tData.numXVertices, vi + tData.numXVertices + 1);
				ivec3 tri2 = ivec3(vi, vi + tData.numXVertices + 1, vi + 1);

				tris.push_back(tri1);
				tris.push_back(tri2);
			}

			data.push_back(p);
			data.push_back(n);
			++vi;
		}
	}

	//Bind VAO
	glBindVertexArray(terrainVAO);
	//Bind VBO, send data
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * data.size(), data.data(), GL_STATIC_DRAW);
	//Bind EBO, send indices 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ivec3) * tris.size(), tris.data(), GL_STATIC_DRAW);

	//Configure Vertex Attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	//Data passing and configuration is done 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	triCount = tris.size();
}

void Terrain::createTerrainOpenGLInformation()
{
	//Now set and configure the data for OpenGL
	glGenVertexArrays(1, &terrainVAO);
	glGenBuffers(1, &terrainVBO);
	glGenBuffers(1, &terrainEBO);
}

Terrain::~Terrain()
{
	glDeleteVertexArrays(1, &terrainVAO);
	glDeleteBuffers(1, &terrainVBO);
	glDeleteBuffers(1, &terrainEBO);
}

void Terrain::renderTerrain(Shader& shader, const Camera& camera) const
{
	shader.use();
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 PV = projection * view;
	shader.setMat4("PVM", PV * model);
	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
}


