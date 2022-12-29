#version 430
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout( location = 0 ) uniform mat4 uProjection;
layout( location = 1 ) uniform mat4 uView;
layout( location = 2 ) uniform mat4 uModel;

void main()
{
    TexCoords = aPos;
    vec4 pos = uProjection * uView * uModel * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  