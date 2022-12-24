#include "cube.hpp"
#include "../vmlib/mat33.hpp"

SimpleMeshData make_cube(float x, Vec3f aColor, Mat44f aPreTransform) {
	
	std::vector<Vec3f> pos;
	std::vector<Vec3f> normal;

	pos.emplace_back(Vec3f{+x, +x, -x});
	normal.emplace_back(Vec3f{ 0.f, +1.f, 0.f });
	pos.emplace_back(Vec3f{-x, +x, -x});
	normal.emplace_back(Vec3f{ 0.f, +1.f, 0.f });
	pos.emplace_back(Vec3f{-x, +x, +x});
	normal.emplace_back(Vec3f{ 0.f, +1.f, 0.f });
	pos.emplace_back(Vec3f{+x, +x, -x});
	normal.emplace_back(Vec3f{ 0.f, +1.f, 0.f });
	pos.emplace_back(Vec3f{-x, +x, +x});
	normal.emplace_back(Vec3f{ 0.f, +1.f, 0.f });
	pos.emplace_back(Vec3f{+x, +x, +x});
	normal.emplace_back(Vec3f{ 0.f, +1.f, 0.f });

	pos.emplace_back(Vec3f{+x, -x, +x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, +1.f });
	pos.emplace_back(Vec3f{+x, +x, +x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, +1.f });
	pos.emplace_back(Vec3f{-x, +x, +x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, +1.f });
	pos.emplace_back(Vec3f{+x, -x, +x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, +1.f });
	pos.emplace_back(Vec3f{-x, +x, +x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, +1.f });
	pos.emplace_back(Vec3f{-x, -x, +x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, +1.f });

	pos.emplace_back(Vec3f{-x, -x, +x});
	normal.emplace_back(Vec3f{ -1.f, 0.f, 0.f });
	pos.emplace_back(Vec3f{-x, +x, +x});
	normal.emplace_back(Vec3f{ -1.f, 0.f, 0.f });
	pos.emplace_back(Vec3f{-x, +x, -x});
	normal.emplace_back(Vec3f{ -1.f, 0.f, 0.f });
	pos.emplace_back(Vec3f{-x, -x, +x});
	normal.emplace_back(Vec3f{ -1.f, 0.f, 0.f });
	pos.emplace_back(Vec3f{-x, +x, -x});
	normal.emplace_back(Vec3f{ -1.f, 0.f, 0.f });
	pos.emplace_back(Vec3f{-x, -x, -x});
	normal.emplace_back(Vec3f{ -1.f, 0.f, 0.f });
	
	pos.emplace_back(Vec3f{-x, -x, -x});
	normal.emplace_back(Vec3f{ 0.f, -1.f, 0.f });
	pos.emplace_back(Vec3f{+x, -x, -x});
	normal.emplace_back(Vec3f{ 0.f, -1.f, 0.f });
	pos.emplace_back(Vec3f{+x, -x, +x});
	normal.emplace_back(Vec3f{ 0.f, -1.f, 0.f });
	pos.emplace_back(Vec3f{-x, -x, -x});
	normal.emplace_back(Vec3f{ 0.f, -1.f, 0.f });
	pos.emplace_back(Vec3f{+x, -x, +x});
	normal.emplace_back(Vec3f{ 0.f, -1.f, 0.f });
	pos.emplace_back(Vec3f{-x, -x, +x});
	normal.emplace_back(Vec3f{ 0.f, -1.f, 0.f });

	pos.emplace_back(Vec3f{+x, -x, -x});
	normal.emplace_back(Vec3f{ +1.f, 0.f, 0.f });
	pos.emplace_back(Vec3f{+x, +x, -x});
	normal.emplace_back(Vec3f{ +1.f, 0.f, 0.f });
	pos.emplace_back(Vec3f{+x, +x, +x}); 
	normal.emplace_back(Vec3f{ +1.f, 0.f, 0.f });
	pos.emplace_back(Vec3f{+x, -x, -x});
	normal.emplace_back(Vec3f{ +1.f, 0.f, 0.f });
	pos.emplace_back(Vec3f{+x, +x, +x});
	normal.emplace_back(Vec3f{ +1.f, 0.f, 0.f });
	pos.emplace_back(Vec3f{+x, -x, +x});
	normal.emplace_back(Vec3f{ +1.f, 0.f, 0.f });

	pos.emplace_back(Vec3f{-x, -x, -x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, -1.f });
	pos.emplace_back(Vec3f{-x, +x, -x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, -1.f });
	pos.emplace_back(Vec3f{+x, +x, -x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, -1.f });
	pos.emplace_back(Vec3f{-x, -x, -x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, -1.f });
	pos.emplace_back(Vec3f{+x, +x, -x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, -1.f });
	pos.emplace_back(Vec3f{+x, -x, -x});
	normal.emplace_back(Vec3f{ 0.f, 0.f, -1.f });

	Mat33f const N = mat44_to_mat33(transpose(invert(aPreTransform)));

	for (auto& p : pos)
	{
		Vec4f p4{ p.x, p.y, p.z, 1.f };
		Vec4f t = aPreTransform * p4;
		t /= t.w;
		p = Vec3f{ t.x, t.y, t.z };
	}

	for (auto& n : normal)
	{
		Vec3f n4{ n.x, n.y, n.z };
		Vec3f t = N * n4;
		t = normalize(t);
		n = Vec3f{ t.x, t.y, t.z };
	}

	std::vector col(pos.size(), aColor);
	return SimpleMeshData{ std::move(pos), std::move(col), std::move(normal) };
}