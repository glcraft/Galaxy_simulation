#pragma once
#include <glm/glm.hpp>
#include "Utils.h"
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



// Classe définissant une zone de l'algorithme de Barnes-Hut

class Block
{

public:

	bool				as_stars;		// Présence d'étoiles dans le bloc
	std::vector<int>	stars;			// Les indices des étoiles présentes dans le bloc
	Vector				mass_center;	// Centre de gravité du bloc
	int					index;			// Indice du bloc
	bool				as_children;	// Présence de blocs enfants
	bool				as_parents;		// Présence de blocs parents
	std::vector<int>	parents;		// Indice des blocs parents
	int					children[8];		// Indice des blocs enfants
	Vector				position;		// Position du bloc
	Float				mass;			// Masse contenue dans le bloc (en kilogrames)
	Float				size;			// Taille du bloc (en mètres)
	
	Block();
	Block(const Block& block);

	void operator=(Block block);

	void stars_maj(std::vector<Star>& galaxy, std::vector<Block>& blocks);
	void mass_center_and_mass_maj(const std::vector<Star>& galaxy);
	void divide(int& index_value, std::vector<Star>& galaxy, std::vector<Block>& blocks, std::vector<Block>& blocks_temp);
};

bool is_in(const Block& block, const Star& star);
void initialise_blocks(int& index_value, const Float& area, std::vector<Star>& galaxy, std::vector<Block>& blocks);
void create_blocks(const Float& area, std::vector<Block>& blocks, std::vector<Star>& galaxy, std::vector<Block>& blocks_temp);
