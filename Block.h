#pragma once
#include "Vector.h"
#include "Star.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <time.h>



// Classe d�finissant une zone de l'algorithme de Barnes�Hut

class Block
{

public:

	bool				as_stars;		// Pr�sence d'�toiles dans le bloc
	std::vector<int>	stars;			// Les indices des �toiles pr�sentes dans le bloc
	Vector				mass_center;	// Centre de gravit� du bloc
	int					index;			// Indice du bloc
	bool				as_children;	// Pr�sence de blocs enfants
	bool				as_parents;		// Pr�sence de blocs parents
	std::vector<int>	parents;		// Indice des blocs parents
	std::vector<int>	children;		// Indice des blocs enfants
	Vector				position;		// Position du bloc
	double				mass;			// Masse contenue dans le bloc (en kilogrames)
	double				size;			// Taille du bloc (en m�tres)
	
	Block();
	Block(const Block& block);

	void operator=(const Block& block);

	void stars_maj(std::vector<Star>& galaxy, std::vector<Block>& blocks);
	void mass_center_and_mass_maj(const std::vector<Star>& galaxy);
	void divide(int& index_value, std::vector<Star>& galaxy, std::vector<Block>& blocks, std::vector<Block>& blocks_temp);
};

bool is_in(const Block& block, const Star& star);
void initialise_blocks(int& index_value, const double& area, std::vector<Star>& galaxy, std::vector<Block>& blocks);
void create_blocks(const double& area, std::vector<Block>& blocks, std::vector<Star>& galaxy, std::vector<Block>& blocks_temp);
