// Chip-8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#define  SDL_MAIN_HANDLED
#include <SDL.h>
#include "Chip_8.h"
#include <chrono>

SDL_Window* window{};
SDL_Renderer* renderer{};
SDL_Texture* texture{};

Chip_8 myChip;
// chi-8 has two time registers that count at 60Hz
// when set above zero they whll count down to zero.
unsigned char delay_timer;
unsigned char sound_timer;
int videopitch;
// hex based keypad
unsigned char key[16];

SDL_Renderer* DrawGraphicsInit(SDL_Window* wind);
void DrawGraphicsRender(SDL_Renderer*);

void ProcessInput(uint8_t* Keys);

int main()
{
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());
	}

	auto window = SDL_CreateWindow(
		"Chip8 Emulator",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		500,
		500,
		SDL_WINDOW_SHOWN
	);

	// Create renderer
	 renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderSetLogicalSize(renderer, 1024, 512);

	// Create texture that stores frame buffer
	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		64, 32);


	if (window == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
	}

	DrawGraphicsInit(window);
	// initialize will clear memory register and screen

	// only 
	
	myChip.initialize();

	
	myChip.loadGame("Pong (alt).ch8");

	bool quit = false;
	videopitch = sizeof(myChip.gfx[0]) * 1000;
	auto cycleDelay = 5;
	auto lastCycleTime = std::chrono::high_resolution_clock::now();

	while(!quit)
	{
		ProcessInput(myChip.key);
		//emulate one cycle
		//if the draw flatg si set update screen
		// draw flag is set because the system doesnot draw every cycle

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			myChip.emulateCycle();
			lastCycleTime = currentTime;
			if (myChip.drawFlag)
			{
				uint32_t pixels[32 * 64];
				myChip.drawFlag = false;
				for (int i = 0; i < 32 * 64; i++)
				{
					if (myChip.gfx[i] == 0)
					{
						pixels[i] = 0xFF000000;
					}
					else
					{
						pixels[i] = 0xFFFFFFFF;
					}
				}
				//drawgraphics();
				SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, NULL, NULL);
				SDL_RenderPresent(renderer);
			}
		}

		//myChip.SetKeys();
	}
	SDL_Quit();
	if (renderer != nullptr)
	{
		SDL_DestroyRenderer(renderer);
		renderer = nullptr;
	}
	SDL_DestroyTexture(texture);
	SDL_DestroyWindow(window);
	return 0;
	
}
int GetOpenGLDriverIndex()
{
	auto openglIndex = -1;
	const auto driverCount = SDL_GetNumRenderDrivers();
	for (auto i = 0; i < driverCount; i++)
	{
		SDL_RendererInfo info;
		if (!SDL_GetRenderDriverInfo(i, &info))
			if (!strcmp(info.name, "opengl"))
				openglIndex = i;
	}
	return openglIndex;
}


SDL_Renderer* DrawGraphicsInit(SDL_Window* window)
{
	//renderer = SDL_CreateRenderer(window, GetOpenGLDriverIndex(), SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}

	return renderer;
}

void DrawGraphicsRender(SDL_Renderer* render)
{
	//const auto& color = SDL_Color{1,0,0,1};
	
	//SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}

void ProcessInput(uint8_t* keys)
{
	bool quit = false;

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
		{
			quit = true;
		} break;

		case SDL_KEYDOWN:
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
			{
				quit = true;
			} break;

			case SDLK_x:
			{
				keys[0] = 1;
			} break;

			case SDLK_1:
			{
				keys[1] = 1;
			} break;

			case SDLK_2:
			{
				keys[2] = 1;
			} break;

			case SDLK_3:
			{
				keys[3] = 1;
			} break;

			case SDLK_q:
			{
				keys[4] = 1;
			} break;

			case SDLK_w:
			{
				keys[5] = 1;
			} break;

			case SDLK_e:
			{
				keys[6] = 1;
			} break;

			case SDLK_a:
			{
				keys[7] = 1;
			} break;

			case SDLK_s:
			{
				keys[8] = 1;
			} break;

			case SDLK_d:
			{
				keys[9] = 1;
			} break;

			case SDLK_z:
			{
				keys[0xA] = 1;
			} break;

			case SDLK_c:
			{
				keys[0xB] = 1;
			} break;

			case SDLK_4:
			{
				keys[0xC] = 1;
			} break;

			case SDLK_r:
			{
				keys[0xD] = 1;
			} break;

			case SDLK_f:
			{
				keys[0xE] = 1;
			} break;

			case SDLK_v:
			{
				keys[0xF] = 1;
			} break;
			}
		} break;

		case SDL_KEYUP:
		{
			switch (event.key.keysym.sym)
			{
			case SDLK_x:
			{
				keys[0] = 0;
			} break;

			case SDLK_1:
			{
				keys[1] = 0;
			} break;

			case SDLK_2:
			{
				keys[2] = 0;
			} break;

			case SDLK_3:
			{
				keys[3] = 0;
			} break;

			case SDLK_q:
			{
				keys[4] = 0;
			} break;

			case SDLK_w:
			{
				keys[5] = 0;
			} break;

			case SDLK_e:
			{
				keys[6] = 0;
			} break;

			case SDLK_a:
			{
				keys[7] = 0;
			} break;

			case SDLK_s:
			{
				keys[8] = 0;
			} break;

			case SDLK_d:
			{
				keys[9] = 0;
			} break;

			case SDLK_z:
			{
				keys[0xA] = 0;
			} break;

			case SDLK_c:
			{
				keys[0xB] = 0;
			} break;

			case SDLK_4:
			{
				keys[0xC] = 0;
			} break;

			case SDLK_r:
			{
				keys[0xD] = 0;
			} break;

			case SDLK_f:
			{
				keys[0xE] = 0;
			} break;

			case SDLK_v:
			{
				keys[0xF] = 0;
			} break;
			}
		} break;
		}
	}
}
