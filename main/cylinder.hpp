#ifndef CYLINDER_HPP_E4D1E8EC_6CDA_4800_ABDD_264F643AF5DB
#define CYLINDER_HPP_E4D1E8EC_6CDA_4800_ABDD_264F643AF5DB

#include <vector>

#include <cstdlib>

#include "simple_mesh.hpp"

#include "../vmlib/vec3.hpp"
#include "../vmlib/mat44.hpp"


SimpleMeshData make_cylinder(
	bool aCapped = true,
	std::size_t aSubdivs = 16,
	Vec3f aColor = { 1.f, 1.f, 1.f },
	Mat44f aPreTransform = kIdentity44f
);

#endif // CYLINDER_HPP_E4D1E8EC_6CDA_4800_ABDD_264F643AF5DB
