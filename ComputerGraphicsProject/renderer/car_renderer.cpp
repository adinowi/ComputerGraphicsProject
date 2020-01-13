#include <stdlib.h>
#include "car_renderer.hpp"
#include "..\model\car.hpp"

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GL\GLU.h>

using namespace renderer;

CarRenderer::CarRenderer() {
	mCar = model::Car(glm::vec3(0,0,0));
	mCarModel = renderer::Model(CAR_MODEL_PATH);
}

CarRenderer::CarRenderer(model::Car tCar) : mCar(tCar) {
	mCarModel = renderer::Model(CAR_MODEL_PATH);
}

CarRenderer::CarRenderer(const char* path) {
	mCar = model::Car(glm::vec3(0, 0, 0));
	mCarModel = renderer::Model(path);
}


void CarRenderer::Draw(GLuint shaderId, GLuint modelId, glm::mat4 tModel) {
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, mCar.mPosition);
	model = glm::rotate(model, mCar.mRotation, glm::vec3(0, 1.0f, 0));
	glUniformMatrix4fv(modelId, 1, GL_FALSE, &tModel[0][0]);

	mCarModel.Draw(shaderId);
}
