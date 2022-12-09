#include "frustum.hpp"

SimpleMeshData make_frust(float inner_radius, std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform)
{

	std::vector<Vec3f> pos;
	float prevX = std::cos(0.f);
	float prevY = std::cos(0.f);
	float prevZ = std::sin(0.f);
	//for (int i = 0; i < 1; i++) {
		for (std::size_t i = 0; i < aSubdivs; i++)
		{
			float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
			float x = std::cos(angle);
			float y = std::cos(angle);
			float z = std::sin(angle);
			float yNew = y / (1 / inner_radius);
			float zNew = z / (1 / inner_radius);
			float nextYNew = std::cos((1) / float(aSubdivs) * 2.f * 3.1415926f) / (1 / inner_radius);
			float nextZNew = std::sin((1) / float(aSubdivs) * 2.f * 3.1415926f) / (1 / inner_radius);
			if (i < aSubdivs - 1) {
				nextYNew = std::cos((i + 2) / float(aSubdivs) * 2.f * 3.1415926f) / (1 / inner_radius);
				nextZNew = std::sin((i + 2) / float(aSubdivs) * 2.f * 3.1415926f) / (1 / inner_radius);
			}
			// Two triangles (= 3*2 positions) create one segment of the cylinder’s shell. 13

			pos.emplace_back(Vec3f{ 0.f, prevY, prevZ });
			pos.emplace_back(Vec3f{ 0.f, y, z });
			pos.emplace_back(Vec3f{ 1.f, yNew, zNew });
			pos.emplace_back(Vec3f{ 0.f, y, z});
			pos.emplace_back(Vec3f{ 1.f, yNew, zNew });
			pos.emplace_back(Vec3f{ 1.f, nextYNew, nextZNew });
			
			
			prevX = x;
			prevY = y;
			prevZ = z;
		}
	//}
	
	for (auto& p : pos)
	{
		Vec4f p4{ p.x, p.y, p.z, 1.f };
		Vec4f t = aPreTransform * p4;
		t /= t.w;
		p = Vec3f{ t.x, t.y, t.z };
	}
	std::vector col(pos.size(), aColor);
	return SimpleMeshData{ std::move(pos), std::move(col) };
}
