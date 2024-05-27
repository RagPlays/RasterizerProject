//Standard includes
#include <iostream>

//External includes
#ifdef _DEBUG
#include "vld.h"
#endif // DEBUG

// SDL includes
#include <memory>
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Project includes
#include "Timer.h"
#include "Renderer.h"

using namespace dae;

int main()
{
	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	constexpr uint32_t width{ 640 };
	constexpr uint32_t height{ 480 };

	SDL_Window* pWindow
	{
		SDL_CreateWindow
		(
			"Rasterizer - **Maurice Vandenheede** - 2DAE18N",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width, height, 0
		)
	};

	if (!pWindow) return 1;

	//Initialize "framework"
	Timer* pTimer{ new Timer{} };
	Renderer* pRenderer{ new Renderer{ pWindow, width, height } };

	//Start loop
	pTimer->Start();

	float printTimer{};
	bool showFPS{ true };
	bool isLooping{ true };
	bool takeScreenshot{ false };
	bool clearConsole{ false };

	SDL_Event e;

	while (isLooping)
	{
		//--------- Get input events ---------//
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;
			case SDL_KEYUP:
				switch (e.key.keysym.scancode)
				{
				case SDL_SCANCODE_X:
					takeScreenshot = true;
					break;

				case SDL_SCANCODE_C:
					clearConsole = !clearConsole;
					break;

				case SDL_SCANCODE_F4:
					pRenderer->ToggleDepthBuffer();
					break;

				case SDL_SCANCODE_F5:
					pRenderer->ToggleRotation();
					break;

				case SDL_SCANCODE_F6:
					pRenderer->ToggleNormalMap();
					break;

				case SDL_SCANCODE_F7:
					pRenderer->CycleShadingMode();
					break;

				case SDL_SCANCODE_F:
					showFPS = !showFPS;
					break;
				}
				break;
			}
		}

		//--------- Update ---------
		pRenderer->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render();

		//--------- Timer ---------
		pTimer->Update();

		// fps
		if (showFPS)
		{
			printTimer += pTimer->GetElapsed();
			if (printTimer >= 1.f)
			{
				printTimer = 0.f;
				if (clearConsole) { std::cout << "\x1B[2J\x1B[H"; }
				std::cout << "dFPS: " << pTimer->GetdFPS() << "\n";
			}
		}

		//Save screenshot
		if (takeScreenshot)
		{
			takeScreenshot = false;
			if (pRenderer->SaveBufferToImage())
			{
				std::cout << "Something went wrong. Screenshot not saved!" << "\n";
			}
			else
			{
				std::cout << "Screenshot saved!" << "\n";
			}
		}
	}
	pTimer->Stop();

	SDL_DestroyWindow(pWindow);
	SDL_Quit();

	delete pTimer;
	delete pRenderer;

	return 0;
}