#define USE_OPENGL 1
#define VARYING_TIME 1

#include <algorithm>
#include <fstream>
#include <thread>
#include <atomic>
#include <SDL2/SDL.h>
#include <nlohmann/json.hpp>
#include <Utils.h>
#include <Block.h>
#include <Star.h>
#include <Draw_OGL.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

#define READ_PARAMETER(value) { if (auto it = parameters.find(#value); it != parameters.end()) value = *it; }
struct MutexRange {
	Star::range part;
	std::atomic<int> ready = 0;
};
template <size_t N>
void make_partitions(std::array<MutexRange, N>& mutparts, Star::range alive_galaxy, size_t total)
{
	const size_t nPerPart = total / N;
	auto currentIt = alive_galaxy.begin, prevIt = alive_galaxy.begin;
	for (size_t i = 0; i < N - 1; ++i)
	{
		for (size_t iIt = 0; iIt < nPerPart; ++iIt, ++currentIt);

		mutparts[i].part = { prevIt, currentIt };
		mutparts[i].ready = 1;

		prevIt = currentIt;
	}
	mutparts.back().part = { currentIt, alive_galaxy.end };
	mutparts.back().ready = 1;
}


int main(int argc, char* argv[])
{
	// ------------------------- Paramètres de la simulation -------------------------
	Float	area = 500.;				// Taille de la zone d'apparition des étoiles (en années lumière)
	Float	galaxy_thickness = 0.2;	// Epaisseur de la galaxie (en "area")
	Float	precision = 300.0;			// Précision du calcul de l'accélération (algorithme de Barnes-Hut)
	bool	verlet_integration = true;	// Utiliser l'intégration de Verlet au lieu de la méthode d'Euler

	int		stars_number = 10000;		// Nombre d'étoiles 
	Float	initial_speed = 2500.;		// Vitesse initiale des d'étoiles (en mêtres par seconde)
	Float	black_hole_mass = 0.;		// Masse du trou noir (en masses solaires)
	bool	is_black_hole = false;		// Présence d'un trou noir

	View	view = default_view;		// Type de vue
	Float	zoom = 800.;				// Taille de "area" (en pixel)
	bool	real_colors = false;		// Activer la couleur réelle des étoiles
	bool	show_blocks = false;		// Afficher les blocs

	Float	step = 200000.;				// Pas de temps de la simulation (en années)
	time_t	simulation_time = 600;		// Temps de simulation (en seconde)
	constexpr size_t nThread = 8;
	// -------------------------------------------------------------------------------
	{
		std::ifstream fileParams((argc == 2) ? argv[1] : "./parameters.json");
		if (fileParams)
		{
			nlohmann::json parameters;// = nlohmann::json::parse(fileParams);
			fileParams >> parameters;

			READ_PARAMETER(area)
			READ_PARAMETER(galaxy_thickness)
			READ_PARAMETER(precision)
			READ_PARAMETER(verlet_integration)

			READ_PARAMETER(stars_number)
			READ_PARAMETER(initial_speed)
			READ_PARAMETER(black_hole_mass)
			READ_PARAMETER(is_black_hole)

			READ_PARAMETER(view)
			READ_PARAMETER(zoom)
			READ_PARAMETER(real_colors)
			READ_PARAMETER(show_blocks)

			READ_PARAMETER(step)
			READ_PARAMETER(simulation_time)
		}
	}
	SDL_Init(SDL_INIT_VIDEO);

	window = nullptr;
	renderer = nullptr;
#if USE_OPENGL
	// Version d'OpenGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Double Buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	window = SDL_CreateWindow("Galaxy simulation", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext glcont = SDL_GL_CreateContext(window);
	if(!glcont)
		throw std::runtime_error("Unable to create GL Context.");

	DrawGL drawPlugin;
	drawPlugin.setWindow(_std::make_observer(window));
#else
	SDL_CreateWindowAndRenderer(static_cast<int>(WIDTH), static_cast<int>(HEIGHT), 0, &window, &renderer);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
	SDL_SetWindowTitle(window, "Galaxy simulation");
#endif
	SDL_Event event;

	if (area < 0.1) area = 0.1;
	////if (galaxy_thickness < 0.1) galaxy_thickness = 0.1;
	// if (precision > 1.) precision = 1.;
	if (stars_number < 1) stars_number = 1;
	//if (stars_number > 30000) stars_number = 30000;
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
	
	drawPlugin.init();

	initialize_galaxy(galaxy, stars_number, area, initial_speed, step, is_black_hole, black_hole_mass, galaxy_thickness);

	Star::range alive_galaxy = { galaxy.begin(), galaxy.end() };
	float step2 = step * step;
	Float currentStep=static_cast<Float>(1.);
	bool stopThreads = false;
	auto updateStars = [&block, precision, verlet_integration, step, area, real_colors, &stopThreads, &currentStep](MutexRange* mutpart)
	{
		using namespace std::chrono_literals;
		while (mutpart->ready != 1)
			std::this_thread::sleep_for(2ms);
		while (!stopThreads)
		{
			for (auto itStar = mutpart->part.begin; itStar != mutpart->part.end; ++itStar) // Boucle sur les étoiles de la galaxie
			{
				itStar->acceleration_and_density_maj(precision, block);

				if (!(verlet_integration))
					itStar->speed_maj(step * currentStep, area);

				itStar->position_maj(step * currentStep, verlet_integration);

				if (!is_in(block, *itStar))
					itStar->is_alive = false;
				else if (!(real_colors))
					itStar->color_maj();
			}
			mutpart->ready = 2;
			while (mutpart->ready != 1 && !stopThreads)
				std::this_thread::sleep_for(2ms);
		}
	};
	std::array<std::thread, nThread> mythreads;
	std::array<MutexRange, nThread> mutparts;
	for (int i = 0; i < mythreads.size(); ++i)
	{
		mutparts[i].ready = 0;
		mythreads[i] = std::thread(updateStars, &mutparts[i]);
	}
	auto totalGalaxy = std::distance(alive_galaxy.begin, alive_galaxy.end);
	auto t0 = std::chrono::steady_clock::now();
	bool stopProgram=false;
	bool pauseSimulation=false;
	create_blocks(area, block, alive_galaxy);
	make_partitions<nThread >(mutparts, alive_galaxy, totalGalaxy);
	while (!stopProgram) // Boucle du pas de temps de la simulation
	{
		using namespace std::chrono_literals;
		if (!pauseSimulation)
		{
			block.updateNodes();
			block.updateMass(true);
			make_partitions<nThread >(mutparts, alive_galaxy, totalGalaxy);
			for (auto& mp : mutparts)
				while (mp.ready != 2)
					std::this_thread::sleep_for(1ms);
			{
				auto prevEnd = alive_galaxy.end;
				alive_galaxy.end = std::partition(alive_galaxy.begin, alive_galaxy.end, [](const Star& star) { return star.is_alive; });
				totalGalaxy -= std::distance(alive_galaxy.end, prevEnd);
			}
		}
		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.scancode==SDL_SCANCODE_ESCAPE))
			{
				stopProgram=true;
			}
			if (event.type == SDL_KEYDOWN && event.key.keysym.scancode==SDL_SCANCODE_SPACE)
				pauseSimulation=!pauseSimulation;
			drawPlugin.event(&event);
		}
		drawPlugin.update(alive_galaxy);
		// SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		// SDL_RenderClear(renderer);

		// draw_stars(alive_galaxy, block.mass_center, area, zoom, view);
		drawPlugin.render();

		// SDL_RenderPresent(renderer);
		
#if VARYING_TIME
		auto t1 = std::chrono::steady_clock::now();
		std::chrono::duration<Float, std::ratio<1,60>> duree = t1-t0;
		t0=t1;
		currentStep = duree.count();
#else
			currentStep = 1.;
#endif
	}
	stopThreads = true;
	for (auto& thr : mythreads)
		thr.join();

	if (renderer)
		SDL_DestroyRenderer(renderer);

	if (window)
		SDL_DestroyWindow(window);

	SDL_Quit();
	return 0;
}