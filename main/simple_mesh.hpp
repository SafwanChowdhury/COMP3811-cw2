#ifndef SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9

#include <glad.h>

#include <vector>

#include "../vmlib/vec3.hpp"
#include "../vmlib/vec2.hpp"

#include "stb_image.h"

struct SimpleMeshData
{
	std::vector<Vec3f> positions;
	std::vector<Vec3f> colors;
	std::vector<Vec3f> normals;
        std::vector<Vec2f> texcoords;
};

SimpleMeshData concatenate(SimpleMeshData, SimpleMeshData const&);

GLuint load_texture_2d (char const* aPath);


GLuint create_vao(SimpleMeshData const&);

#endif // SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
