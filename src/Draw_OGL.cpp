#include "Draw_OGL.h"
#include <memory>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>

void DrawGL::init() 
{
    glewExperimental = true;
    auto res = glewInit();
    if (res!=GLEW_OK)
        throw std::runtime_error("GLEW Initialization error");

    auto vao=std::make_shared<gl::VertexArray>();
    m_VBO.instantiate();
    m_VBO.attachVertexArray(vao);
    m_VBO.set_attrib(decltype(m_VBO)::Attrib<0>(0));

    m_shader 
        << gl::sl::Shader<gl::sl::Vertex>("res/shader.vert")
        << gl::sl::Shader<gl::sl::Fragment>("res/shader.frag") 
        << gl::sl::link;
    m_projmat = glm::perspective<float>(70.f, WIDTH/HEIGHT, 0.1f, 1000.f);
    m_viewmat = glm::lookAt(glm::vec3(100), glm::vec3(0), glm::vec3(0,1,0));
}
void DrawGL::render()
{
    m_shader
        << gl::sl::use
        << gl::UniformRef("projmat", m_projmat)
        << gl::UniformRef("viewmat", m_viewmat);
    m_VBO.draw(GL_POINTS);
}
void DrawGL::update(Star::range alive_galaxy, SDL_Event* sdlevent)
{
    if (m_VBO.size()==0)
    {
        m_VBO.reserve(std::distance(alive_galaxy.begin, alive_galaxy.end));
    }

    auto verts = m_VBO.map_write();
    for(auto itStar = alive_galaxy.begin; itStar != alive_galaxy.end; ++itStar)
    {
        *verts = itStar->position/LIGHT_YEAR;
        ++verts;
    }
    m_VBO.unmap();
}
