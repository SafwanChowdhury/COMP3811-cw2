#ifndef CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B
#define CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B

// This defines the vertex data for a colored unit cube.


#include <vector>

#include <cstdlib>

#include "simple_mesh.hpp"

#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"


SimpleMeshData make_cube(
	float x = 1,
	Vec3f aColor = { 1.f, 1.f, 1.f },
	Vec3f aDiffuse = { 1.0f, 0.5f, 0.31f },
	Vec3f aSpec = { 0.5f,0.5f,0.5f },
	float aShininess = 32.0f,
	float aAlpha = 1.f,
	Mat44f aPreTransform = kIdentity44f
);

SimpleMeshData make_cube_tex(
	float x = 1,
	Vec3f aColor = { 1.f, 1.f, 1.f },
	Vec3f aDiffuse = { 1.0f, 0.5f, 0.31f },
	Vec3f aSpec = { 0.5f,0.5f,0.5f },
	float aShininess = 32.0f,
	float aAlpha = 1.f,
	Mat44f aPreTransform = kIdentity44f
);


#endif // CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B
