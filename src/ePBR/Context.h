#include <string>

#include <SDL2/SDL.h>

// GUI system: https://github.com/ocornut/imgui
// - prevent compile error by building with: WINDOWS_IGNORE_PACKING_MISMATCH
#include <imgui/imgui.h>

namespace ePBR 
{
	class Context 
	{
	private:
		SDL_Window* m_window;
		SDL_Renderer* m_SDL_Renderer;
		SDL_GLContext m_SDL_GL_Context;

		void InitGL();
		void InitSDL();
		void InitSDL_GL();
		void InitImGui();
	public:
		void Test(std::string _pwd);

		Context();
	};
}