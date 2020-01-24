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
	using myvariant_t = std::variant<nullptr_t, Star::container::iterator, std::vector<Block>>;
	enum CoType // Contains Type
	{
		CoNull = 0,
		CoStar,
		CoBlocks
	};
	Vector				position;		// Position du bloc
	Float				size, halfsize;	// Taille du bloc (en mètres)
	Float				mass;			// Masse contenue dans le bloc (en kilogrames)
	Vector				mass_center;	// Centre de gravité du bloc
	size_t 				nb_stars;		// Nombre d'étoile contenu dans le block
	_std::observer_ptr<Block>			parent;			// Indice des blocs parents
	myvariant_t contains;
	
	Block();
	Block(const Block& block);

	Block& operator=(const Block& block);

	// void stars_maj(Star::range& stars, std::vector<Block>& blocks);
	void mass_center_and_mass_maj(const Star::range& stars);
	void divide(Star::range galaxy);
	void updateMass(bool deep = false);
	bool updateNodes();
	void makeOcto();
	void addStar(Star::container::iterator itStar);
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

bool is_in(const Block& block, const Star& star);
void initialise_blocks(int& index_value, const Float& area, Star::container& galaxy, std::vector<Block>& blocks);
void create_blocks(const Float& area, Block& block, Star::range& galaxy);
