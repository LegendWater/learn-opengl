#include "Model.h"
#include <stb_image.h>

Model::Model(const char* path)
{
	if (!loadModel(path)) {
		throw std::runtime_error("");
	}
}

void Model::draw(MShader& shader)
{
	for (Mesh& mesh : meshes) {
		mesh.draw(shader);
	}
}

bool Model::loadModel(std::string path)
{
	//解析得到dir
	auto slash_pos1 = path.find_last_of('/');
	auto slash_pos2 = path.find_last_of('\\');
	auto slash_pos3 = path.find_last_of("\\\\");
	if (slash_pos1 == std::string::npos && slash_pos2 == std::string::npos && slash_pos3 == std::string::npos) {
		std::cerr << "invalid model path" << std::endl;
		return false;
	}
	dir = path.substr(0, std::max(slash_pos1 == std::string::npos ? 0 : slash_pos1, std::min(slash_pos2, slash_pos3)));

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "model " << path << " read failed" << std::endl;
		return false;
	}

	processNode(scene->mRootNode, scene);
	return true;
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	//处理每个结点的mesh
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		auto& mesh_ptr = scene->mMeshes[node->mMeshes[i]];
		auto mesh = processMesh(mesh_ptr, scene);
		meshes.push_back(mesh);
	}

	//递归处理结点的所有子结点
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);
	std::vector<unsigned int> indices;
	indices.reserve(mesh->mNumFaces * 3); //暂时认为索引的数量是这么多
	std::vector<Texture> textures;

	//Vertex
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex v;
		//顶点
		v.pos.x = mesh->mVertices[i].x;
		v.pos.y = mesh->mVertices[i].y;
		v.pos.z = mesh->mVertices[i].z;
		//法线
		if (mesh->HasNormals()) {
			v.normal.x = mesh->mNormals[i].x;
			v.normal.y = mesh->mNormals[i].y;
			v.normal.z = mesh->mNormals[i].z;
		}
		//纹理坐标
		if (mesh->mTextureCoords[0]) {
			v.tex_coord.x = mesh->mTextureCoords[0][i].x;
			v.tex_coord.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			v.tex_coord = glm::vec2(0.f);
		}

		vertices.emplace_back(v);
	}
	//index
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		auto& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.emplace_back(face.mIndices[j]);
		}
	}
	//texture
	if (mesh->mMaterialIndex >= 0) {
		auto material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuse = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse");
		std::vector<Texture> specular = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular");
		textures.insert(textures.cend(), diffuse.begin(), diffuse.end());
		textures.insert(textures.cend(), specular.begin(), specular.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0, count = mat->GetTextureCount(type); i < count; i++)
	{
		aiString path;
		mat->GetTexture(type, i, &path);
		auto finded = tex_loaded.find(path.C_Str());
		if (finded == tex_loaded.end()) { //纹理没有被加载过
			Texture tex;
			tex.id = textureFromFile(path.C_Str());
			if (tex.id < 0) { //创建纹理失败
				continue;
			}
			tex.type = typeName;
			textures.push_back(tex);
			tex_loaded.insert({ std::string(path.C_Str()), tex });
		}
		else { //纹理已被加载过
			textures.push_back(finded->second);
		}
	}

	return textures;
}

int Model::textureFromFile(std::string model_path) const
{
	if (model_path.empty()) {
		std::cerr << "cannot generate texture from file: model path is empty" << std::endl;
		return -1;
	}

	std::string whole_path = dir + '/' + model_path;
	int width, height, channel;
	stbi_set_flip_vertically_on_load(0);
	unsigned char* data = stbi_load(whole_path.c_str(), &width, &height, &channel, 0);
	if (data) {
		GLuint texid;
		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR/*_MIPMAP_LINEAR*/);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		GLint format;
		switch (channel)
		{
		case 1: format = GL_RED; break;
		case 2: format = GL_RG; break;
		case 4: format = GL_RGBA; break;
		case 3:
		default: format = GL_RGB; break;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		CHECK_GL_ERROR();

		stbi_image_free(data);
		return texid;
	}
	return -1;
}
