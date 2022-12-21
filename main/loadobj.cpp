#include "loadobj.hpp"
#include <rapidobj/rapidobj.hpp>

#include "../support/error.hpp"

SimpleMeshData load_wavefront_obj( char const* aPath, Mat44f aPreTransform)
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

			ret.normals.emplace_back(Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2],
				});

			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];
			ret.colors.emplace_back(Vec3f{
				mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2]
			});
		}
	}
	Mat33f const N = mat44_to_mat33(transpose(invert(aPreTransform)));

	for (auto& p : ret.positions)
	{
		Vec4f p4{ p.x, p.y, p.z, 1.f };
		Vec4f t = aPreTransform * p4;
		t /= t.w;
		p = Vec3f{ t.x, t.y, t.z };
	}

	for (auto& n : ret.normals)
	{
		Vec3f n4{ n.x, n.y, n.z };
		Vec3f t = N * n4;
		t = normalize(t);
		n = Vec3f{ t.x, t.y, t.z };
	}

	return ret;

}

