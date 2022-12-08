#ifndef CONE_HPP_CB812C27_5E45_4ED9_9A7F_D66774954C29
#define CONE_HPP_CB812C27_5E45_4ED9_9A7F_D66774954C29

#include <vector>

#include <cstdlib>

#include "simple_mesh.hpp"

#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"

SimpleMeshData make_cone(
	bool aCapped = true,
	std::size_t aSubdivs = 16,
	Vec3f aColor = { 1.f, 1.f, 1.f },
	Mat44f aPreTransform = kIdentity44f
);

#endif // CONE_HPP_CB812C27_5E45_4ED9_9A7F_D66774954C29
