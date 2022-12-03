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

		int m_windowWidth;
		int m_windowHeight;

		bool m_initialised;

		void InitGL();
		void InitSDL();
		void InitSDL_GL();
		void InitImGui();
	public:
		void Test(std::string _pwd);
		void Init(SDL_Window* _window);

		/// @brief Maximise the window. Updates window width and height variables which can be used to update Renderers and RenderTextures
		void MaximiseWindow();

		int GetWindowWidth() const { return m_windowWidth; }
		int GetWindowHeight() const { return m_windowHeight; }

		Context();
		~Context();
	};
}