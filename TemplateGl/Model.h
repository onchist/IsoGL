#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL/glew.h>
#include "Shader.h"
#include "Mesh.h"
#include <vector>
#include <string>
#include "utilities.h"


class Model {
public:
	Model();
	Model(GLchar* path) {
		this->loadModel(path);
	}
	void draw(Shader* shader);

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadTextures(aiMaterial* material, aiTextureType type, std::string typeName);
};