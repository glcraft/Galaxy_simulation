#pragma once
#include <glm/gtx/quaternion.hpp>
#include <libglw/GLClass.h>
#include <libglw/Shaders.h>
#include "Draw.h"

class DrawGL : public Draw
{
public:
    virtual void init() override;
    virtual void event(SDL_Event* sdlevent) override;
    virtual void update(Star::range alive_galaxy) override;
    virtual void render() override;

    
private:
    gl::ArrayBuffer<glm::vec3> m_VBO;
    gl::sl::Program m_shader;
    glm::mat4 m_viewmat, m_projmat;
    glm::quat m_orientation;
    bool m_mouse_down;
};