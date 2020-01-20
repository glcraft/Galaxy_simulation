#pragma once
#include <Utils.h>
#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <list>
#include <cmath>


class Block;
// Classe définissant une étoile

class Star
{

public:
	using container = std::list<Star>;
	struct range{
		container::iterator begin;
		container::iterator end;
	};

	bool		is_alive;			// Indique si l'étoile est prise en compte
	Vector		previous_position;	// Position d'avant
	Vector		position;			// Position
	Vector		speed;				// Vitesse
	Vector		acceleration;		// Accélération
	Float		mass;				// Masse (en kilogrames)
	Float		density;			// Densité autour de l'étoile
	COLORREF	color;				// Couleur
	int			index;				// Indice
	int			block_index;		// Indice du bloc

	Star();
	Star(const Float& speed_initial, const Float& area, const Float& step, const Float& galaxy_thickness);
	Star(const Star& star);

	void operator=(const Star& star);

	void position_maj(const Float& step, const bool& verlet_integration);
	void speed_maj(const Float& step, const Float& area);
	void acceleration_and_density_maj(const Float& precision, const Block& block);
	void color_maj();
};

Vector force_and_density_calculation(const Float& precision, Star& star, const Block& block);
void initialize_galaxy(Star::container& galaxy, const int& stars_number, const Float& area, const Float& speed_initial, const Float& step, const bool& is_black_hole, const Float& black_hole_mass, const Float& galaxy_thickness);

void draw_stars(typename Star::container& galaxy, const Vector& mass_center, const Float& area, const Float& zoom, const View& view);