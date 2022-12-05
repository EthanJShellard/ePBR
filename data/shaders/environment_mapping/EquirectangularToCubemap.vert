#version 330 core

in vec3 aPos;

out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    localPos = aPos;
    glPosition = projection * view * vec4(localPos, 1.0);
}