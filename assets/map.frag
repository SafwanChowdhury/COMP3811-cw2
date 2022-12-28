#version 430

in vec3 texCoords;

layout (binding = 0) uniform samplerCube skybox;

out vec4 fragColor;

void main()
{

	fragColor = texture(skybox, texCoords);
	
}

