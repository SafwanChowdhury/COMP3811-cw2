#ifndef SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9

#include <glad.h>

#include <vector>

#include "../vmlib/vec3.hpp"

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
	Materials material;
};



SimpleMeshData concatenate(SimpleMeshData, SimpleMeshData const&);


GLuint create_vao(SimpleMeshData const&);

#endif // SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
