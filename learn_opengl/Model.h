#pragma once
#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include <unordered_map>

class Model
{
public:
	Model(const char* path);
	~Model() = default;

	void draw(MShader& shader);
private:
	std::string dir; //模型所在的父级文件夹
	std::vector<Mesh> meshes;
	std::unordered_map<std::string, Texture> tex_loaded; //遍历mesh过程中已保存的纹理路径和纹理对象

	bool loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	int textureFromFile(std::string model_path) const;
};
#endif // !MODEL_H
