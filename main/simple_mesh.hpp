#ifndef SIMPLE_MESH_HPP
#define SIMPLE_MESH_HPP

#include <glad.h>

#include <vector>
#include <string>
#include <iostream>

#include "../vmlib/vec3.hpp"
#include "../vmlib/vec2.hpp"

#include "stb_image.h"

struct Materials
{
	std::vector<Vec3f> ambient;
	std::vector<Vec3f> diffuse;
	std::vector<Vec3f> specular;
	std::vector<float> shininess;
	std::vector<float> alpha;
};

struct SimpleMeshData
{
	std::vector<Vec3f> positions;
	std::vector<Vec3f> normals;
	std::vector<Vec2f> texcoords;
	Materials material;
};

GLuint load_texture_2d (char const* aPath);

GLuint load_cubemap(std::vector<std::string> faces);




SimpleMeshData concatenate(SimpleMeshData, SimpleMeshData const&);


GLuint create_vao(SimpleMeshData const&);

#endif // SIMPLE_MESH_HPP
