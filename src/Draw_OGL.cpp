#include "Draw_OGL.h"
#include <libglw/Shaders.h>
#include <memory>
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#ifdef WIN32
#include <Windows.h>
#endif

void DrawGL::init() 
{
    glewExperimental = true;
    auto res = glewInit();
    if (res!=GLEW_OK)
        throw std::runtime_error("GLEW Initialization error");
    gl::Object::SetAutoInstantiate(true);
    m_VBO.instantiate();
    auto vao=std::make_shared<gl::VertexArray>();
    m_VBO.attachVertexArray(vao);
    m_VBO.set_attrib(decltype(m_VBO)::Attrib<0>(offsetof(Vertex, pos)));
    m_VBO.set_attrib(decltype(m_VBO)::Attrib<1>(offsetof(Vertex, col)));
    try
    {
        m_shader 
            << gl::sl::Shader<gl::sl::Vertex>("res/shader.vert")
            << gl::sl::Shader<gl::sl::Geometry>("res/shader.geom")
            << gl::sl::Shader<gl::sl::Fragment>("res/shader.frag") 
            << gl::sl::link;
    }
    catch(const gl::sl::CompileException& e)
    {
#ifdef WIN32
        MessageBox(nullptr, e.what(), "Erreur de compilation", MB_OK);
#else
        std::cerr << e.what() << std::endl;
#endif
        exit(1);
    }
    
    
    m_projmat = glm::perspective<float>(70.f, WIDTH/HEIGHT, 0.1f, 1000.f);
    m_viewmat = glm::lookAt(glm::vec3(400,0,0), glm::vec3(0), glm::vec3(0,0,1));
    m_orientation = glm::quat(1,0,0,0);
    m_mouse_down=false;
    glPointSize(3.f);
    glEnable(GL_DEPTH_TEST);
}
void DrawGL::render()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    m_shader
        << gl::sl::use
        << gl::UniformRef("projmat", m_projmat)
        << gl::UniformRef("viewmat", m_viewmat)
        << gl::UniformStatic("modelmat", glm::toMat4(m_orientation));
    m_VBO.draw(GL_POINTS);

    SDL_GL_SwapWindow(m_window.get());
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
    auto v0 = verts;
    for(auto itStar = alive_galaxy.begin; itStar != alive_galaxy.end; ++itStar)
    {
        verts->pos = itStar->position/LIGHT_YEAR;
        verts->col = glm::rgbColor(glm::vec3(itStar->density / (3. ), 1., 1.));
        ++verts;
    }
    auto totalPoints = verts - v0;
    m_VBO.reserve(totalPoints);
    m_VBO.unmap();
}
