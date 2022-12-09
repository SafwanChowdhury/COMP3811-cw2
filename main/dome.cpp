#include "dome.hpp"

SimpleMeshData make_dome(std::size_t aSubdivs, Vec3f aColor, Mat44f aPreTransform)
{

	std::vector<Vec3f> pos;
	float inner_radius = 0;
	float prevX = 0;

	for (float i = 0; i < 1; i++) {
		float inner_radius = 1-(i / 360);
		float prevY = std::cos(0.f) * (1 - inner_radius); //need to fix the dome so that spokes are not coming out instead it must be like cylinder
		float prevZ = std::sin(0.f) * (1 - inner_radius);
		float x = sqrt(pow(1,2) - pow(inner_radius, 2));
		for (std::size_t i = 0; i < aSubdivs; i++)
		{
			float const angle = (i + 1) / float(aSubdivs) * 2.f * 3.1415926f;
			float y = std::cos(angle);
			float z = std::sin(angle);
			float y1 = y * (1 - inner_radius);
			float z1 = z * (1 - inner_radius);
			float yNew = y / (1 / inner_radius);
			float zNew = z / (1 / inner_radius);
			float nextYNew = std::cos((1) / float(aSubdivs) * 2.f * 3.1415926f) / (1 / inner_radius);
			float nextZNew = std::sin((1) / float(aSubdivs) * 2.f * 3.1415926f) / (1 / inner_radius);
			if (i < aSubdivs - 1) {
				nextYNew = std::cos((i + 2) / float(aSubdivs) * 2.f * 3.1415926f) / (1 / inner_radius);
				nextZNew = std::sin((i + 2) / float(aSubdivs) * 2.f * 3.1415926f) / (1 / inner_radius);
			}
			// Two triangles (= 3*2 positions) create one segment of the cylinder’s shell. 13

			pos.emplace_back(Vec3f{ prevX, prevY, prevZ });
			pos.emplace_back(Vec3f{ prevX, y1, z1 });
			pos.emplace_back(Vec3f{ x, yNew, zNew });
			//pos.emplace_back(Vec3f{ prevX, y1, z1 });
			//pos.emplace_back(Vec3f{ x, yNew, zNew });
			//pos.emplace_back(Vec3f{ x, nextYNew, nextZNew });

			prevY = y1;
			prevZ = z1;
		}
		prevX = x;

	}

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
