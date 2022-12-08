#include "loadobj.hpp"

#include <rapidobj/rapidobj.hpp>

#include "../support/error.hpp"

SimpleMeshData load_wavefront_obj( char const* aPath )
{
	auto result = rapidobj::ParseFile(aPath);
	if (result.error)
		throw Error("Unable to load OBJ file ’%s’: %s", aPath, result.error.code.message().c_str());
	rapidobj::Triangulate(result);

	SimpleMeshData ret;
	for (auto const& shape : result.shapes)
	{
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];
			ret.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
			});
			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];
			ret.colors.emplace_back(Vec3f{
				mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2]
			});
		}
	}

	return ret;

}

