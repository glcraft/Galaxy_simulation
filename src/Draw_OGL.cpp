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
    m_viewmat = glm::lookAt(glm::vec3(400,0,0), glm::vec3(0), glm::vec3(0,0,1));
    m_orientation = glm::quat(1,0,0,0);
}
void DrawGL::render()
{
    m_shader
        << gl::sl::use
        << gl::UniformRef("projmat", m_projmat)
        << gl::UniformRef("viewmat", m_viewmat)
        << gl::UniformStatic("modelmat", glm::toMat4(m_orientation));
    m_VBO.draw(GL_POINTS);
}
void DrawGL::event(SDL_Event* sdlevent)
{
    if (sdlevent->type==SDL_MOUSEBUTTONDOWN)
    {
        m_mouse_down=true;
    }
    else if (sdlevent->type==SDL_MOUSEBUTTONUP)
    {
        m_mouse_down=false;
    }
    else if (m_mouse_down && sdlevent->type==SDL_MOUSEMOTION)
    {
        glm::vec3 _axisx(glm::rotate(m_orientation, glm::vec3(0,0,1)));
        glm::vec3 _axisy(glm::rotate(m_orientation, glm::vec3(0,1,0)));
        glm::quat q = glm::angleAxis<float>(static_cast<float>(sdlevent->motion.xrel)/100.f, glm::vec3(0,0,1));
        q = glm::angleAxis<float>(static_cast<float>(sdlevent->motion.yrel)/100.f, glm::vec3(0,1,0)) * q;
        m_orientation=q*m_orientation;
    }
}
void DrawGL::update(Star::range alive_galaxy)
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
