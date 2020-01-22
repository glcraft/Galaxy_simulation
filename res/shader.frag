#version 330 core
in vec3 frag_col;
out vec4 outColor;

void main()
{
    outColor = vec4(frag_col, 1.);
}