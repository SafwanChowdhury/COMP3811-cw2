#ifndef SCREENSHOT_HPP
#define SCREENSHOT_HPP

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stb_image_write.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

void saveScreenshot();

const char* getScreenshotName();

#endif // !SCREENSHOT_HPP

