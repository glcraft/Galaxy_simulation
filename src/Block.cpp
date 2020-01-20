#include <Block.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <Star.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <functional>
#include <cmath>
#include <time.h>

std::array<Star::range, 8> set_octree(Star::range stars, glm::vec3 pivot)
{
	std::array<std::function<bool(const Star& star)>, 3> testStarAxis = {
		[pivot](const Star& star){return star.position.x<pivot.x;},
		[pivot](const Star& star){return star.position.y<pivot.y;},
		[pivot](const Star& star){return star.position.z<pivot.z;}
	};
    std::array<Star::range, 8> result;
	int iPart=0;
    Star::container::iterator itX = std::partition(stars.begin, stars.end, testStarAxis[0]);
    auto xParts = std::array{Star::range{stars.begin, itX}, Star::range{itX, stars.end}};
    for(auto& part : xParts)
    {
        Star::container::iterator itY = std::partition(part.begin, part.end, testStarAxis[1]);
		auto yParts = std::array{Star::range{part.begin, itY}, Star::range{itY, part.end}};
        
        for(auto& part : yParts)
        {
        	Star::container::iterator itZ = std::partition(part.begin, part.end, testStarAxis[2]);
			result[iPart++] = Star::range{part.begin, itZ};
			result[iPart++] = Star::range{itZ, part.end};
        }
    }
    return result;
}

// Construit un bloc

Block::Block()
{
	as_stars = false;
	mass_center = Vector(0., 0., 0.);
	as_children = false;
	as_parents = false;
	position = Vector(0., 0., 0.);
	mass = 0.;
	size = halfsize = 0.;
	nb_stars = 0;
}



// Construit un bloc à partir d'un autre bloc

Block::Block(const Block& block)
{
	operator=(block);
}



// Assignation

Block& Block::operator=(const Block& block)
{
	as_stars = block.as_stars;
	mass_center = block.mass_center;
	as_children = block.as_children;
	as_parents = block.as_parents;
	parent = block.parent;
	position = block.position;
	mass = block.mass;
	size = block.size;
	halfsize = block.halfsize;
	contains = block.contains;
	nb_stars = block.nb_stars;
	return *this;
}



// Met à jour les étoiles contenues dans le bloc

// void Block::stars_maj(Star::range& stars, std::vector<Block>& blocks)
// {
// 	for (int i = 0; i < blocks[parent].stars.size(); i++)
// 	{
// 		if (is_in(*this, galaxy[blocks[parent].stars[i]]))
// 		{
// 			stars.push_back(blocks[parent].stars[i]);
// 			galaxy[blocks[parent].stars[i]].block_index = index;

// 			if (!(as_stars))
// 				as_stars = true;
// 		}
// 	}
// }



// Met à jour le centre de gravité et la masse du bloc

void Block::mass_center_and_mass_maj(const Star::range& galaxy)
{
	mass_center = Vector(0., 0., 0.);
	mass = 0.;

	for (auto it = galaxy.begin; it!=galaxy.end; ++it)
	{
		mass_center += it->position * it->mass;
		mass += it->mass;
	}
	mass_center = mass_center / mass;
}



// Divise un bloc en 8 plus petits

void Block::divide(Star::range stars)
{
	
	if (stars.begin==stars.end) // pas d'etoile
	{
		contains = stars.begin; // pas très utile, permet de clear la memoire de array<Block, 8> si c'était sa valeur précédente
		nb_stars = 0;
		as_children = false;
	}
	else if (std::next(stars.begin)==stars.end) // une étoile
	{
		contains = stars.begin;
		nb_stars = 1;
		as_children = false;
	}
	else
	{
		if (contains.index()!=1)
			contains = std::vector<Block>(8);
		nb_stars = std::distance(stars.begin, stars.end);
		as_children = true;
		
		Block block;

		block.as_parents = true;
		block.setSize(halfsize);

		Vector posis[] = {
			{position.x - size / 4., position.y - size / 4., position.z - size / 4.},
			{position.x - size / 4., position.y - size / 4., position.z + size / 4.},
			{position.x - size / 4., position.y + size / 4., position.z - size / 4.},
			{position.x - size / 4., position.y + size / 4., position.z + size / 4.},
			{position.x + size / 4., position.y - size / 4., position.z - size / 4.},
			{position.x + size / 4., position.y - size / 4., position.z + size / 4.},
			{position.x + size / 4., position.y + size / 4., position.z - size / 4.},
			{position.x + size / 4., position.y + size / 4., position.z + size / 4.}
		};
		auto& myblocks = std::get<1>(contains);
		auto partitions_stars = set_octree(stars, position);
		for (int ibloc = 0; ibloc < 8; ibloc++)
		{
			// bloc 1
			myblocks[ibloc] = block;
			myblocks[ibloc].position = posis[ibloc];
			// block.stars_maj(galaxy, blocks);
			block.mass_center_and_mass_maj(partitions_stars[ibloc]);
			myblocks[ibloc].divide(partitions_stars[ibloc]);
		}
	}
	
	
}
void Block::setSize(Float size)
{
	this->size = size;
	this->halfsize = size / static_cast<Float>(2.);
}



// Dit si l'étoile est dans le bloc

bool is_in(const Block& block, const Star& star)
{
	return (block.position.x + block.size / 2. > star.position.x && block.position.x - block.size / 2. < star.position.x
		&& block.position.y + block.size / 2. > star.position.y && block.position.y - block.size / 2. < star.position.y
		&& block.position.z + block.size / 2. > star.position.z && block.position.z - block.size / 2. < star.position.z);
}



// Crée le premier bloc

// void initialise_blocks(int& index_value, const Float& area, Star::container& galaxy, std::vector<Block>& blocks)
// {
// 	Block block;

// 	block.index = index_value;
// 	index_value++;
// 	block.size = area * 3.;

// 	for (int i = 0; i < galaxy.size(); i++)
// 	{
// 		if (is_in(block, galaxy[i]))
// 		{
// 			block.stars.push_back(i);
// 			galaxy[i].block_index = block.index;

// 			if (!(block.as_stars))
// 				block.as_stars = true;
// 		}

// 		else
// 			galaxy[i].is_alive = false;
// 	}

// 	block.mass_center_and_mass_maj(galaxy);
// 	blocks.push_back(block);
// }



// Génère les blocs (l'utilisation de "blocks_temp" ne sert qu'à augmenter la limite des "std:::vector<>" pour pouvoir mettre plus d'étoiles)

void create_blocks(const Float& area, Block& block, Star::range& alive_galaxy)
{
	int index_value = 0;
	block.setSize(area * 3.);
	block.mass_center_and_mass_maj(alive_galaxy);
	block.divide(alive_galaxy);
}
