#include <Block.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <Star.h>
#include <vector>



// Construit une étoile

Star::Star()
{
	is_alive = false;
	previous_position = Vector3(0.f);
	position = Vector3(0.f);
	speed = Vector3(0.f);
	acceleration = Vector3(0.f);
	color = 0;
	mass = 0.f;
	density = 0.f;
	index = 0;
	block_index = 0;
}



// Construit une étoile à des coordonnées aléatoires dans la zone

Star::Star(const Float& initial_speed, const Float& area, const Float& step, const Float& galaxy_thickness)
{
	is_alive = true;
	position = create_spherical((sqrt(random_double(0., 1.)) - 0.5) * area, random_double(0., 2. * PI), PI / 2.)*Vector3(1);
	position.z = ((random_double(0., 1.) - 0.5) * (area * galaxy_thickness));
	speed = glm::normalize(Vector3(-position.y, position.x, 0.)) * initial_speed;//create_spherical((((area / 2.) - glm::length(position)) / (area / 2.)) * initial_speed, get_phi(position) + PI / 2., PI / 2.);
	previous_position = position - speed * step;
	acceleration = Vector3(0.f);
	color = RGB(0, 0, 0);
	mass = 0.;
	density = 0.;
	index = 0;
	block_index = 0;
}



// Construit une étoile à partir d'une autre étoile

Star::Star(const Star& star)
{
	is_alive = star.is_alive;
	previous_position = star.previous_position;
	position = star.position;
	speed = star.speed;
	acceleration = star.acceleration;
	mass = star.mass;
	density = star.density;
	color = star.color;
	index = star.index;
	block_index = star.block_index;
}



// Assignation

void Star::operator=(const Star& star)
{
	is_alive = star.is_alive;
	previous_position = star.previous_position;
	position = star.position;
	speed = star.speed;
	acceleration = star.acceleration;
	mass = star.mass;
	density = star.density;
	color = star.color;
	index = star.index;
	block_index = star.block_index;
}



// Met à jour la position

void Star::position_maj(Float step, bool verlet_integration)
{
	if (verlet_integration)
	{
		Vector3 temp = position;

		position = static_cast<Float>(2) * position - previous_position + acceleration * step * step; // Intégration de Verlet
		previous_position = temp;
	}

	else
		position += speed * step; // Méthode d'Euler
}



// Met à jour la vitesse

void Star::speed_maj(Float step, Float area)
{
	speed += acceleration * step;
}



// Met à jour l'accélération et la densité

void Star::acceleration_and_density_maj(const Float& precision, const Block& block)
{
	density = 0.;
	Float max_acceleration = 0.0000000005; // Permet de limiter l'erreur due au pas de temps (à régler en fonction du pas de temps)

	acceleration = force_and_density_calculation(precision, *this, block); // Pas de division par la masse de l'étoile (c.f. ligne 131)

	if (glm::length2(acceleration) > max_acceleration* max_acceleration)
		acceleration = max_acceleration * glm::normalize(acceleration);//create_spherical(max_acceleration, get_phi(acceleration), get_theta(acceleration));
}



// Calcule la densité et la force exercée sur une étoile (divisée par la masse de l'étoile pour éviter des calculs inutiles)

Vector3 force_and_density_calculation(const Float& precision, Star& star, const Block& block)
{
	Vector3 force = Vector3(0.f);
	{
		Vector3 starToMass = (star.position - block.mass_center);
		Float distance2 = glm::length2(starToMass);
		if (block.is<Block::CoNull>())
			return force;
		if (block.is<Block::CoStar>())
		{
			Star::container::iterator itStar = std::get<Block::CoStar>(block.contains);
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
						force += force_and_density_calculation(precision, star, blocks[i]);
				}
			}
		}
	}

	return force;
}



// Met à jour la couleur

void Star::color_maj()
{
	int color_nb = density / 3.;

	if (color_nb > 255 * 2)
		color_nb = 255 * 2;

	if (color_nb < 255)
		color = RGB(0, color_nb, 255);

	else
		color = RGB(color_nb - 255, 255, 255);
}



// Initialise la galaxie

void initialize_galaxy(Star::container& galaxy, const int& stars_number, const Float& area, const Float& initial_speed, const Float& step, const bool& is_black_hole, const Float& black_hole_mass, const Float& galaxy_thickness)
{
	for (int i = 0; i <= stars_number * 0.764; i++)
	{
		galaxy.push_back(Star(initial_speed, area, step, galaxy_thickness));
		galaxy.back().mass = random_double(0.08, 0.45) * SOLAR_MASS;
		galaxy.back().color = RGB(255, 10, 10);
		galaxy.back().index = galaxy.size() - 1;
	}

	for (int i = 0; i <= stars_number * 0.121; i++)
	{
		galaxy.push_back(Star(initial_speed, area, step, galaxy_thickness));
		galaxy.back().mass = random_double(0.45, 0.8) * SOLAR_MASS;
		galaxy.back().color = RGB(255, 127, 10);
		galaxy.back().index = galaxy.size() - 1;
	}

	for (int i = 0; i <= stars_number * 0.076; i++)
	{
		galaxy.push_back(Star(initial_speed, area, step, galaxy_thickness));
		galaxy.back().mass = random_double(0.8, 1.04) * SOLAR_MASS;
		galaxy.back().color = RGB(255, 255, 10);
		galaxy.back().index = galaxy.size() - 1;
	}

	for (int i = 0; i <= stars_number * 0.030; i++)
	{
		galaxy.push_back(Star(initial_speed, area, step, galaxy_thickness));
		galaxy.back().mass = random_double(1.04, 1.4) * SOLAR_MASS;
		galaxy.back().color = RGB(255, 255, 127);
		galaxy.back().index = galaxy.size() - 1;
	}

	for (int i = 0; i <= stars_number * 0.006; i++)
	{
		galaxy.push_back(Star(initial_speed, area, step, galaxy_thickness));
		galaxy.back().mass = random_double(1.4, 2.1) * SOLAR_MASS;
		galaxy.back().color = RGB(255, 255, 255);
		galaxy.back().index = galaxy.size() - 1;
	}

	for (int i = 0; i <= stars_number * 0.0013; i++)
	{
		galaxy.push_back(Star(initial_speed, area, step, galaxy_thickness));
		galaxy.back().mass = random_double(2.1, 16) * SOLAR_MASS;
		galaxy.back().color = RGB(50, 255, 255);
		galaxy.back().index = galaxy.size() - 1;
	}

	if (is_black_hole)
	{
		galaxy.push_back(Star(initial_speed, area, step, galaxy_thickness));
		galaxy.back().position = Vector3(0.f);
		galaxy.back().speed = Vector3(0.f);
		galaxy.back().mass = black_hole_mass;
		galaxy.back().color = RGB(0, 0, 0);
		galaxy.back().index = galaxy.size() - 1;
	}
}