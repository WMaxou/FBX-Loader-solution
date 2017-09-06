#pragma once

#include "SDL.h"

class Window
{
public:
	Window();
	~Window();

	void Create(const char* Title, unsigned int Width, unsigned int Height);
	void Clear();
	bool IsOpen();

	void Close();

	SDL_Window& GetSDLWindow() const;

	static Window& Instance();
private:
	static Window& _instance;

	SDL_Window*		_SDLwindow;
	SDL_GLContext	_GLcontext;
	SDL_Event		_event;
};
