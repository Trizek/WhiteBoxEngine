#include "System/OperatingSystem.h"

#include <SDL.h>
#include <GL/glew.h>

WHITEBOX_BEGIN

struct SSDLData
{
	SSDLData()
	{
		m_sdlKeyMap[SDL_SCANCODE_F1] = Key::F1;
		m_sdlKeyMap[SDL_SCANCODE_F2] = Key::F2;
		m_sdlKeyMap[SDL_SCANCODE_F3] = Key::F3;
		m_sdlKeyMap[SDL_SCANCODE_F4] = Key::F4;
		m_sdlKeyMap[SDL_SCANCODE_F5] = Key::F5;
	}

	std::unordered_map<unsigned int, Key>	m_sdlKeyMap;
	bool									m_pressedKeys[(size_t)Key::Count];
	bool									m_justPressedKeys[(size_t)Key::Count];
};

bool ProcessEvents( COperatingSystem& os, SSDLData* pSDLData )
{
	/*for (size_t i = 0; i < (size_t)Key::Count; ++i)
	{
		m_justPressedKeys[i] = false;
	}*/

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return false;

		case SDL_WINDOWEVENT:
		{
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				os.Reshape(event.window.data1, event.window.data2);
				break;
			}
		}
		break;

		case SDL_KEYDOWN:
		{
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
			{
				return false;
			}

			auto itSdlKey = pSDLData->m_sdlKeyMap.find(event.key.keysym.scancode);
			if (itSdlKey != pSDLData->m_sdlKeyMap.end())
			{
				pSDLData->m_justPressedKeys[(size_t)itSdlKey->second] = !pSDLData->m_pressedKeys[(size_t)itSdlKey->second];
				pSDLData->m_pressedKeys[(size_t)itSdlKey->second] = true;
			}
		}
		break;

		case SDL_KEYUP:
		{
			auto itSdlKey = pSDLData->m_sdlKeyMap.find(event.key.keysym.scancode);
			if (itSdlKey != pSDLData->m_sdlKeyMap.end())
			{
				pSDLData->m_pressedKeys[(size_t)itSdlKey->second] = false;
			}
		}
		break;
		}
	}

	return true;
}

COperatingSystem::COperatingSystem()
{
	m_pSpecificData = new SSDLData();
}

COperatingSystem::~COperatingSystem()
{
	delete static_cast<SSDLData*>(m_pSpecificData);
}

void	COperatingSystem::Init(uint width, uint height)
{
	SDL_Window*		window;
	SDL_GLContext	context;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return;
	}

	/* Turn on double buffering with a 24bit Z buffer.
	* You may need to change this to 16 or 32 for your system */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	window = SDL_CreateWindow("App", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)width, (int)height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == nullptr)
	{
		return;
	}

	context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(0);

	//gVars->pRenderer->Init();

	while (ProcessEvents(*this, static_cast<SSDLData*>(m_pSpecificData)))
	{
		//gVars->pRenderer->Update();
		SDL_GL_SwapWindow(window);
	}

	//gVars->pRenderer->Reset();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void	COperatingSystem::Reshape(uint width, uint height){}

Vec2	COperatingSystem::GetMousePos() { return Vec2(); }
bool	COperatingSystem::GetMouseButton(int button) { return false;  }
bool	COperatingSystem::IsPressingKey(Key key) { return false; }
bool	COperatingSystem::JustPressedKey(Key key) { return false; }

WHITEBOX_END


/*

bool			ProcessEvents();
void			ResetKeys();




*/