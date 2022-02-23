#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//My headers
#include "progen/Utilities.h"
#include "progen/Shader.h"
#include "progen/Camera.h"


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


//Time parameters
double deltaTime = 0.0;
double lastFrame = 0.0;

//Window
GLFWwindow* window;


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

	return 0;
}



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

int main()
{
	setup();

	GLuint VAO = testRectangle();
	Shader testShader("../Shaders/solidColor/solidColor.vert", "../Shaders/solidColor/solidColor.frag");

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
		renderTestRectangle(VAO, testShader);


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	return 0;
}