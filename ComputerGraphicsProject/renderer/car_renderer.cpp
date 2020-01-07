#include <stdlib.h>
#include "car_renderer.hpp"
#include "..\model\car.hpp"

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GL\GLU.h>

using namespace renderer;

CarRenderer::CarRenderer() {
	mCar = model::Car(glm::vec3(0,1.4f,0));
	mCarModel = renderer::Model(CAR_MODEL_PATH);
}

CarRenderer::CarRenderer(model::Car tCar) : mCar(tCar) {
	mCarModel = renderer::Model(CAR_MODEL_PATH);
};

void CarRenderer::Draw(GLuint shaderId, GLuint modelId) {
	
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, mCar.mPosition);
	glUniformMatrix4fv(modelId, 1, GL_FALSE, &model[0][0]);

	mCarModel.Draw(shaderId);
}
