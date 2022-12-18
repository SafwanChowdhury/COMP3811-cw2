
#include <typeinfo>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"


int main() {
	Mat44f newMat1 = {
	1.f, 0.f, 1.f, 0.f,
	0.f, 1.f, 2.f, 0.f,
	0.f, 0.f, 3.f, 0.f,
	0.f, 0.f, 4.f, 1.f };
	Mat44f newMat2 = {
	0.f, 0.f, 0.f, 1.f,
	0.f, 0.f, 2.f, 0.f,
	0.f, 3.f, 0.f, 0.f,
	4.f, 0.f, 0.f, 0.f };

	Mat44f testMat{};

	testMat = make_rotation_z(0);

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; j++)
		{
			printf(" %0.f ", testMat(i, j));
		}
		printf("\n");
	}
}