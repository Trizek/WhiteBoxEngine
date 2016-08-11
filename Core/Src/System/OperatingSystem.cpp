#include "System/OperatingSystem.h"

#include <SDL.h>
#include <GL/glew.h>
#include "GlobalVariables.h"
#include "Render/Renderer.h"
#include "Application.h"


WHITEBOX_BEGIN


void CTimer::Start()
{
#if defined(WIN32) || defined(WIN64)
	QueryPerformanceFrequency(&m_frequency);
	QueryPerformanceCounter(&m_startTime);
#endif
	clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &m_startTime );
}

void CTimer::Stop()
{
#if defined(WIN32) || defined(WIN64)
	QueryPerformanceCounter(&m_stopTime);
#else
	clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &m_stopTime );
#endif
}

float	CTimer::GetDuration() const
{
#if defined(WIN32) || defined(WIN64)
	return (float)(m_stopTime.QuadPart - m_startTime.QuadPart) / (float)m_frequency.QuadPart;
#else
	double duration = ( m_stopTime.tv_sec - m_startTime.tv_sec ) + ( m_stopTime.tv_nsec - m_startTime.tv_nsec ) / (double)1e9;
	return (float)duration;
#endif
}

struct SSDLData
{
	SSDLData()
	{
		m_sdlKeyMap[SDL_SCANCODE_F1] = Key::F1;
		m_sdlKeyMap[SDL_SCANCODE_F2] = Key::F2;
		m_sdlKeyMap[SDL_SCANCODE_F3] = Key::F3;
		m_sdlKeyMap[SDL_SCANCODE_F4] = Key::F4;
		m_sdlKeyMap[SDL_SCANCODE_F5] = Key::F5;

		m_sdlKeyMap[SDL_SCANCODE_W] = Key::Z;
		m_sdlKeyMap[SDL_SCANCODE_A] = Key::Q;
		m_sdlKeyMap[SDL_SCANCODE_S] = Key::S;
		m_sdlKeyMap[SDL_SCANCODE_D] = Key::D;

		ResetKeys();
	}

	void ResetKeys()
	{
		for (size_t i = 0; i < (size_t)Key::Count; ++i)
		{
			m_pressedKeys[i] = false;
			m_justPressedKeys[i] = false;
		}
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

	if (GLEW_OK != glewInit())
	{
		return;
	}

	gVars->pApplication->Init( width, height );	

	while (ProcessEvents(*this, static_cast<SSDLData*>(m_pSpecificData)))
	{
		gVars->pApplication->FrameUpdate();
		SDL_GL_SwapWindow(window);
	}

	//gVars->pRenderer->Reset();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void	COperatingSystem::Reshape( uint width, uint height )
{
	gVars->pApplication->Resize( width, height );
}

Vec2	COperatingSystem::GetMousePos()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	//y = m_height - y;

	return Vec2((float)x, (float)y);
}

bool	COperatingSystem::GetMouseButton( int button ) 
{
	int flag = 0;
	switch (button)
	{
	case 0:	flag = SDL_BUTTON_LEFT; break;
	case 1: flag = SDL_BUTTON_MIDDLE; break;
	case 2: flag = SDL_BUTTON_RIGHT; break;
	}

	return ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(flag)) != 0);
}

bool	COperatingSystem::IsPressingKey( Key key )
{ 
	return static_cast<SSDLData*>(m_pSpecificData)->m_pressedKeys[ (size_t)key ];
}

bool	COperatingSystem::JustPressedKey(Key key) { return false; }

WHITEBOX_END


/*

bool			ProcessEvents();
void			ResetKeys();




*/
