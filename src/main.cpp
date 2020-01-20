#include <Utils.h>
#include <Block.h>
#include <glm/glm.hpp>
#include <Star.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <time.h>
#include <SDL2/SDL.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int main(int argc, char* argv[])
{
	// ------------------------- Paramètres de la simulation -------------------------
	Float	area = 500.;				// Taille de la zone d'apparition des étoiles (en années lumière)
	Float	galaxy_thickness = 0.05;	// Epaisseur de la galaxie (en "area")
	Float	precision = 0.5;			// Précision du calcul de l'accélération (algorithme de Barnes-Hut)
	bool	verlet_integration = true;	// Utiliser l'intégration de Verlet au lieu de la méthode d'Euler

	int		stars_number = 30000;		// Nombre d'étoiles (Limité à 30 000 par les std::vector<>)
	Float	initial_speed = 2500.;		// Vitesse initiale des d'étoiles (en mêtres par seconde)
	Float	black_hole_mass = 0.;		// Masse du trou noir (en masses solaires)
	bool	is_black_hole = false;		// Présence d'un trou noir

	View	view = xy;		// Type de vue
	Float	zoom = 800.;				// Taille de "area" (en pixel)
	Float	real_colors = false;		// Activer la couleur réelle des étoiles
	bool	show_blocks = false;		// Afficher les blocs

	Float	step = 200000.;				// Pas de temps de la simulation (en années)
	time_t	simulation_time = 3600;		// Temps de simulation (en seconde)
	// -------------------------------------------------------------------------------



	SDL_Init(SDL_INIT_VIDEO);

	window = NULL;
	renderer = NULL;

	SDL_CreateWindowAndRenderer(static_cast<int>(WIDTH), static_cast<int>(HEIGHT), 0, &window, &renderer);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
	SDL_SetWindowTitle(window, "Galaxy simulation");
	SDL_Event event;

	if (area < 0.1) area = 0.1;
	if (galaxy_thickness < 0.1) galaxy_thickness = 0.1;
	if (precision < 0.) precision = 0.;
	if (precision > 1.) precision = 1.;
	if (stars_number < 1) stars_number = 1;
	if (stars_number > 30000) stars_number = 30000;
	if (initial_speed < 0.) initial_speed = 0.;
	if (black_hole_mass < 0.) black_hole_mass = 0.;
	if (zoom < 1.) zoom = 1.;
	if (step < 0.) step = 0.;
	if (simulation_time < 1) simulation_time = 1;

	area *= LIGHT_YEAR;
	black_hole_mass *= SOLAR_MASS;
	step *= YEAR;

	Star::container galaxy;
	Block block;

	initialize_galaxy(galaxy, stars_number, area, initial_speed, step, is_black_hole, black_hole_mass, galaxy_thickness);

	Star::range alive_galaxy = { galaxy.begin(), galaxy.end() };

	time_t start = time(NULL);

	while (time(NULL) < start + simulation_time) // Boucle du pas de temps de la simulation
	{
		
		create_blocks(area, block, alive_galaxy);

		for (auto itStar = alive_galaxy.begin; itStar!= alive_galaxy.end; ++itStar) // Boucle sur les étoiles de la galaxie
		{
			itStar->acceleration_and_density_maj(precision, block);

			if (!(verlet_integration))
				itStar->speed_maj(step, area);

			itStar->position_maj(step, verlet_integration);

			if (!is_in(block, *itStar))
				itStar->is_alive = false;
			else if (!(real_colors))
				itStar->color_maj();
		}
			alive_galaxy.end = std::partition(alive_galaxy.begin, alive_galaxy.end, [](const Star& star) { return star.is_alive; });
			SDL_PollEvent(&event);
			if (event.type == SDL_QUIT)
			{
				SDL_Quit();
				exit(1);
			}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		draw_stars(alive_galaxy, block.mass_center, area, zoom, view);

		// if (show_blocks)
		// 	draw_blocks(blocks, block.mass_center, area, zoom, view);

		SDL_RenderPresent(renderer);
	}

	if (renderer)
		SDL_DestroyRenderer(renderer);

	if (window)
		SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}