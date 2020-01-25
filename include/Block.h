#pragma once
#include <iostream>
#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include <Utils.h>
#include <Star.h>
#include "mymemory.hpp"



// Classe définissant une zone de l'algorithme de Barnes-Hut

class Block
{
public:
	using myvariant_t = std::variant<nullptr_t, Star<3>::container::iterator, std::vector<Block>>;
	enum CoType // Contains Type
	{
		CoNull = 0,
		CoStar,
		CoBlocks
	};
	Vector3				position;		// Position du bloc
	Float				size, halfsize;	// Taille du bloc (en mètres)
	Float				mass;			// Masse contenue dans le bloc (en kilogrames)
	Vector3				mass_center;	// Centre de gravité du bloc
	size_t 				nb_stars;		// Nombre d'étoile contenu dans le block
	_std::observer_ptr<Block>			parent;			// Indice des blocs parents
	myvariant_t contains;
	
	Block();
	Block(const Block& block);

	Block& operator=(const Block& block);

	// void stars_maj(Star::range& stars, std::vector<Block>& blocks);
	void mass_center_and_mass_maj(const Star<3>::range& stars);
	void divide(Star<3>::range galaxy);
	void updateMass(bool deep = false);
	bool updateNodes();
	void makeOcto();
	void addStar(Star<3>::container::iterator itStar);
	void setSize(Float size);

	constexpr bool hasChildren() const noexcept
	{
		return contains.index() == 1;
	}
	constexpr bool hasParent() const noexcept
	{
		return static_cast<bool>(parent);
	}
	template <size_t idx>
	constexpr bool is() const
	{
		return contains.index() == idx;
	}
	template <size_t idx>
	static constexpr bool is(const myvariant_t& contains)
	{
		return contains.index() == idx;
	}
};

bool is_in(const Block& block, const Star<3>& star);
void initialise_blocks(int& index_value, const Float& area, Star<3>::container& galaxy, std::vector<Block>& blocks);
void create_blocks(const Float& area, Block& block, Star<3>::range& galaxy);

template <int N>
Vector<N> force_and_density_calculation(Float precision, Star<N>& star, const Block& block)
{
	Vector3 force = Vector3(0.f);
	{
		Vector3 starToMass = (star.position - block.mass_center);
		Float distance2 = glm::length2(starToMass);
		if (block.is<Block::CoNull>())
			return force;
		if (block.is<Block::CoStar>())
		{
			typename Star<N>::container::iterator itStar = std::get<Block::CoStar>(block.contains);
			if (distance2 != 0.)
			{
				Float inv_distance = 1. / glm::sqrt(distance2);
				force += (starToMass * inv_distance) * (-(G * block.mass) / distance2);
				star.density += (inv_distance / LIGHT_YEAR);
			}
		}
		else
		{
			Float distance = glm::sqrt(distance2);
			Float thema = block.size / distance;

			if (thema < precision)
			{
				force += (starToMass / distance) * (-(G * block.mass) / distance2);
				star.density += block.nb_stars / (distance / LIGHT_YEAR);
			}
			else
			{
				auto& blocks = std::get<Block::CoBlocks>(block.contains);
				for (int i = 0; i < 8; i++)
				{
					if (blocks[i].nb_stars > 0)
						force += force_and_density_calculation<N>(precision, star, blocks[i]);
				}
			}
		}
	}

	return force;
}
template <int N>
void acceleration_and_density_maj(const Float& precision, Star<N>& star, const Block& block)
{
	star.density = 0.;
	Float max_acceleration = 0.0000000005; // Permet de limiter l'erreur due au pas de temps (à régler en fonction du pas de temps)

	star.acceleration = force_and_density_calculation(precision, star, block); // Pas de division par la masse de l'étoile (c.f. ligne 131)

	if (glm::length2(star.acceleration) > max_acceleration* max_acceleration)
		star.acceleration = max_acceleration * glm::normalize(star.acceleration);
}