#pragma once
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/color_space.hpp>
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
    
    template <int N>
    void update(typename Star<N>::range alive_galaxy)
    {
        if (m_VBO.size() == 0)
        {
            m_VBO.reserve(std::distance(alive_galaxy.begin, alive_galaxy.end));
        }

        auto verts = m_VBO.map_write();
        auto v0 = verts;
        for (auto itStar = alive_galaxy.begin; itStar != alive_galaxy.end; ++itStar)
        {
            verts->pos = convPos(itStar->position / LIGHT_YEAR);
            verts->col = glm::rgbColor(glm::vec3(itStar->density / (3.), 1., 1.));
            ++verts;
        }
        auto totalPoints = verts - v0;
        m_VBO.reserve(totalPoints);
        m_VBO.unmap();
    }
    virtual void render() override;

    void setWindow(_std::observer_ptr<SDL_Window> win)
    { m_window = win; }
private:
    template <int N>
    glm::vec3 convPos(Vector<N> v)
    {
        glm::vec3(v[0], v[1], v[2]);
    }
    template <>
    glm::vec3 convPos<3>(Vector<3> v)
    {
        return v;
    }
    template <>
    glm::vec3 convPos<2>(Vector<2> v)
    {
        return glm::vec3(v.x, v.y, 0.f);
    }
    template <>
    glm::vec3 convPos<1>(Vector<1> v)
    {
        return glm::vec3(v.x, 0.f, 0.f);
    }
    gl::ArrayBuffer<Vertex> m_VBO;
    gl::sl::Program m_shader;
    _std::observer_ptr<SDL_Window> m_window;
    glm::mat4 m_viewmat, m_projmat;
    glm::quat m_orientation;
    bool m_mouse_down;
};