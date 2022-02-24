#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



//My headers
#include "progen/Utilities.h"
#include "progen/Shader.h"
#include "progen/Camera.h"
#include "progen/PerlinNoise.h"



//Utility Headers
#include <iostream>
#include <vector>


//ImGui
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_glfw.h> 
#include <ImGui/imgui_impl_opengl3.h>



Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


//Time parameters
double deltaTime = 0.0;
double lastFrame = 0.0;

//Window
GLFWwindow* window;


//ImGui Variables
ImGuiIO* io;



//Terrain Variables
//Some globals that will be used to draw the terrain
GLuint terrainVAO, terrainVBO, terrainEBO;
GLuint triCount;
int W = 10, H = 10;
int numXVertices = 256;
int numZVertices = 256;
//Noise Parameters
double scale = 0.3;
double octaves = 4;
double persistence = 0.5;
double lacunarity = 2.0;
int seed = 21;
glm::vec2 offset = glm::vec2(0.0, 0.0);



void updateDeltaTime()
{
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

//Callback function in case of resizing the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//Function that will process the inputs, such as keyboard inputs
void processInput(GLFWwindow* window)
{
	//If pressed glfwGetKey return GLFW_PRESS, if not it returns GLFW_RELEASE
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	int speedUp = 1; //Default

	//If shift is pressed move the camera faster
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speedUp = 2;	
	
	//Camera movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, deltaTime, speedUp);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, deltaTime, speedUp);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, deltaTime, speedUp);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, deltaTime, speedUp);



	//Camera y-axis movement
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.moveCameraUp(deltaTime, speedUp);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.moveCameraDown(deltaTime, speedUp);

}

//Callback function for mouse position inputs
void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		camera.processMouseMovement(xPos, yPos, GL_TRUE);
	}

	camera.setLastX(xPos);
	camera.setLastY(yPos);	
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.processMouseScroll(yOffset);
}


int setup()
{
	glfwInit();
	//Specify the version and the OpenGL profile. We are using version 3.3
	//Note that these functions set features for the next call of glfwCreateWindow
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create the window object
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Window", NULL, NULL);
	if (window == nullptr)
	{
		std::cout << "Failed to create the window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	//Before calling any OpenGL function we need to initialize GLAD since it manages the function pointers
	//for OpenGL, which are OS-Specific.
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Specify the actual window rectangle for renderings.
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//Register our size callback funtion to GLFW.
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//GLFW will capture the mouse and will hide the cursor
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Configure Global OpenGL State
	glEnable(GL_DEPTH_TEST);


	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	return 0;
}



//For testing purposes.
GLuint testRectangle()
{
	GLfloat vertices[] = 
	{
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 
	};

	GLuint indices[] = 
	{  
		2, 1, 0,  // first Triangle
		2, 0, 3   // second Triangle
	};

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	//Bind VAO
	glBindVertexArray(VAO);
	//Bind VBO, send data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	//Bind EBO, send indices 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	//Configure Vertex Attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

	//Data passing and configuration is done 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

void renderTestRectangle(GLuint VAO, Shader shader)
{
	shader.use();
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.getFov()), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 PV = projection * view;
	shader.setMat4("PVM", PV * model);
	glBindVertexArray(VAO);
	//total 6 indices since we have triangles
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}





/*
	Generate Terrain generates an heightmap given: 
	Size:
	W: Width of the terrain
	L: Length of the terrain
	Resolution:
	numXVertices: Number of vertices do we have in X axis
	numZVertices: Number of vertices do we have in Z axis

	For vertex generation the following approach is used:
	Along X and Z axis I will generate vertices and normalize them between -0.5 and 0.5
	then cast them back between [-W/2,-L/2:W/2,L/2]  range.
*/
void generateTerrain
(	int W, 
	int L, 
	int numXVertices, 
	int numZVertices,
	const std::vector<std::vector<double>>& heightMap
)
{
	//I am lazy
	using namespace std;
	using namespace glm;
	
	const int nVertices = numXVertices * numZVertices;
	vector<ivec3> tris;
	vector<vec3> data; //Total drawing data in the form pos1|norm1|pos2|norm2...

	int vi = 0; //index of the currently created vertex

	//Generate from top-left to bottom-right. (If thinked in 2D)
	for (int z = 0; z < numZVertices; ++z)
	{
		for (int x = 0; x < numXVertices; ++x)
		{
			//Generate the normalized point
			vec3 p = vec3(x/(float)(numXVertices-1), 0.0, z/(float)(numZVertices-1));
			//Cast it back in range [-W/2,-L/2:W/2,L/2] range	
			p.x *= W;
			p.z *= L;
			p.x -= W / 2.0;
			p.z -= L / 2.0;

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
			if (((vi + 1) % numXVertices != 0) && ((z + 1) < numZVertices))
			{
				ivec3 tri1 = ivec3(vi, vi+numXVertices, vi+numXVertices+1);
				ivec3 tri2 = ivec3(vi, vi+numXVertices+1, vi+1);

				tris.push_back(tri1);
				tris.push_back(tri2);
			}

			data.push_back(p);
			data.push_back(n);
			++vi;
		}
	}

	//Now set and configure the data for OpenGL
	glGenVertexArrays(1, &terrainVAO);
	glGenBuffers(1, &terrainVBO);
	glGenBuffers(1, &terrainEBO);

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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
	
	//Data passing and configuration is done 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	triCount = tris.size();

}

void renderTheTerrain(Shader shader)
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


void handleImGui()
{
	//Set the new ImGui Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	//ImGui Handling
	ImGui::Begin("Test Window");
	ImGui::End();


	//Render the ImGui Window
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main()
{
	setup();

	PerlinNoise noise;
	std::vector<std::vector<double>> noiseMap = noise.generateNoiseMap(numXVertices, numZVertices, seed, scale, octaves, persistence, lacunarity, offset);

	//for (const auto& row : noiseMap)
	//{
	//	std::cout << ":::::::::::::::::ROW::::::::::::\n";
	//	for (double d : row)
	//	{
	//		std::cout << d << "\n";
	//	}
	//}

	generateTerrain(10, 10, numXVertices, numZVertices, noiseMap);
	Shader terrainShader("../Shaders/basicLighting/basicLighting.vert", "../Shaders/basicLighting/basicLighting.frag");



	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		//Update deltaTime
		updateDeltaTime();
		// input
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render Shapes
		renderTheTerrain(terrainShader);

		//Handle ImGui
		handleImGui();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Terminate ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	return 0;
}