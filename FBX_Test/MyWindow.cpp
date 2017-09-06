#include "MyWindow.h"

#include "Driver.h"
#include "GL\glew.h"

#include <iostream>

Window& Window::_instance = Window();
	
void Window::Create(const char * Title, unsigned int Width, unsigned int Height)
{
	glGetError();

	_SDLwindow = SDL_CreateWindow(Title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_OPENGL);

	SDL_assert(_SDLwindow != nullptr && "Failed to create Window: ");

	_GLcontext = SDL_GL_CreateContext(_SDLwindow);

	Driver::Instance().Init();
}

void Window::Clear()
{
	SDL_GL_SwapWindow(_SDLwindow);
}

bool Window::IsOpen()
{
	while (SDL_PollEvent(&_event))
	{
		switch (_event.type)
		{
		case SDL_QUIT: 
			return false; 

		default: break;
		}
	}

	return true;
}

void Window::Close()
{
	SDL_Quit();
}

SDL_Window& Window::GetSDLWindow() const
{
	return *_SDLwindow;
}

Window& Window::Instance()
{
	return _instance;
}

Window::Window()
	: _SDLwindow(nullptr)
{
	SDL_Init(SDL_INIT_VIDEO);
}

Window::~Window()
{
	SDL_GL_DeleteContext(_GLcontext);
	_SDLwindow = nullptr;
}
