#pragma once
#include <iostream>
#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include <Utils.h>
#include <Star.h>
#include "mymemory.hpp"



// Classe définissant une zone de l'algorithme de Barnes-Hut
template <int N>
class Block
{
public:
	//HELPER
	using StarIterator = typename Star<N>::container::iterator;
	using StarRange = typename Star<N>::range;
	using myvariant_t = std::variant<nullptr_t, StarIterator, std::vector<Block<N>>>;
	using VectorN = Vector<N>;

	enum CoType // Contains Type
	{
		CoNull = 0,
		CoStar,
		CoBlocks
	};
	VectorN				position;		// Position du bloc
	Float				size, halfsize;	// Taille du bloc (en mètres)
	Float				mass;			// Masse contenue dans le bloc (en kilogrames)
	VectorN				mass_center;	// Centre de gravité du bloc
	size_t 				nb_stars;		// Nombre d'étoile contenu dans le block
	_std::observer_ptr<Block>			parent;			// Indice des blocs parents
	myvariant_t contains;
	
	Block()
	{
		mass_center = Vector3(0., 0., 0.);
		position = Vector3(0., 0., 0.);
		mass = 0.;
		size = halfsize = 0.;
		nb_stars = 0;
	}
	Block(const Block& block)
	{
		operator=(block);
	}

	Block& operator=(const Block& block)
	{
		position = block.position;
		size = block.size;
		halfsize = block.halfsize;
		mass = block.mass;
		mass_center = block.mass_center;
		nb_stars = block.nb_stars;
		parent = block.parent;
		contains = block.contains;
		return *this;
	}

	// void stars_maj(Star::range& stars, std::vector<Block>& blocks);
	void mass_center_and_mass_maj(StarRange stars)
	{
		mass_center = VectorN(static_cast<Float>(0));
		mass = 0.;

		for (auto it = stars.begin; it != stars.end; ++it)
		{
			mass_center += it->position * it->mass;
			mass += it->mass;
		}
		mass_center = mass_center / mass;
	}

	void divide(StarRange stars)
	{
		if (stars.begin == stars.end) // pas d'etoile
		{
			contains = nullptr;
			nb_stars = 0;
			mass = 0.;
			mass_center = VectorN(static_cast<Float>(0));
		}
		else if (std::next(stars.begin) == stars.end) // une étoile
		{
			contains = stars.begin;
			nb_stars = 1;
			mass = stars.begin->mass;
			mass_center = stars.begin->position;
		}
		else
		{
			makeTree();
			auto& myblocks = std::get<CoBlocks>(contains);
			auto partitions_stars = set_octree<N>(stars, position);
			mass = 0.;
			mass_center = VectorN(static_cast<Float>(0));
			for (int ibloc = 0; ibloc < (1<<N); ibloc++)
			{
				myblocks[ibloc].divide(partitions_stars[ibloc]);

				if (!myblocks[ibloc].is<CoNull>())
				{
					mass += myblocks[ibloc].mass;
					mass_center += myblocks[ibloc].mass_center * myblocks[ibloc].mass;
					nb_stars += myblocks[ibloc].nb_stars;
				}
			}
			mass_center = mass_center / mass;
		}
	}
	void updateMass(bool deep = false)
	{
		if (is<CoNull>()) // pas d'etoile
		{
			mass = static_cast<Float>(0);
			mass_center = VectorN(static_cast<Float>(0));
			nb_stars = 0;
		}
		else if (is<CoStar>()) // une étoile
		{
			mass = std::get<CoStar>(contains)->mass;
			mass_center = std::get<CoStar>(contains)->position;
			nb_stars = 1;
		}
		else
		{
			mass = static_cast<Float>(0);
			mass_center = VectorN(static_cast<Float>(0));
			auto& children = std::get<CoBlocks>(contains);
			nb_stars = 0;

			for (auto& node : children)
			{
				if (deep)
					node.updateMass(deep);
				if (!node.is<CoNull>())
				{
					mass += node.mass;
					mass_center += node.mass_center * node.mass;
					nb_stars += node.nb_stars;
				}
			}
			assert(nb_stars != 0);
			mass_center = mass_center / mass;
		}
	}
	bool updateNodes()
	{
		if (is<CoNull>(contains)) // pas d'etoile
		{
		}
		else if (is<CoStar>(contains)) // une étoile
		{
			auto itStar = std::get<CoStar>(contains);
			if (!is_in(*this, *itStar))
			{
				_std::observer_ptr<Block> searchParent = parent;

				for (; searchParent && !(is_in(*searchParent, *itStar)); searchParent = searchParent->parent);
				if (searchParent)
				{
					searchParent->addStar(itStar);
				}
				else
					itStar->is_alive = false;
				contains = nullptr;
				return true;
			}
		}
		else
		{
			bool hasChanged = false;
			for (auto& node : std::get<std::vector<Block>>(contains))
				hasChanged |= node.updateNodes();
			if (hasChanged)
			{
				StarIterator itStar;
				int nCoStar = 0;
				for (auto& node : std::get<std::vector<Block>>(contains))
				{
					if (is<CoBlocks>(node.contains))
					{
						nCoStar = 2;
						break;
					}
					if (is<CoStar>(node.contains))
					{
						itStar = std::get<StarIterator>(node.contains);
						nCoStar += 1;
					}
				}
				if (nCoStar == 1)
					contains = itStar;
				if (nCoStar == 0)
					contains = nullptr;

				return true;
			}
		}
		return false;
	}
	void makeTree()
	{
		if (!is<CoBlocks>(contains))
			contains = std::vector<Block>(1<<N);
		Block block;
		block.setSize(halfsize);
		block.mass = 0.;
		block.nb_stars = 0;

		const auto quartSize = size / 4.;
		VectorN posis[(1 << N)];
		
		
		for (int iPos = 0; iPos < (1 << N); ++iPos)
		{
			for (int iAxis = 0; iAxis < N; iAxis++)
				posis[iPos][iAxis] = position[iAxis] + quartSize * static_cast<Float>(1 - 2 * (1 - (iPos >> ((N - 1) - iAxis)) % 2));
		}
		
		auto& children = std::get<CoBlocks>(contains);
		for (int iNode = 0; iNode < (1 << N); iNode++)
		{
			auto& current_node = children[iNode];
			current_node = block;
			current_node.mass_center = block.position;
			current_node.parent = _std::make_observer(this);
			current_node.position = posis[iNode];
		}
	}
	void addStar(StarIterator itStar)
	{
		if (is<CoNull>(contains))
		{
			contains = itStar;
		}
		else
		{
			constexpr auto idx = [](VectorN pos1, VectorN pos2) {
				int res=0;
				for (int i = 0; i < N; i++)
					res += ((pos1[i] > pos2[i]) << (N-1-i));
				return res;
			};

			if (is<CoStar>(contains))
			{
				auto old = std::get<CoStar>(contains);
				assert(old != itStar);
				makeTree();
				auto id = idx(old->position, position);
				std::get<CoBlocks>(contains)[id].addStar(old);
			}
			auto id = idx(itStar->position, position);
			std::get<CoBlocks>(contains)[id].addStar(itStar);
		}

		//updateMass();
	}
	void setSize(Float size)
	{
		this->size = size;
		this->halfsize = size / static_cast<Float>(2.);
	}

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
template <int N>
bool is_in(const Block<N>& block, const Star<N>& star) 
{
	for (int i = 0; i < N; i++)
		if (block.position[i] + block.halfsize < star.position[i] || block.position[i] - block.halfsize > star.position[i])
			return false;
	return true;
}
template <int N>
void create_blocks(const Float& area, Block<N>& block, typename Star<N>::range& alive_galaxy)
{
	block.setSize(area * 3.);
	block.divide(alive_galaxy);
}


template <int N>
Vector<N> force_and_density_calculation(Float precision, Star<N>& star, const Block<N>& block, typename Star<N>::container::iterator& itStarFound)
{
	Vector<N> force = Vector3(0.f);
	{
		Vector<N> starToMass = (star.position - block.mass_center);
		Float distance2 = glm::length2(starToMass);
		if (block.is<Block<N>::CoNull>())
			return force;
		if (block.is<Block<N>::CoStar>())
		{
			typename Star<N>::container::iterator itStar = std::get<Block<N>::CoStar>(block.contains);
			if (distance2 != 0.)
			{
				Float inv_distance = 1. / glm::sqrt(distance2);
				force += (starToMass * inv_distance) * (-(G * block.mass) / distance2);
				star.density += (inv_distance / LIGHT_YEAR);
				itStarFound = itStar;
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
				auto& blocks = std::get<Block<N>::CoBlocks>(block.contains);
				for (int i = 0; i < 1<<N; i++)
				{
					if (blocks[i].nb_stars > 0)
						force += force_and_density_calculation<N>(precision, star, blocks[i], itStarFound);
				}
			}
		}
	}

	return force;
}
template <int N>
void acceleration_and_density_maj(const Float& precision, Star<N>& star, const Block<N>& block)
{
	star.density = 0.;
	Float max_acceleration = 0.0000000005; // Permet de limiter l'erreur due au pas de temps (à régler en fonction du pas de temps)
	typename Star<N>::container::iterator itStarFound;
	star.acceleration = force_and_density_calculation(precision, star, block, itStarFound); // Pas de division par la masse de l'étoile (c.f. ligne 131)

	if (glm::length2(star.acceleration) > max_acceleration* max_acceleration)
		star.acceleration = max_acceleration * glm::normalize(star.acceleration);
}
template <int N, int cN>
struct Test
{
	void operator()(std::array<typename Star<N>::range, 1 << N >& result, int& iPart, typename Star<N>::range stars, Vector<N> pivot)
	{
		auto testStarAxis = [pivot](const Star<N>& star) {return star.position[cN] < pivot[cN]; };
		typename Star<N>::container::iterator itcN = std::partition(stars.begin, stars.end, testStarAxis);
		if (cN == N - 1)
		{
			result[iPart++] = Star<N>::range{ stars.begin, itcN };
			result[iPart++] = Star<N>::range{ itcN, stars.end };
		}
		else
		{
			Test<N, cN + 1>()(result, iPart, { stars.begin, itcN }, pivot);
			Test<N, cN + 1>()(result, iPart, { itcN, stars.end }, pivot);
		}
	}
};
template <int N>
struct Test<N, N>
{
	void operator()(std::array<typename Star<N>::range, 1 << N >& result, int& iPart, typename Star<N>::range stars, Vector<N> pivot)
	{

	}
};
//template <int N, int cN>
//void sub_set_octree(std::array<typename Star<N>::range, 1 << N >& result, int& iPart, typename Star<N>::range stars, Vector<N> pivot)
//{
//	auto testStarAxis = [pivot](const Star<N>& star) {return star.position[cN] < pivot[cN]; };
//	typename Star<N>::container::iterator itcN = std::partition(stars.begin, stars.end, testStarAxis);
//	if (cN==N-1)
//	{
//		result[iPart++] = Star<N>::range{ stars.begin, itcN };
//		result[iPart++] = Star<N>::range{ itcN, stars.end };
//	}
//	else
//	{
//		sub_set_octree<N, cN + 1>(result, iPart, { stars.begin, itcN }, pivot);
//		sub_set_octree<N, cN + 1>(result, iPart, { itcN, stars.end }, pivot);
//	}
//}
//template <int N>
//void sub_set_octree<N, N>(std::array<typename Star<N>::range, 1 << N >& result, int& iPart, typename Star<N>::range stars, Vector<N> pivot)
//{
//
//}

template <int N>
std::array<typename Star<N>::range, 1<<N > set_octree(typename Star<N>::range stars, Vector<N> pivot)
{
	std::array<typename Star<N>::range, 1 << N> result;
	int iPart = 0;
	Test<N, 0>()(result, iPart, stars, pivot);
	return result;
}
