#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <chrono>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>
using Float = double;
using Vector = glm::tvec3<Float>;

class Star;
class Block;

#define LIGHT_YEAR	(9.461 * pow(10, 15))	// Année lumière (en mètres)
#define PI			3.14159265				// Pi
#define G			0.00000000006674		// Constante gravitationnelle (en newton mètre carré par kilogramme carré)
#define HEIGHT		1080.					// Hauteur de la fenêtre (en pixels)
#define WIDTH		1920.					// Largeur de la fenêtre (en pixels)
#define SOLAR_MASS	(1.989 * pow(10, 30))	// Masse solaire (en kilogrammes)
#define YEAR		31536000.				// Année (en secondes)



enum View { default_view, xy, xz, yz };		// Vues possibles de la simulation

extern SDL_Renderer* renderer;

int random_int(const int& min, const int& max);
Float random_double(const Float& min, const Float& max);
void draw_stars(std::vector<Star>& galaxy, const Vector& mass_center, const Float& area, const Float& zoom, const View& view);
void draw_blocks(const std::vector<Block>& blocks, const Vector& mass_center, const Float& area, const Float& zoom, const View& view);
Vector create_spherical(const Float& radius, const Float& phi, const Float& theta);
Float get_phi(Vector vector);
Float get_theta(Vector vector);
Float get_phi(Vector point_1, Vector point_2);
Float get_theta(Vector point_1, Vector point_2);