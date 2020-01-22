#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;

out vec3 frag_col;

uniform mat4 modelmat;
uniform mat4 viewmat;
uniform mat4 projmat;
void main()
{
    gl_Position = projmat * viewmat * modelmat * vec4(pos, 1.);
    frag_col = col;
}