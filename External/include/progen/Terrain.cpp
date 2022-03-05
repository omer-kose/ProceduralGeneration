#include "Terrain.h"

Terrain::Terrain()
{
	biomes.push_back(&WATER);
	biomes.push_back(&GRASS);
	biomes.push_back(&LAND);
	biomes.push_back(&SNOW);
	createTerrainOpenGLInformation();
}


void Terrain::generate(TerrainData& tData, const NoiseData& nData)
{
	std::vector<std::vector<double>> noiseMap = noise.generateNoiseMap(nData);
	tData.fallOffMap = fallOff.generate(tData.numXVertices, tData.numZVertices);
	generateTerrain(tData, noiseMap);
}

void Terrain::setupOpenGLBuffers()
{
	//Bind VAO
	glBindVertexArray(terrainVAO);
	//Bind VBO, send data
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexData.size(), vertexData.data(), GL_STATIC_DRAW);
	//Bind EBO, send indices 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * tris.size(), tris.data(), GL_STATIC_DRAW);

	//Configure Vertex Attributes
	//POSITION
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//NORMALS
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	//Color
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

	//Data passing and configuration is done 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Terrain::computeNormals()
{
	//Traverse each triangle and compute face normal
	for (int i = 0; i < tris.size(); ++i)
	{
		Vertex& v1 = vertexData[tris[i][0]];
		Vertex& v2 = vertexData[tris[i][1]];
		Vertex& v3 = vertexData[tris[i][2]];
		//Triangle vertices are oriented counter-clockwise (I have written so while generating triangles)
		glm::vec3 n = glm::normalize(glm::cross(v2.pos - v1.pos, v3.pos - v1.pos));
		//Accumulate normals in incident vertices
		v1.normal += n;
		v2.normal += n;
		v3.normal += n;
	}

	//Normalize the normals
	for (Vertex& v : vertexData)
		v.normal = glm::normalize(v.normal);
}


/*
	Given the height map and information generateTerrain creates position and connectivity data.
	Depending on the improvements it can do more or less.

	If falloff map is enabled then the terrain becomes an island. 
*/
void Terrain::generateTerrain(TerrainData& tData, const std::vector<std::vector<double>>& heightMap)
{
	//I am lazy
	using namespace std;
	using namespace glm;

	//Clear up the previous data
	vertexData.clear();
	tris.clear();


	int vi = 0; //index of the currently created vertex

	//Generate from top-left to bottom-right. (If thinked in 2D)
	for (int z = 0; z < tData.numZVertices; ++z)
	{
		for (int x = 0; x < tData.numXVertices; ++x)
		{
			Vertex v;
			//Generate the normalized point
			vec3 p = vec3(x / (float)(tData.numXVertices - 1), 0.0, z / (float)(tData.numZVertices - 1));
			//Cast it back in range [-W/2,-L/2:W/2,L/2] range	
			p.x *= tData.W;
			p.z *= tData.L;
			p.x -= tData.W / 2.0;
			p.z -= tData.L / 2.0;

			//If using falloff map the height map value will be updated accordingly
			//So, at the corners of the terrain the value will be diminished by falloff map
			//which will give an impression of island to the terrain.
			double heightValue = heightMap[z][x];
			if (tData.useFallOff)
			{
				heightValue = heightValue - tData.fallOffMap[z][x];
			}

			//Note that since I scale the heights with height multiplier bellow
			//Determining biome should come first before setting the actual height
			//Basically, I first pick the biome in the range [0.0,1.0]
			//Traverse biomes and see which biome fit
			for (const Biome* biome : biomes)
			{
				if (biome->inRange(heightValue))
				{
					v.color = biome->getColor();
					break;
				}
			}

			//Pick the height value from the given height map
			//p.y = heightMap[z][x];
			p.y = ImGui::BezierValue(heightValue, tData.controlPoints) * tData.heightMultiplier;



			vec3 n = vec3(0.0, 1.0, 0.0);

			v.pos = p;
			v.normal = n;

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
				//Oriented counter-clockwise
				ivec3 tri1 = ivec3(vi, vi + tData.numXVertices, vi + tData.numXVertices + 1);
				ivec3 tri2 = ivec3(vi, vi + tData.numXVertices + 1, vi + 1);

				tris.push_back(tri1);
				tris.push_back(tri2);
			}

			vertexData.push_back(v);
			++vi;
		}
	}

	triCount = tris.size();
	setupOpenGLBuffers();
	computeNormals();

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

void Terrain::renderTerrain
(
	Shader& shader, 
	const Camera& camera,
	const glm::vec3& lightDir,
	const glm::vec3& lightColor
) const
{
	shader.use();
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 PV = projection * view;
	//Matrices
	shader.setMat4("PVM", PV * model);
	shader.setMat4("modelMat", model);
	shader.setMat3("normalTransformation", glm::transpose(glm::inverse(glm::mat3(model))));
	//Light Properties
	shader.setVec3("lightDir", lightDir);
	shader.setVec3("lightColor", lightColor);
	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, 3 * triCount, GL_UNSIGNED_INT, 0);
}


