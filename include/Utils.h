#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <SDL2/SDL.h>

using Float = double;
template <int N>
using Vector = glm::vec<N, Float>;
using Vector3 = Vector<3>;

template <typename float_t>
constexpr float_t const_pow(float_t x, int y)
{
    float_t res=x;
    for (int i = 1; i < y; i++)
        res *= x;
    return res;
}

constexpr Float LIGHT_YEAR = (9.461 * const_pow<Float>(10, 15));
constexpr Float PI = 3.14159265;
constexpr Float G = 0.00000000006674;
constexpr Float HEIGHT = 1080./4.*3.;
constexpr Float WIDTH = 1920./4.*3.;
constexpr Float SOLAR_MASS = (1.989 * const_pow<Float>(10, 30));
constexpr Float YEAR = 31536000.;


enum View { default_view, xy, xz, yz };		// Vues possibles de la simulation

extern SDL_Renderer* renderer;

int random_int(int min, int max);
Float random_double(Float min, Float max);
// draw_stars in Star.h

// void draw_blocks(const std::vector<Block>& blocks, const Vector3& mass_center, const Float& area, const Float& zoom, const View& view);
Vector3 create_spherical(Float radius, Float phi, Float theta);
Float get_phi(Vector3 vector);
Float get_theta(Vector3 vector);
Float get_phi(Vector3 point_1, Vector3 point_2);
Float get_theta(Vector3 point_1, Vector3 point_2);

constexpr uint32_t RGB(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 16) + (g << 8) + b;
}
template <int index>
constexpr uint32_t GetRGBValue(uint32_t rgb)
{
    return (rgb >> (index*8))&0xFF;
}