#include <stdlib.h>
#include <stdio.h>

// Include GLEW
#include <GL\glew.h>
#include <GL\GLU.h>

// Include GLFW
#include <GLFW\glfw3.h>

// Include GLM
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\constants.hpp>
#include "..\renderer\model.h"
#include "car.hpp"

using namespace model;

Car::Car()
{
	mPosition = glm::vec3(0, 0, 0);
	mRotation = 0.0f;
}

Car::Car(glm::vec3 tPosition, float tRotation) {
	mPosition = tPosition;
	mRotation = tRotation;
};

Car::Car(glm::vec3 tPosition) {
	mPosition = tPosition;
	mRotation = 0.0f;
};