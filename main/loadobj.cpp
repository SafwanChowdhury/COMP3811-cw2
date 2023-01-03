#include "loadobj.hpp"
#include <rapidobj/rapidobj.hpp>
#include <iostream>

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

            //Vertices
			ret.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
			});

            //Normals
			ret.normals.emplace_back(Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2],
				});
            
            //Texture coordinates
			if (result.attributes.texcoords.size() != 0) {
				ret.texcoords.emplace_back(Vec2f{
					result.attributes.texcoords[idx.texcoord_index * 2 + 0],
					result.attributes.texcoords[idx.texcoord_index * 2 + 1]
			  	});
			}
			else{
			  //std::cout<<aPath<<" doesn't have textures"<<std::endl;
			}
			
            //Material Data
			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];
			ret.material.ambient.emplace_back(Vec3f{
				mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2]
			});

			ret.material.diffuse.emplace_back(Vec3f{
				mat.diffuse[0],
				mat.diffuse[1],
				mat.diffuse[2]
				});

			ret.material.specular.emplace_back(Vec3f{
				mat.specular[0],
				mat.specular[1],
				mat.specular[2]
				});

			ret.material.shininess.emplace_back(float{
				mat.shininess
				});

			ret.material.alpha.emplace_back(float{
				mat.dissolve
				});
		}
	}
	//allow for imported objects to be pretransformed
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

