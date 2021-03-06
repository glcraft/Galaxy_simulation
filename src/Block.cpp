#include <Block.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <Star.h>
#include <array>
#include <algorithm>
#include <functional>
#include <cmath>

std::array<Star<3>::range, 8> set_octree(Star<3>::range stars, glm::vec3 pivot)
{
	std::array<std::function<bool(const Star<3>& star)>, 3> testStarAxis = {
		[pivot](const Star<3>& star){return star.position.x<pivot.x;},
		[pivot](const Star<3>& star){return star.position.y<pivot.y;},
		[pivot](const Star<3>& star){return star.position.z<pivot.z;}
	};
    std::array<Star<3>::range, 8> result;
	int iPart=0;
    Star<3>::container::iterator itX = std::partition(stars.begin, stars.end, testStarAxis[0]);
    auto xParts = std::array{Star<3>::range{stars.begin, itX}, Star<3>::range{itX, stars.end}};
    for(auto& part : xParts)
    {
        Star<3>::container::iterator itY = std::partition(part.begin, part.end, testStarAxis[1]);
		auto yParts = std::array{Star<3>::range{part.begin, itY}, Star<3>::range{itY, part.end}};
        
        for(auto& part : yParts)
        {
        	Star<3>::container::iterator itZ = std::partition(part.begin, part.end, testStarAxis[2]);
			result[iPart++] = Star<3>::range{part.begin, itZ};
			result[iPart++] = Star<3>::range{itZ, part.end};
        }
    }
    return result;
}

// Construit un bloc

Block::Block()
{
	mass_center = Vector3(0., 0., 0.);
	position = Vector3(0., 0., 0.);
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



// Met à jour les étoiles contenues dans le bloc

// void Block::stars_maj(Star<3>::range& stars, std::vector<Block>& blocks)
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

void Block::mass_center_and_mass_maj(const Star<3>::range& galaxy)
{
	mass_center = Vector3(0., 0., 0.);
	mass = 0.;

	for (auto it = galaxy.begin; it!=galaxy.end; ++it)
	{
		mass_center += it->position * it->mass;
		mass += it->mass;
	}
	mass_center = mass_center / mass;
}



// Divise un bloc en 8 plus petits

void Block::divide(Star<3>::range stars)
{
	if (stars.begin==stars.end) // pas d'etoile
	{
		contains = nullptr;
		nb_stars = 0;
		mass = 0.;
		mass_center = Vector3(0.);
	}
	else if (std::next(stars.begin)==stars.end) // une étoile
	{
		contains = stars.begin;
		nb_stars = 1;
		mass = stars.begin->mass;
		mass_center = stars.begin->position;
	}
	else
	{
		if (!is<CoBlocks>(contains))
			contains = std::vector<Block>(8);
		//nb_stars = std::distance(stars.begin, stars.end);
		
		Block block;
		block.setSize(halfsize);
		const auto quartSize = size / 4.;
		Vector3 posis[] = {
			{position.x - quartSize, position.y - quartSize, position.z - quartSize},
			{position.x - quartSize, position.y - quartSize, position.z + quartSize},
			{position.x - quartSize, position.y + quartSize, position.z - quartSize},
			{position.x - quartSize, position.y + quartSize, position.z + quartSize},
			{position.x + quartSize, position.y - quartSize, position.z - quartSize},
			{position.x + quartSize, position.y - quartSize, position.z + quartSize},
			{position.x + quartSize, position.y + quartSize, position.z - quartSize},
			{position.x + quartSize, position.y + quartSize, position.z + quartSize}
		};
		auto& myblocks = std::get<CoBlocks>(contains);
		auto partitions_stars = set_octree(stars, position);
		Float newMass = 0.f;
		Vector3 newMassCenter(0);
		nb_stars = 0;
		for (int ibloc = 0; ibloc < 8; ibloc++)
		{
			myblocks[ibloc] = block;
			myblocks[ibloc].parent = _std::make_observer(this);
			myblocks[ibloc].position = posis[ibloc];
			myblocks[ibloc].divide(partitions_stars[ibloc]);

			if (myblocks[ibloc].nb_stars > 0)
			{
				newMass += myblocks[ibloc].mass;
				newMassCenter += myblocks[ibloc].mass_center * myblocks[ibloc].mass;
				nb_stars += myblocks[ibloc].nb_stars;
			}
		}
		mass = newMass;
		mass_center = newMassCenter / newMass;
	}
}
void Block::updateMass(bool deep )
{
	if (is<CoNull>()) // pas d'etoile
	{
		mass = 0.;
		mass_center = Vector3(0.);
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
		mass = 0.;
		mass_center = Vector3(0.);
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
bool Block::updateNodes()
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
			Star<3>::container::iterator itStar;
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
					itStar = std::get<Star<3>::container::iterator>(node.contains);
					nCoStar += 1;
				}
			}
			if (nCoStar==1)
				contains = itStar;
			if (nCoStar==0)
				contains = nullptr;
			
			return true;
		}
	}
	return false;
}
void Block::makeOcto()
{
	if (!is<CoBlocks>(contains))
		contains = std::vector<Block>(8);
	Block block;
	block.setSize(halfsize);
	block.mass = 0.;
	block.nb_stars=0;
	
	const auto quartSize = size / 4.;
	Vector3 posis[] = {
		{position.x - quartSize, position.y - quartSize, position.z - quartSize},
		{position.x - quartSize, position.y - quartSize, position.z + quartSize},
		{position.x - quartSize, position.y + quartSize, position.z - quartSize},
		{position.x - quartSize, position.y + quartSize, position.z + quartSize},
		{position.x + quartSize, position.y - quartSize, position.z - quartSize},
		{position.x + quartSize, position.y - quartSize, position.z + quartSize},
		{position.x + quartSize, position.y + quartSize, position.z - quartSize},
		{position.x + quartSize, position.y + quartSize, position.z + quartSize}
	};
	auto& children = std::get<CoBlocks>(contains);
	for (int iNode = 0; iNode < 8; iNode++)
	{
		auto& current_node = children[iNode];
		current_node = block;
		current_node.mass_center = block.position;
		current_node.parent = _std::make_observer(this);
		current_node.position = posis[iNode];
	}
}
void Block::addStar(Star<3>::container::iterator itStar)
{
	if (is<CoNull>(contains))
	{
		contains = itStar;
	}
	else
	{
		constexpr auto idx = [](Vector3 pos1, Vector3 pos2) {
			return ((pos1.x > pos2.x)<<2) + ((pos1.y > pos2.y)<<1)  + (pos1.z > pos2.z);
		};
		
		if (is<CoStar>(contains))
		{
			auto old = std::get<CoStar>(contains);
			assert(old != itStar);
			makeOcto();
			auto id = idx(old->position, position);
			std::get<CoBlocks>(contains)[id].addStar(old);
		}
		auto id = idx(itStar->position, position);
		std::get<CoBlocks>(contains)[id].addStar(itStar);
	}
	
	//updateMass();
}
void Block::setSize(Float size)
{
	this->size = size;
	this->halfsize = size / static_cast<Float>(2.);
}



// Dit si l'étoile est dans le bloc

bool is_in(const Block& block, const Star<3>& star)
{
	return (block.position.x + block.halfsize > star.position.x && block.position.x - block.halfsize < star.position.x
		 && block.position.y + block.halfsize > star.position.y && block.position.y - block.halfsize < star.position.y
		 && block.position.z + block.halfsize > star.position.z && block.position.z - block.halfsize < star.position.z);
}

// Génère les blocs

void create_blocks(const Float& area, Block& block, Star<3>::range& alive_galaxy)
{
	block.setSize(area * 3.);
	block.divide(alive_galaxy);
}
