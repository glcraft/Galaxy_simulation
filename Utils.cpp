#include "Block.h"
#include "Utils.h"
#include <glm/glm.hpp>
#include "Star.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include "SDL.h"



// Donne un int al�atoire entre deux bornes

int random_int(const int& min, const int& max)
{
	return rand() % (max - min) + min;
}



// Donne un Float al�atoire entre deux bornes

Float random_double(const Float& min, const Float& max)
{
	return (Float(rand()) / Float(RAND_MAX)) * (max - min) + min;
}



// Affiche les �toiles de la galaxie

void draw_stars(std::vector<Star>& galaxy, const Vector& mass_center, const Float& area, const Float& zoom, const View& view)
{
	Float x;
	Float y;
	Float z;
	Vector screen_position;

	Vector camera = Vector(0., area / 2., area / 2.);

	Float  coef = 1. / (area / zoom);

	for (int i = 0; i < galaxy.size(); i++)
	{
		if (galaxy[i].is_alive)
		{
			switch (view)
			{
			case default_view:
				x = (galaxy[i].position - mass_center).x;
				y = (galaxy[i].position - mass_center).y / 3. - (galaxy[i].position - mass_center).z / 1.5;

				screen_position = create_spherical(glm::length(Vector(x, y, 0.)) / (glm::distance(galaxy[i].position, camera)), get_phi(Vector(x, y, 0.)), get_theta(Vector(x, y, 0.)));

				x = screen_position.x * zoom + WIDTH / 2.;
				y = screen_position.y * zoom + HEIGHT / 2.;
				break;
			case xy:
				x = (galaxy[i].position - mass_center).x * coef + WIDTH / 2.;
				y = (galaxy[i].position - mass_center).y * coef + HEIGHT / 2.;
				break;
			case xz:
				x = (galaxy[i].position - mass_center).x * coef + WIDTH / 2.;
				y = (galaxy[i].position - mass_center).z * coef + HEIGHT / 2.;
				break;
			case yz:
				x = (galaxy[i].position - mass_center).y * coef + WIDTH / 2.;
				y = (galaxy[i].position - mass_center).z * coef + HEIGHT / 2.;
				break;
			}

			SDL_SetRenderDrawColor(renderer, GetRValue(galaxy[i].color), GetGValue(galaxy[i].color), GetBValue(galaxy[i].color), SDL_ALPHA_OPAQUE);

			// SDL_RenderDrawPoint(renderer, x - 1, y);
			// SDL_RenderDrawPoint(renderer, x, y - 1);
			// SDL_RenderDrawPoint(renderer, x, y + 1);
			// SDL_RenderDrawPoint(renderer, x + 1, y);
			SDL_RenderDrawPoint(renderer, x, y);

			// SDL_SetRenderDrawColor(renderer, GetRValue(galaxy[i].color), GetGValue(galaxy[i].color), GetBValue(galaxy[i].color), SDL_ALPHA_OPAQUE / 2.);

			// SDL_RenderDrawPoint(renderer, x - 1, y - 1);
			// SDL_RenderDrawPoint(renderer, x - 1, y + 1);
			// SDL_RenderDrawPoint(renderer, x + 1, y - 1);
			// SDL_RenderDrawPoint(renderer, x + 1, y + 1);
		}
	}
}



// Affiche les blocs (ne fonctionne pas en vue "delault_view")

void draw_blocks(const std::vector<Block>& blocks, const Vector& mass_center, const Float& area, const Float& zoom, const View& view)
{
	if (view == default_view)
		return;

	SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

	int x;
	int y;
	int block_size;

	for (int i = 0; i < blocks.size(); i++)
	{
		if (view == default_view)
		{
			x = ((blocks[i].position - mass_center).x - blocks[i].size / 2.) / (area / zoom) + WIDTH / 2.;
			y = (((blocks[i].position - mass_center).y - blocks[i].size / 2.) / (area / zoom)) / 2. - (((blocks[i].position - mass_center).z - blocks[i].size / 2.) / (area / zoom)) / 2. + HEIGHT / 2.;
		}

		if (view == xy)
		{
			x = ((blocks[i].position - mass_center).x - blocks[i].size / 2.) / (area / zoom) + WIDTH / 2.;
			y = ((blocks[i].position - mass_center).y - blocks[i].size / 2.) / (area / zoom) + HEIGHT / 2.;
		}

		if (view == xz)
		{
			x = ((blocks[i].position - mass_center).x - blocks[i].size / 2.) / (area / zoom) + WIDTH / 2.;
			y = ((blocks[i].position - mass_center).z - blocks[i].size / 2.) / (area / zoom) + HEIGHT / 2.;
		}

		if (view == yz)
		{
			x = ((blocks[i].position - mass_center).y - blocks[i].size / 2.) / (area / zoom) + WIDTH / 2.;
			y = ((blocks[i].position - mass_center).z - blocks[i].size / 2.) / (area / zoom) + HEIGHT / 2.;
		}

		block_size = blocks[i].size / (area / zoom);

		for (int j = 0; j < block_size; j++)
		{
			SDL_RenderDrawPoint(renderer, x, y + j);
			SDL_RenderDrawPoint(renderer, x + block_size, y + j);
		}

		for (int j = 0; j < block_size; j++)
		{
			SDL_RenderDrawPoint(renderer, x + j, y);
			SDL_RenderDrawPoint(renderer, x + j, y + block_size);
		}
	}
}

// Donne la valeur cartésienne x à partir des coordonnées sphériques (en mètres)

Float get_x(const Float& radius, const Float& phi, const Float& theta)
{
	return glm::cos(phi) * glm::sin(theta) * radius;
}



// Donne la valeur cartésienne y à partir des coordonnées sphériques (en mètres)

Float get_y(const Float& radius, const Float& phi, const Float& theta)
{
	return glm::sin(phi) * glm::sin(theta) * radius;
}



// Donne la valeur cartésienne z à partir des coordonnées sphériques (en mètres)

Float get_z(const Float& radius, const Float& phi, const Float& theta)
{
	return glm::cos(theta) * radius;
}

// Construit un vecteur à partir de ses coordonnées sphériques

Vector create_spherical(const Float& radius, const Float& phi, const Float& theta)
{
	return Vector(get_x(radius, phi, theta), get_y(radius, phi, theta), get_z(radius, phi, theta));
}

// Donne l'angle phi du vecteur (en radiants)

Float get_phi(Vector vector)
{
	vector.z = 0.;

	if (vector.y > 0)
		return glm::acos(vector.x / glm::length(vector));

	if (vector.y < 0)
		return 2 * PI - acos(vector.x / glm::length(vector));
}



// Donne l'angle theta du vecteur (en radiants)

Float get_theta(Vector vector)
{
	return glm::acos(vector.z / glm::length(vector));
}

Float get_phi(Vector point_1, Vector point_2)
{
	return get_phi(point_1 - point_2);
}
Float get_theta(Vector point_1, Vector point_2)
{
	return get_theta(point_1 - point_2);
}