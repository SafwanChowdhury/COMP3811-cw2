#ifndef FRUST_HPP
#define FRUST_HPP

#include <vector>

#include <cstdlib>

#include "simple_mesh.hpp"

#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"


SimpleMeshData make_frust(
	float radius = 0.4,
	std::size_t aSubdivs = 16,
	Vec3f aColor = { 1.f, 1.f, 1.f },
	Mat44f aPreTransform = kIdentity44f
);

#endif
