#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <SDL2/SDL.h>
using Float = double;
using Vector = glm::tvec3<Float>;
template <typename float_t>
constexpr float_t const_pow(float_t x, int y)
{
    float_t res=x;
    for (int i = 1; i < y; i++)
        res *= x;
    return res;
}

class Star;
class Block;

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

// void draw_blocks(const std::vector<Block>& blocks, const Vector& mass_center, const Float& area, const Float& zoom, const View& view);
Vector create_spherical(const Float& radius, const Float& phi, const Float& theta);
Float get_phi(Vector vector);
Float get_theta(Vector vector);
Float get_phi(Vector point_1, Vector point_2);
Float get_theta(Vector point_1, Vector point_2);

constexpr uint32_t RGB(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 16) + (g << 8) + b;
}
template <int index>
constexpr uint32_t GetRGBValue(uint32_t rgb)
{
    return (rgb >> (index*8))&0xFF;
}