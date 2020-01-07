// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL\glew.h>
#include <GL\GLU.h>

// Include GLFW
#include <GLFW\glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\constants.hpp>
using namespace glm;

#include "shader\shader.hpp"
#include <vector>

#include "io\controls.hpp"

#include "renderer\model.h"
#include "utils\utils.h"
#include "renderer\car_renderer.hpp"


static int SPRING_POINTS_SIZE;
static const int SCREEN_WIDTH = 1024;
static const int SCREEN_HEIGHT = 768;


int init() {
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tutorial 02 - Red triangle", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

void initMVP(glm::mat4 &P, glm::mat4 &V, glm::mat4 &M) {
	// Projection matrix : 
		//45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	P = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 1000.0f);
	// Or, for an ortho camera :
	//P = glm::ortho(-3.0f, 3.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// View/Camera matrix
	V = glm::lookAt(
		glm::vec3(-20,0, 7), // Camera is at (0, 0, 1), in World Space
		glm::vec3(0, 0, 7), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

	// Model matrix : an identity matrix (model will be at the origin)
	M = glm::mat4(1.0f);
}



int main(void)
{
	if (init() == -1) {
		return -1;
	}

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Create a Vertex Array Object and set it as the current one
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	// Create and compile our GLSL program from the shaders
	GLuint programID = shader::LoadShaders("shader/SimpleVertexShader.vertexshader", "shader/SimpleFragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint modelMatrixID = glGetUniformLocation(programID, "model");
	GLuint viewMatrixID = glGetUniformLocation(programID, "view");
	GLuint projectionlMatrixID = glGetUniformLocation(programID, "projection");

	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 modelMatrix;
	glm::mat4 MVP;

	//renderer::RectangleRenderer rectangle = renderer::RectangleRenderer("resources/duck.png", 0, 0, 0, 5, 5);
	//Model ourModel = Model("test\\nanosuit.obj");
	renderer::Model parking = renderer::Model("resources/parking.obj");
	//renderer::Model car = renderer::Model("resources/car.obj");
	renderer::CarRenderer carRenderer = renderer::CarRenderer();
	

	initMVP(Projection, View, modelMatrix);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLuint vertexbuffer;
	GLuint colorbuffer;
	//defineSpring(vertexbuffer, colorbuffer);
	glPointSize(1.0f);

	
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	/*GLuint texAttrib = Utils::loadTGA_glfw("wood.png");
	GLuint steelAttrib = Utils::loadTGA_glfw("steel2.png");
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texAttrib);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, steelAttrib);*/


	
	//glEnableVertexAttribArray(texAttrib);
	//glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	float time = 0.0f;
	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Model matrix : an identity matrix (model will be at the origin)
		modelMatrix = glm::mat4(1.0f);
		io::ComputeMatricesFromInputs();
		View = io::GetViewMatrix();
		Projection = io::GetProjectionMatrix();
		//ourModel.Draw(programID);
		parking.Draw(programID);
		//car.Draw(programID);
		carRenderer.Draw(programID, modelMatrixID);

		glUniform1i(TextureID, 1);
		//defineTriangle(vertexbuffer, colorbuffer);

		// Draw 1st triangle
		// Compute MVP matrix
		MVP = Projection * View * modelMatrix;
		// Send our transformation to the currently bound shader, in the "MVP" uniform
		//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(projectionlMatrixID, 1, GL_FALSE, &Projection[0][0]);
		// Draw the triangle !
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, SPRING_POINTS_SIZE); // Starting from vertex 0; 3 vertices total -> 1 triangle
		//s.evaluatePoints(time);
		//s.draw(Projection, View, modelMatrix, MatrixID);

		glUniform1i(TextureID, 0);

	
		MVP = Projection * View * modelMatrix;
		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		

		time += 0.01f;

		// Draw 2nd triangl
		// Change model position
		/*
		Model = glm::translate(Model, glm::vec3(-2.0f, -1.0f, 0.0f));
		//compute MVP matrix
		MVP = Projection * View * Model;
		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle

		*/
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Cleanup VBO
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

