#pragma once
#include <glm/gtx/quaternion.hpp>
#include <libglw/GLClass.h>
#include <libglw/Shaders.h>
#include "mymemory.hpp"
#include "Draw.h"
struct Vertex
{
    glm::vec3 pos, col;
};

class DrawGL : public Draw
{
public:
    virtual void init() override;
    virtual void event(SDL_Event* sdlevent) override;
    virtual void update(Star::range alive_galaxy) override;
    virtual void render() override;

    void setWindow(_std::observer_ptr<SDL_Window> win)
    { m_window = win; }
private:
    gl::ArrayBuffer<Vertex> m_VBO;
    gl::sl::Program m_shader;
    _std::observer_ptr<SDL_Window> m_window;
    glm::mat4 m_viewmat, m_projmat;
    glm::quat m_orientation;
    bool m_mouse_down;
};