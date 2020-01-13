#ifndef CONTROLS_HPP
#define CONTROLS_HPP
#include <glm/glm.hpp>

namespace io {
	void ComputeMatricesFromInputs();
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::vec3 GetPosition();
	glm::vec3 GetLightPosition();
}
#endif
