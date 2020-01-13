#pragma once
#include "Vector.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>

class Block;



// Classe d�finissant une �toile

class Star
{

public:

	bool		is_alive;			// Indique si l'�toile est prise en compte
	Vector		previous_position;	// Position d'avant
	Vector		position;			// Position
	Vector		speed;				// Vitesse
	Vector		acceleration;		// Acc�l�ration
	double		mass;				// Masse (en kilogrames)
	double		density;			// Densit� autour de l'�toile
	COLORREF	color;				// Couleur
	int			index;				// Indice
	int			block_index;		// Indice du bloc

	Star();
	Star(const double& speed_initial, const double& area, const double& step, const double& galaxy_thickness);
	Star(const Star& star);

	void operator=(const Star& star);

	void position_maj(const double& step, const bool& verlet_integration);
	void speed_maj(const double& step, const double& area);
	void acceleration_and_density_maj(const double& precision, const std::vector<Star>& galaxy, const std::vector<Block>& blocks);
	void color_maj(const std::vector <Star>& galaxy, const double& zoom, double& area, const std::vector<Block>& blocks);
};

Vector force_and_density_calculation(const double& precision, Star& star, const std::vector<Block>& blocks, int index);
void initialize_galaxy(std::vector<Star>& galaxy, const int& stars_number, const double& area, const double& speed_initial, const double& step, const bool& is_black_hole, const double& black_hole_mass, const double& galaxy_thickness);
