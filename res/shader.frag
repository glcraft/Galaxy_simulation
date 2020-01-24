#version 330 core
in vec3 frag_col;
in vec2 frag_uv;
out vec4 outColor;

void main()
{
    outColor = vec4(frag_col, 0.2*clamp(1-length(frag_uv), 0., 1.));
}