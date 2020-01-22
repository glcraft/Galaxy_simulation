#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 viewmat;
uniform mat4 projmat;
void main()
{
    gl_Position = projmat * viewmat * vec4(pos, 1.);
}