#pragma once
#ifndef MESH_H
#define MESH_H

#include <glm.hpp>
#include <string>
#include <vector>
#include "MShader.h"

class Vertex
{
public:
	Vertex();
	Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& tex_coord);
	Vertex(Vertex& v);
	Vertex(Vertex&& v) noexcept;
	~Vertex();

public:
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 tex_coord;
};


class Texture
{
public:
	Texture();
	Texture(Texture& tex);
	Texture(Texture&& tex) noexcept;
	~Texture();

public:
	unsigned int id = 0;
	std::string type; //漫反射贴图或者镜面光贴图, "diffuse"或"specular"
};


class Mesh
{
private:
	unsigned int VAO = 0;
	unsigned int VBO = 0;
	unsigned int EBO = 0;

	void setupMesh();

public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures);
	~Mesh();

	void draw(MShader& shader);
};
#endif // !MESH_H
