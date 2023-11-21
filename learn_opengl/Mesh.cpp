#include "Mesh.h"

Vertex::Vertex()
	:pos(0), normal(0), tex_coord(0)
{
}

Vertex::Vertex(const glm::vec3& pos, const glm::vec3& normal, const glm::vec2& tex_coord)
	:pos(pos), normal(normal), tex_coord(tex_coord)
{
}

Vertex::Vertex(Vertex& v)
	:pos(v.pos), normal(v.normal), tex_coord(v.tex_coord)
{
}

Vertex::Vertex(Vertex&& v) noexcept
	:pos(std::move(v.pos)), normal(std::move(v.normal)), tex_coord(std::move(v.tex_coord))
{
}

Vertex::~Vertex()
{
}

Texture::Texture()
{
}

Texture::Texture(Texture& tex)
	:id(tex.id), type(tex.type)
{
}

Texture::Texture(Texture&& tex) noexcept
	:id(std::move(tex.id)), type(std::move(tex.type))
{
}

Texture::~Texture()
{
}

void Mesh::setupMesh()
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof Vertex, vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, (void*)offsetof(Vertex, pos));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, (void*)offsetof(Vertex, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof Vertex, (void*)offsetof(Vertex, tex_coord));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
	:vertices(vertices), indices(indices), textures(textures)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	setupMesh();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::draw(MShader& shader)
{
	int diffuse_num = 0, specular_num = 0;
	int gl_max_tex_num;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &gl_max_tex_num);
	if (textures.size() > gl_max_tex_num) {
		std::cerr << "have no enough texture units" << std::endl;
		return;
	}

	shader.use();
	unsigned int tex_num = textures.size();
	for (unsigned int i = 0; i < tex_num; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string tex_name = textures[i].type;
		if (tex_name == "diffuse") {
			diffuse_num += 1;
			tex_name += std::to_string(diffuse_num);
		}
		else if (tex_name == "specular")
		{
			specular_num += 1;
			tex_name += std::to_string(specular_num);
		}
		else
		{
			std::cerr << "texture name \"" << tex_name << "\" is not valid" << std::endl;
			continue;
		}

		shader.setInt(("material." + tex_name).c_str(), i); //TODO 改为由函数外部传入material变量名
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	//glActiveTexture(GL_TEXTURE0); 就不改回去

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}
