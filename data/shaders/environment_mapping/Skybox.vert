#version 330 core

in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 localPos;

void main()
{
    localPos = aPos;

    mat4 rotView = mat4(mat3(view)); // Removes translation components from view matrix
    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);

    // Swizzle the z out. Ensure that depth value always ends up at 1.0
    gl_Position = clipPos.xyww;
}