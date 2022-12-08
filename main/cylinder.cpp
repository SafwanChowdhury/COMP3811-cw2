#include "cylinder.hpp"

SimpleMeshData make_cylinder( bool aCapped, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform )
{
	//TODO: implement me
	std::vector<Vec3f> pos;

	float prevY = std::cos(0.f);
	float prevZ = std::sin(0.f);
	for (std::size_t i = 0; i < aSubdivs; i++) 
	{
		float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;

		float y = std::cos(angle);
		float z = std::sin(angle);
		// Two triangles (= 3*2 positions) create one segment of the cylinder’s shell. 13
		if (aCapped == true) {
			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
			pos.emplace_back(Vec3f{ 0.f, 0.f, 0.f });
			pos.emplace_back(Vec3f{ 0.f, y, z });
		}
		pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
		pos.emplace_back(Vec3f{ 0.f, y, z });
		pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
		pos.emplace_back(Vec3f{ 0.f, y, z });
		pos.emplace_back(Vec3f{ 1.f, y, z });
		pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
		if (aCapped == true) {
			pos.emplace_back(Vec3f{ 1.f, y, z });
			pos.emplace_back(Vec3f{ 1.f, 0.f, 0.f });
			pos.emplace_back(Vec3f{ 1.f, prevY, prevZ });
		}
		prevY = y;
		prevZ = z;
	}
	for(auto & p : pos)
	{
		Vec4f p4{ p.x, p.y, p.z, 1.f };
		Vec4f t = aPreTransform * p4;
		t /= t.w;
		p = Vec3f{ t.x, t.y, t.z };
	}
	std::vector col(pos.size(), aColor);
	return SimpleMeshData{ std::move(pos), std::move(col) };
}
