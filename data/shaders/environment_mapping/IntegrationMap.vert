#version 330 core

layout(location = 0) in vec3 vPositionIn;
layout(location = 2) in vec2 vTexCoordIn;

out vec2 texCoordV;

void main()
{
    texCoordV =  TexCoordIn;
    gl_Position = vPositionIn;
}