#pragma once
#include <glm/gtx/quaternion.hpp>
#include <libglw/GLClass.h>
#include <libglw/Shaders.h>
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

    
private:
    gl::ArrayBuffer<Vertex> m_VBO;
    gl::sl::Program m_shader;
    glm::mat4 m_viewmat, m_projmat;
    glm::quat m_orientation;
    bool m_mouse_down;
};