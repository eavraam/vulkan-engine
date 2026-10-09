#include <memory>
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#	include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

const uint32_t WIDTH  = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication
{
  public:
	void run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

  private:
	SDL_Window *window = nullptr;

	void initWindow()
	{
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
		}

		window = SDL_CreateWindow("Vulkan", WIDTH, HEIGHT, SDL_WINDOW_VULKAN);
		if (!window)
		{
			throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
		}
	}

	void initVulkan()
	{
	}

	void mainLoop()
	{
		bool running = true;
		while (running)
		{
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
					case SDL_EVENT_QUIT:
						running = false;
						break;

					// keypresses
					case SDL_EVENT_KEY_DOWN:
						if (event.key.key == SDLK_ESCAPE) // exit application on ESC
						{
							running = false;
						}
						break;
					default:
						break;
				}
			}
		}
	}

	void cleanup()
	{
		SDL_DestroyWindow(window);
		SDL_Quit();
	}
};

int main()
{
	try
	{
		HelloTriangleApplication app;
		app.run();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}