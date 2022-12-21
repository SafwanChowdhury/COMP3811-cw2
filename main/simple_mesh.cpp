#include "simple_mesh.hpp"

SimpleMeshData concatenate(SimpleMeshData aM, SimpleMeshData const& aN)
{
	aM.positions.insert(aM.positions.end(), aN.positions.begin(), aN.positions.end());
	aM.colors.insert(aM.colors.end(), aN.colors.begin(), aN.colors.end());
	aM.normals.insert(aM.normals.end(), aN.normals.begin(), aN.normals.end());
	return aM;
}


GLuint create_vao(SimpleMeshData const& aMeshData)
{
	//TODO: implement me
	GLuint position = 0;
	glGenBuffers(1, &position);
	glBindBuffer(GL_ARRAY_BUFFER, position);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.positions.size() * sizeof(Vec3f), aMeshData.positions.data(), GL_STATIC_DRAW);

	GLuint colors = 0;
	glGenBuffers(1, &colors);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.colors.size() * sizeof(Vec3f), aMeshData.colors.data(), GL_STATIC_DRAW);

	GLuint normals = 0;
	glGenBuffers(1, &normals);
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.normals.size() * sizeof(Vec3f), aMeshData.normals.data(), GL_STATIC_DRAW);

	//texture
	GLuint texcoords = 0;
	glGenBuffers(1, &texcoords);
	glBindBuffer(GL_ARRAY_BUFFER, texcoords);
	glBufferData(GL_ARRAY_BUFFER, aMeshData.texcoords.size() * sizeof(Vec2f), aMeshData.texcoords.data(), GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, position);

	glVertexAttribPointer(
		0, // location = 0 in vertex shader
		3, GL_FLOAT, GL_FALSE,
		0, // stride = 0 indicates that there is no padding between inputs
		0 // data starts at offset 0 in the VBO
	);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, colors);

	glVertexAttribPointer(
		1, // location = 1 in vertex shader
		3, GL_FLOAT, GL_FALSE,
		0, // see above
		0
	);

	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, normals);

	glVertexAttribPointer(
		2, // location = 0 in vertex shader
		3, GL_FLOAT, GL_FALSE,
		0, // stride = 0 indicates that there is no padding between inputs
		0 // data starts at offset 0 in the VBO
	);
	glEnableVertexAttribArray(2);

	//texture
	glBindBuffer(GL_ARRAY_BUFFER, texcoords);

	glVertexAttribPointer(
		3, // location = 0 in vertex shader
		2, GL_FLOAT, GL_FALSE,
		0, // stride = 0 indicates that there is no padding between inputs
		0 // data starts at offset 0 in the VBO
	);
	glEnableVertexAttribArray(3);
	
	

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &position);
	glDeleteBuffers(1, &colors);
	glDeleteBuffers(1, &normals);

	return vao;
}

//Added for textures

GLuint load_texture_2d (char const* aPath) {
  assert(aPath);

  stbi_set_flip_vertically_on_load(true);

  int w, h, channels;

  stbi_uc* ptr = stbi_load(aPath, &w, &h, &channels, 4);

  if(!ptr)
    return 0;

  GLuint tex = 0;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);
  
  stbi_image_free(ptr);

  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 6.f);


  return tex;
}

