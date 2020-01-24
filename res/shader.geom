#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
in vec3 geom_col[]; // Sortie du vertex shader, pour chaque sommet
out vec3 frag_col;

uniform mat4 projmat;

void set_vertex(vec2 pos)
{
    gl_Position = projmat * (gl_in[0].gl_Position + vec4(pos*1., 0.0, 0.0));
    
    EmitVertex();
}
void main()
{
    frag_col=geom_col[0];
    set_vertex(vec2(-1,-1));
    set_vertex(vec2(1,-1));
    set_vertex(vec2(-1,1));
    set_vertex(vec2(1,1));

    EndPrimitive();
}