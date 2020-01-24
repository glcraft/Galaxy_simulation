#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 col;

out vec3 geom_col;

uniform mat4 modelmat;
uniform mat4 viewmat;

void main()
{
    gl_Position = viewmat * modelmat * vec4(pos, 1.);
    geom_col = col;
}