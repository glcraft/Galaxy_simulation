#pragma once
#include "Utils.h"
#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>


class Block;
// Classe définissant une étoile

class Star
{

public:

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
	void acceleration_and_density_maj(const Float& precision, const std::vector<Star>& galaxy, const std::vector<Block>& blocks);
	void color_maj(const std::vector <Star>& galaxy, const Float& zoom, Float& area, const std::vector<Block>& blocks);
};

Vector force_and_density_calculation(const Float& precision, Star& star, const std::vector<Block>& blocks, int index);
void initialize_galaxy(std::vector<Star>& galaxy, const int& stars_number, const Float& area, const Float& speed_initial, const Float& step, const bool& is_black_hole, const Float& black_hole_mass, const Float& galaxy_thickness);
