#ifndef CAR_RENDERER_HPP
#define CAR_RENDERER_HPP

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GL\GLU.h>

#include "..\model\car.hpp"
#include "model.h"
namespace renderer {
	class CarRenderer
	{
	public:
		model::Car mCar;
		const char* CAR_MODEL_PATH = "resources/car.obj";

		CarRenderer();
		CarRenderer(model::Car tCar);
		void Draw(GLuint shaderId, GLuint modelId);
		
	private:
		renderer::Model mCarModel;
	};
}
#endif // !CAR_RENDERER_HPP
