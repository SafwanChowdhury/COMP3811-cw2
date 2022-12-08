#ifndef CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B
#define CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B

// This defines the vertex data for a colored unit cube.

constexpr float const kCubePositions[] = {
	+1.f, +1.f, -1.f,
	-1.f, +1.f, -1.f,
	-1.f, +1.f, +1.f,
	+1.f, +1.f, -1.f,
	-1.f, +1.f, +1.f,
	+1.f, +1.f, +1.f,

	+1.f, -1.f, +1.f,
	+1.f, +1.f, +1.f,
	-1.f, +1.f, +1.f,
	+1.f, -1.f, +1.f,
	-1.f, +1.f, +1.f,
	-1.f, -1.f, +1.f,

	-1.f, -1.f, +1.f,
	-1.f, +1.f, +1.f,
	-1.f, +1.f, -1.f,
	-1.f, -1.f, +1.f,
	-1.f, +1.f, -1.f,
	-1.f, -1.f, -1.f,

	-1.f, -1.f, -1.f,
	+1.f, -1.f, -1.f,
	+1.f, -1.f, +1.f,
	-1.f, -1.f, -1.f,
	+1.f, -1.f, +1.f,
	-1.f, -1.f, +1.f,

	+1.f, -1.f, -1.f,
	+1.f, +1.f, -1.f,
	+1.f, +1.f, +1.f,
	+1.f, -1.f, -1.f,
	+1.f, +1.f, +1.f,
	+1.f, -1.f, +1.f,

	-1.f, -1.f, -1.f,
	-1.f, +1.f, -1.f,
	+1.f, +1.f, -1.f,
	-1.f, -1.f, -1.f,
	+1.f, +1.f, -1.f,
	+1.f, -1.f, -1.f,
};

constexpr float const kCubeColors[] = {
	+1.f, +0.f, +0.f,
	+1.f, +0.f, +0.f,
	+1.f, +0.f, +0.f,
	+1.f, +0.f, +0.f,
	+1.f, +0.f, +0.f,
	+1.f, +0.f, +0.f,

	+0.f, +1.f, +0.f,
	+0.f, +1.f, +0.f,
	+0.f, +1.f, +0.f,
	+0.f, +1.f, +0.f,
	+0.f, +1.f, +0.f,
	+0.f, +1.f, +0.f,

	+0.f, +0.f, +1.f,
	+0.f, +0.f, +1.f,
	+0.f, +0.f, +1.f,
	+0.f, +0.f, +1.f,
	+0.f, +0.f, +1.f,
	+0.f, +0.f, +1.f,

	+1.f, +0.f, +1.f,
	+1.f, +0.f, +1.f,
	+1.f, +0.f, +1.f,
	+1.f, +0.f, +1.f,
	+1.f, +0.f, +1.f,
	+1.f, +0.f, +1.f,

	+1.f, +1.f, +0.f,
	+1.f, +1.f, +0.f,
	+1.f, +1.f, +0.f,
	+1.f, +1.f, +0.f,
	+1.f, +1.f, +0.f,
	+1.f, +1.f, +0.f,

	+0.f, +1.f, +1.f,
	+0.f, +1.f, +1.f,
	+0.f, +1.f, +1.f,
	+0.f, +1.f, +1.f,
	+0.f, +1.f, +1.f,
	+0.f, +1.f, +1.f
};

static_assert(sizeof(kCubeColors) == sizeof(kCubePositions),
	"Size of cube colors and cube positions do not match. Both are 3D vectors."
	);

#endif // CUBE_HPP_6874B39C_112D_4D34_BD85_AB81A730955B
