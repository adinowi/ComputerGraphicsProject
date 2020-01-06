#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <GL\GLU.h>

namespace Utils {
	void draw(std::vector<float> interleavedVertices, std::vector<int> indices);
	GLuint loadTGA_glfw(const char* path);
}
#endif