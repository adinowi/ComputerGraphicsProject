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
		//45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
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
	GLuint shadowID = shader::LoadShaders("shader/ShadowMappingDepth.vertexshader", "shader/ShadowMappingDepth.fragmentshader");
	glm::vec3 lightPos(-5.0f, 3.0f, -5.0f);

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint modelMatrixID = glGetUniformLocation(programID, "model");
	GLuint viewMatrixID = glGetUniformLocation(programID, "view");
	GLuint projectionMatrixID = glGetUniformLocation(programID, "projection");
	GLuint directionID = glGetUniformLocation(programID, "light.position");
	GLuint viewPosID = glGetUniformLocation(programID, "viewPos");
	GLuint lightAmbientID = glGetUniformLocation(programID, "light.ambient");
	GLuint lightDiffuseID = glGetUniformLocation(programID, "light.diffuse");
	GLuint lightSpecularID = glGetUniformLocation(programID, "light.specular");
	GLuint shininessID = glGetUniformLocation(programID, "shininess");
	GLuint lightSpaceMatrixShadowID = glGetUniformLocation(shadowID, "lightSpaceMatrix");
	GLuint modelShadowID = glGetUniformLocation(shadowID, "model");
	GLuint lightSpaceMatrixProgramID = glGetUniformLocation(programID, "lightSpaceMatrix");
	GLuint shadowMapID = glGetUniformLocation(programID, "shadowMap");

	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 modelMatrix;
	glm::mat4 MVP;




	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//renderer::RectangleRenderer rectangle = renderer::RectangleRenderer("resources/duck.png", 0, 0, 0, 5, 5);
	//Model ourModel = Model("test\\nanosuit.obj");
	renderer::Model parking = renderer::Model("resources/parking.obj");
	//renderer::Model car = renderer::Model("resources/car.obj");
	renderer::CarRenderer carRenderer = renderer::CarRenderer();
	

	initMVP(Projection, View, modelMatrix);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	modelMatrix = glm::mat4(1.0f);
	io::ComputeMatricesFromInputs();
	View = io::GetViewMatrix();
	Projection = io::GetProjectionMatrix();


	glUniformMatrix4fv(modelShadowID, 1, GL_FALSE, &modelMatrix[0][0]);

	do {
		modelMatrix = glm::mat4(1.0f);
		io::ComputeMatricesFromInputs();
		View = io::GetViewMatrix();
		Projection = io::GetProjectionMatrix();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 1.0f, far_plane = 20.0f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		glUseProgram(shadowID);
		glUniformMatrix4fv(lightSpaceMatrixShadowID, 1, GL_FALSE, &lightSpaceMatrix[0][0]);
		glUniformMatrix4fv(modelShadowID, 1, GL_FALSE, &modelMatrix[0][0]);
		//simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		
		//parking.Draw(shadowID);
		//car.Draw(programID);
		carRenderer.Draw(shadowID, modelShadowID);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);







		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);
		glUniformMatrix4fv(lightSpaceMatrixProgramID, 1, GL_FALSE, &lightSpaceMatrix[0][0]);

		// Model matrix : an identity matrix (model will be at the origin)
		
		//ourModel.Draw(programID);
		
		//defineTriangle(vertexbuffer, colorbuffer);

		// Draw 1st triangle
		// Compute MVP matrix
		MVP = Projection * View * modelMatrix;
		// Send our transformation to the currently bound shader, in the "MVP" uniform
		//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glm::vec3 viewPos = io::GetPosition();
		glUniform3fv(directionID, 1, &lightPos[0]);
		glUniform3fv(viewPosID, 1, &viewPos[0]);
		glUniform3f(lightAmbientID, 0.5f, 0.5f, 0.5f);
		glUniform3f(lightDiffuseID, 0.5f, 0.5f, 0.5f);
		glUniform3f(lightSpecularID, 1.0f, 1.0f, 1.0f);
		glUniform1f(shininessID, 4.0f);

		glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, &View[0][0]);
		glUniformMatrix4fv(projectionMatrixID, 1, GL_FALSE, &Projection[0][0]);

		glActiveTexture(GL_TEXTURE10);
		glPrioritizeTextures(10, NULL, 0);
		glUniform1i(shadowMapID, 10);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		// Draw the triangle !
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, SPRING_POINTS_SIZE); // Starting from vertex 0; 3 vertices total -> 1 triangle
		//s.evaluatePoints(time);
		//s.draw(Projection, View, modelMatrix, MatrixID);
		parking.Draw(programID);
		//car.Draw(programID);
		carRenderer.Draw(programID, modelMatrixID);


	
		MVP = Projection * View * modelMatrix;
		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	

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
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

