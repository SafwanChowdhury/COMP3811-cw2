#ifndef CUBEMAP_HPP
#define CUBEMAP_HPP

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <string>
#include <vector>

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"

#include "../support/program.hpp"



class Cubemap{
public: 
	Cubemap(); 

	void loadTextures(std::string dir,
		std::string right = "right.png",
		std::string left = "left.png",
		std::string top = "top.png",
		std::string bottom = "bottom.png",
		std::string front = "front.png",
		std::string back = "back.png");

	void init();

	void render(ShaderProgram shader);

	void cleanup();

private:

	unsigned int id;
	std::string dir;
	std::vector<std::string> faces;
	bool hasTextures;


	GLuint VAO = 0;

};



#endif