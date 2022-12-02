#version 430
in vec3 v2fColor;

layout( location = 0 ) out vec3 oColor;

void main()
{
	oColor = v2fColor;
}