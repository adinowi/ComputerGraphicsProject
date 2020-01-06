#include <stdlib.h>
#include <vector>
#include <glm/gtc/constants.hpp>
#include <GL\glew.h>
#include <GL\GLU.h>

// Include GLFW
#include <GLFW\glfw3.h>
#include <soil.h>

namespace Utils {
		void draw(std::vector<float> interleavedVertices, std::vector<int> indices) {
			GLuint vboId;
			glGenBuffers(1, &vboId);
			glBindBuffer(GL_ARRAY_BUFFER, vboId);           // for vertex data
			glBufferData(GL_ARRAY_BUFFER,                   // target
				interleavedVertices.size() * sizeof(float), // data size, # of bytes
				&interleavedVertices[0],   // ptr to vertex data
				GL_STATIC_DRAW);                   // usage

			// copy index data to VBO
			GLuint iboId;
			glGenBuffers(1, &iboId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);   // for index data
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
				indices.size() * sizeof(int),             // data size, # of bytes
				&indices[0],               // ptr to index data
				GL_STATIC_DRAW);                   // usage


			// bind VBOs
			glBindBuffer(GL_ARRAY_BUFFER, vboId);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

			// activate attrib arrays
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);

			// set attrib arrays with stride and offset
			int stride = 20;
			glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
			glVertexAttribPointer(1, 2, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));


			// draw a sphere with VBO
			glDrawElements(GL_TRIANGLES,                    // primitive type
				indices.size(),          // # of indices
				GL_UNSIGNED_INT,                 // data type
				(void*)0);                       // offset to indices

			// deactivate attrib arrays
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

			// delete VBOs
			glDeleteBuffers(1, &vboId);
			glDeleteBuffers(1, &iboId);
		}

		GLuint loadTGA_glfw(const char* path) {

			// Create one OpenGL texture
			GLuint textureID;
			glGenTextures(1, &textureID);

			// "Bind" the newly created texture : all future texture functions will modify this texture
			glBindTexture(GL_TEXTURE_2D, textureID);


			int width, height;
			unsigned char* image =
				SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, image);
			SOIL_free_image_data(image);

			// Nice trilinear filtering.

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);

			// Return the ID of the texture we just created
			return textureID;
		}
};