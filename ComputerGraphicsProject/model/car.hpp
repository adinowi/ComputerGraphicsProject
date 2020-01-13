#ifndef CAR_HPP
#define CAR_HPP

#include <glm\glm.hpp>

namespace model {
	class Car
	{
	public:
		Car();
		Car(glm::vec3 tPosition, float tRotation);
		Car(glm::vec3 tPosition);
		void move(float x, float y, float z);
		void rotate(float angle);

		glm::vec3 mPosition;
		float mRotation;
	private:
	};
}

#endif // !CAR_HPP
