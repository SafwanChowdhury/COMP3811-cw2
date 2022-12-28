#include <typeinfo>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <filesystem>

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"

void printMat44(Mat44f inMat) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%f ", inMat(i, j));
		}
		printf("\n");
	}
}

void printVec4(Vec4f inVec) {
	for (int j = 0; j < 4; j++) {
		printf("%f ", inVec[j]);
	}
	printf("\n");
}

void mat44Test(Mat44f testMatA, Mat44f testMatB, Vec4f testVec4, float angleDeg, Vec3f translation, Vec3f scaling) {
	float angleRad = angleDeg / (180.f / 3.141592f);
	printf("mat*mat: \n\n");
	printMat44(testMatA * testMatB);
	printf("\nmat*vec: \n\n");
	printVec4(testMatA * testVec4);
	printf("\nmat+mat: \n\n");
	printMat44(testMatA + testMatB);
	printf("\nx rotate: %f \n\n", angleRad);
	printMat44(make_rotation_x(angleRad));
	printf("\ny rotate: %f \n\n", angleRad);
	printMat44(make_rotation_y(angleRad));
	printf("\nz rotate: %f \n\n", angleRad);
	printMat44(make_rotation_z(angleRad));
	printf("\ntranslate: %f %f %f \n\n", translation.x, translation.y, translation.z);
	printMat44(make_translation(translation));
	printf("\nscaling: %f %f %f \n\n", scaling.x, scaling.y, scaling.z);
	printMat44(make_scaling(scaling.x,scaling.y,scaling.z));
	printf("\nperspective: \n\n");
	printMat44(make_perspective_projection(
		60.f * 3.1415926f / 180.f,
		float(1200) / float(720),
		0.1f, 100.0f
	));
}


int main() {
	Mat44f Mat4A = { 10.f,5.f,3.f,3.f,
					  5.f,6.f,1.f,2.f,
					  1.f,7.f,8.f,4.f,
					  9.f,3.f,5.f,7.f
	};
	Mat44f Mat4B = { 2.f,2.f,4.f,8.f,
					  1.f,5.f,8.f,5.f,
					  4.f,7.f,7.f,0.f,
					  9.f,5.f,1.f,6.f
	};
	Vec4f Vec4A = { 0.f,6.f,2.f,7.f };
	float angle = 45.f;
	Vec3f T = { 3.f,5.f,7.f };
	Vec3f S = { 0.2f, 0.5f, 0.7f };
	mat44Test(Mat4A, Mat4B, Vec4A, angle, T, S);
}