#pragma once
#include <glm/glm.hpp>
#include <Utils.h>
#include <Star.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <variant>
#include <cmath>
#include <time.h>



// Classe définissant une zone de l'algorithme de Barnes-Hut

class Block
{

public:

	bool				as_stars;		// Présence d'étoiles dans le bloc
	bool				as_children;	// Présence de blocs enfants
	bool				as_parents;		// Présence de blocs parents
	int					parent;			// Indice des blocs parents
	Vector				position;		// Position du bloc
	Float				mass;			// Masse contenue dans le bloc (en kilogrames)
	Vector				mass_center;	// Centre de gravité du bloc
	size_t 				nb_stars;		// Nombre d'étoile contenu dans le block
	Float				size, halfsize;	// Taille du bloc (en mètres)
	std::variant<Star::container::iterator, std::vector<Block>> contains;
	
	Block();
	Block(const Block& block);

	Block& operator=(const Block& block);

	// void stars_maj(Star::range& stars, std::vector<Block>& blocks);
	void mass_center_and_mass_maj(const Star::range& stars);
	void divide(Star::range galaxy);
	void setSize(Float size);
};

bool is_in(const Block& block, const Star& star);
void initialise_blocks(int& index_value, const Float& area, Star::container& galaxy, std::vector<Block>& blocks);
void create_blocks(const Float& area, Block& block, Star::container& galaxy);
