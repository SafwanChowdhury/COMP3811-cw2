#version 430

layout (location = 0) in vec3 aPos;

layout (location = 1) uniform mat4 projection;
layout (location = 2) uniform mat4 view;

out vec3 texCoords;



void main()
{
	texCoords = vec3(aPos.x, aPos.y, -aPos.z);
	vec4 pos = projection * view * vec4(aPos, 1.0);
	gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
        
}

