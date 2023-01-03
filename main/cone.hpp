#ifndef CONE_HPP
#define CONE_HPP

#include <vector>

#include <cstdlib>

#include "simple_mesh.hpp"

#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"

SimpleMeshData make_cone(
	bool aCapped = true,
	std::size_t aSubdivs = 16,
	Vec3f aColor = { 1.f, 1.f, 1.f },
	Vec3f aDiffuse = { 1.0f, 0.5f, 0.31f },
	Vec3f aSpec = { 0.5f,0.5f,0.5f },
	float aShininess = 32.0f,
	float aAlpha = 1.f,
	Mat44f aPreTransform = kIdentity44f
);

#endif // CONE_HPP
