#include <Block.h>
#include <Utils.h>
#include <glm/glm.hpp>
#include <Star.h>
#include <vector>
#include <random>
#include <SDL2/SDL.h>

std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

// Donne un int aléatoire entre deux bornes

int random_int(int min, int max)
{
	std::uniform_int_distribution<> dis(min, max);
	return dis(gen);
}



// Donne un Float aléatoire entre deux bornes

Float random_double(Float min, Float max)
{
    std::uniform_real_distribution<> dis(min, max);
	return dis(gen);
}

// Donne la valeur cartésienne x à partir des coordonnées sphériques (en mètres)

Float get_x(Float radius, Float phi, Float theta)
{
	return glm::cos(phi) * glm::sin(theta) * radius;
}



// Donne la valeur cartésienne y à partir des coordonnées sphériques (en mètres)

Float get_y(Float radius, Float phi, Float theta)
{
	return glm::sin(phi) * glm::sin(theta) * radius;
}



// Donne la valeur cartésienne z à partir des coordonnées sphériques (en mètres)

Float get_z(Float radius, Float phi, Float theta)
{
	return glm::cos(theta) * radius;
}

// Construit un vecteur à partir de ses coordonnées sphériques

Vector3 create_spherical(Float radius, Float phi, Float theta)
{
	return Vector3(get_x(radius, phi, theta), get_y(radius, phi, theta), get_z(radius, phi, theta));
}

// Donne l'angle phi du vecteur (en radiants)

Float get_phi(Vector3 vector)
{
	vector.z = 0.;

	if (vector.y > 0)
		return glm::acos(vector.x / glm::length(vector));

	if (vector.y < 0)
		return 2 * PI - acos(vector.x / glm::length(vector));
}



// Donne l'angle theta du vecteur (en radiants)

Float get_theta(Vector3 vector)
{
	return glm::acos(vector.z / glm::length(vector));
}

Float get_phi(Vector3 point_1, Vector3 point_2)
{
	return get_phi(point_1 - point_2);
}
Float get_theta(Vector3 point_1, Vector3 point_2)
{
	return get_theta(point_1 - point_2);
}