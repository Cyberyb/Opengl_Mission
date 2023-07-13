#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 cameraSpaceMat;
uniform mat4 model;
uniform mat4 perspective;
void main()
{
    gl_Position = perspective * cameraSpaceMat *  model * vec4(aPos, 1.0);
}