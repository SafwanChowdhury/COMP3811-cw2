#ifndef LOADOBJ_HPP
#define LOADOBJ_HPP

#include "simple_mesh.hpp"
#include "../vmlib/mat33.hpp"
#include "../vmlib/mat44.hpp"


SimpleMeshData load_wavefront_obj( char const* aPath, Mat44f aPreTransform);

#endif // LOADOBJ_HPP
