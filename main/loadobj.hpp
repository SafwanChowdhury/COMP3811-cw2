#ifndef LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F
#define LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F

#include "simple_mesh.hpp"
#include "../vmlib/mat33.hpp"
#include "../vmlib/mat44.hpp"


SimpleMeshData load_wavefront_obj( char const* aPath, Mat44f aPreTransform);

#endif // LOADOBJ_HPP_2CF735BE_6624_413E_B6DC_B5BBA337F96F
