#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL\GLU.h>

namespace shader {
	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
}
#endif