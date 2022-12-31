#include "screenshot.hpp"

void saveScreenshot()
{
	GLint viewport[4];
	char filepath[50];

	glGetIntegerv(GL_VIEWPORT, viewport);
	int width = viewport[2];
	int height = viewport[3];

	char* data = (char*)malloc((size_t)(width * height * 3));
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	stbi_flip_vertically_on_write(1);
	const char* filename = getScreenshotName();
	strcpy(filepath, "screenshots/");
	strcat(filepath, filename);
	stbi_write_png(filepath, width, height, 3, data, 0);
	
	free(data);

}

const char* getScreenshotName() {

	static char basename[30];

	time_t t = time(NULL);
	strftime(basename, 30, "%Y%m%d_%H%M%S.png", localtime(&t));

	return basename;
}