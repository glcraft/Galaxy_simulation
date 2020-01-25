#pragma once
#include <list>
#include <functional>
#include <glm/gtx/norm.hpp>
#include <Utils.h>


// Classe définissant une étoile
template <int N>
class Star
{

public:
	using container = std::list<Star<N>>;
	using VectorN = Vector<N>;
	struct range{
		typename container::iterator begin;
		typename container::iterator end;
	};

	bool		is_alive;			// Indique si l'étoile est prise en compte
	VectorN		previous_position;	// Position d'avant
	VectorN		position;			// Position
	VectorN		speed;				// Vitesse
	VectorN		acceleration;		// Accélération
	Float		mass;				// Masse (en kilogrames)
	Float		density;			// Densité autour de l'étoile
	uint32_t	color;				// Couleur

	Star()
	{
		is_alive = false;
		previous_position = VectorN(static_cast<Float>(0));
		position = VectorN(static_cast<Float>(0));
		speed = VectorN(static_cast<Float>(0));
		acceleration = Vector3(static_cast<Float>(0));
		color = 0;
		mass = static_cast<Float>(0);
		density = static_cast<Float>(0);
	}

	Star(const std::function<void(Star&)>& func)
	{
		is_alive = true;
		//position = create_spherical((sqrt(random_double(0., 1.)) - 0.5) * area, random_double(0., 2. * PI), PI / 2.) * VectorN(1.);
		//position.z = ((random_double(0., 1.) - 0.5) * (area * galaxy_thickness));
		//speed = glm::normalize(glm::cross(position, crvec)) * initial_speed;//create_spherical((((area / 2.) - glm::length(position)) / (area / 2.)) * initial_speed, get_phi(position) + PI / 2., PI / 2.);
		//previous_position = position - speed * step;
		acceleration = Vector<N>(static_cast<Float>(0));
		color = RGB(0, 0, 0);
		mass = static_cast<Float>(0);
		density = static_cast<Float>(0);
		func(*this);
	}
	Star(const Star& star)
	{
		operator=(star);
	}

	Star& operator=(const Star& star)
	{
		is_alive = star.is_alive;
		previous_position = star.previous_position;
		position = star.position;
		speed = star.speed;
		acceleration = star.acceleration;
		mass = star.mass;
		density = star.density;
		color = star.color;
		return *this;
	}

	void position_maj(Float step, bool verlet_integration)
	{
		if (verlet_integration)
		{
			VectorN temp = position;

			position = static_cast<Float>(2)* position - previous_position + acceleration * step * step; // Intégration de Verlet
			previous_position = temp;
		}
		else
			position += speed * step; // Méthode d'Euler
	}
	void speed_maj(Float step, Float area)
	{
		speed += acceleration * step;
	}
	
	
	
	void color_maj()
	{
		int color_nb = density / 3.;

		if (color_nb > 255 * 2)
			color_nb = 255 * 2;

		if (color_nb < 255)
			color = RGB(0, color_nb, 255);

		else
			color = RGB(color_nb - 255, 255, 255);
	}
};

