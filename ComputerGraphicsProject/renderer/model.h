#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL\GLU.h>


#include <string>
#include <vector>
#include "mesh.h"
#include "..\utils\utils.h"

namespace renderer {
	class Model
	{
	public:
		/*  Model Data */
		std::vector<renderer::Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
		std::vector<renderer::Mesh> meshes;
		std::string directory;
		bool gammaCorrection;


		Model();
		/*  Functions   */
		// constructor, expects a filepath to a 3D model.
		Model(std::string const& path, bool gamma = false);

		// draws the model, and thus all its meshes
		void Draw(GLuint shaderId);


	private:
		/*  Functions   */
		// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
		void loadModel(std::string const& path);

		// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
		void processNode(aiNode* node, const aiScene* scene);

		renderer::Mesh processMesh(aiMesh* mesh, const aiScene* scene);

		// checks all material textures of a given type and loads the textures if they're not loaded yet.
		// the required info is returned as a Texture struct.
		std::vector<renderer::Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	};
}
#endif // !MODEL_H


