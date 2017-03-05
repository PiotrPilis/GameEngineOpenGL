#define _USING_V110_SDK71_

#include <Windows.h>
#include <iostream>
#include <utility>

#include "config.hpp"
#include "event.hpp"
#include "shader.hpp"
#include "log.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "terrain.hpp"
#include "light.hpp"
#include "objectmanager.hpp"
#include "math.hpp"

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void VSync(bool sync)
{
	// Function pointer for the wgl extention function we need to enable/disable
	// vsync
	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

	const char *extensions = (char*)glGetString(GL_EXTENSIONS);

	if (strstr(extensions, "WGL_EXT_swap_control") == 0)
	{
		return;
	}
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(sync);
	}
}

using namespace std;

namespace GameEngine
{
	math::Vector2f lastMousePos;
	GLboolean mouseButtonRightClicked = GL_FALSE;

	class GameManager
	{
	private:
		GameManager(const GameManager&) = delete;
		GameManager& operator =(const GameManager&) = delete;
		GameManager()
		{
			ErrorLog::Write("", false, true);
			stringstream sout;
			sout << "OpenGL initialized\nRenderer: " << glGetString(GL_RENDERER) << "\nVersion " << glGetString(GL_VERSION) << "GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
			NormalLog::Write(sout, false, true);

			VSync(false);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);

			LoadShaders();

			delete Terrain::light;
			Terrain::light = new Light();

			Camera::Instance().SetPosition(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(10.0f, 50.0f, 20.0F)));
			//Camera::Instance().SetPosition(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(8.0f, 21.0f, -23.0f)));
			//Camera::Instance().Rotate(1.8f, 2.0f);
			//Camera::Instance().SetDirection(math::Vector3f(-0.07, -0.44, 0.9));


			//Camera::Instance().SetPosition(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(100.0f, 700.0f, 100.0F) + Camera::Instance().Position()));
			//Camera::Instance().Rotate(-0.2f, 3.0f);

			ObjectManager::Instance().Initialize(Camera::Instance());

			// terrain
			terrain = new Terrain(&Camera::Instance(), "Data/Terrain/Map1");

			glPolygonMode(GL_FRONT, GL_FILL);
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//Camera::Instance().BindBuffer(*Terrain::light->shadowMap.GetProgram());

			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);

			CheckError();
		}
		~GameManager()
		{
			delete GameEngine::Terrain::terrainVS;
			delete GameEngine::Terrain::terrainCS;
			delete GameEngine::Terrain::terrainES;
			delete GameEngine::Terrain::terrainFS;
			delete GameEngine::Terrain::terrainPO;

			delete GameEngine::Terrain::light;

			delete terrain;
		}

		void LoadShaders()
		{
			// Terrain
			Terrain::terrainVS = new Shader("Data/Shaders/terrain.vs", TypeShader::VertexShader);
			Terrain::terrainCS = new Shader("Data/Shaders/terrain.cs", TypeShader::TessellationControlShader);
			Terrain::terrainES = new Shader("Data/Shaders/terrain.es", TypeShader::TessellationEvaluationShader);
			Terrain::terrainFS = new Shader("Data/Shaders/terrain.fs", TypeShader::FragmentShader);

			Terrain::terrainPO = new Program();
			Terrain::terrainPO->Initialize(*Terrain::terrainVS, *Terrain::terrainCS, *Terrain::terrainES, *Terrain::terrainFS);
		}
	public:

		void RenderScene()
		{
			Camera::Instance().Update();
			Terrain::light->UpdateLightAndShadow();
			Time::GlobalTime().Update();

			// -> ShadowMap rendering
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);				// gdzieœ nie wy³¹czam bufora - przez co jak wyrzuce z renderowania cala ta funckje nic sie nie renderuje ^.^ 
			RenderShadowMap();

			// -> Normal rendering
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			
			ObjectManager::Instance().Render();
			terrain->Render();
		}
	private:
		void RenderShadowMap()
		{
			glCullFace(GL_FRONT);
			Terrain::light->shadowMap.Enable();
			
			for (unsigned i = 0; i < mNumCascadeShadow; ++i)
			{
				Terrain::light->shadowMap.BindForWriting(i);
				
				glClear(GL_DEPTH_BUFFER_BIT);
				//Terrain::light->shadowMap.SetVP(Camera::Instance().transform.viewProjectionMatrix);
				Terrain::light->shadowMap.SetVP(Terrain::light->ShadowProjectionMatrix(i));

				//terrain->RenderForShadow();
				ObjectManager::Instance().RenderForShadow(Terrain::light->shadowMap.WorldMatrixLocation());
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);				// gdzieœ nie wy³¹czam bufora - przez co jak wyrzuce z renderowania cala ta funckje nic sie nie renderuje ^.^

			/*glUseProgram(*Terrain::light->shadowMap.GetProgram());
			
			Terrain::light->shadowMap.SetVP(Terrain::light->ShadowProjectionMatrix(1));
			ObjectManager::Instance().RenderForShadow(Terrain::light->shadowMap.WorldMatrixLocation());*/
			glCullFace(GL_BACK);
		}
	public:
		void CheckError()
		{
			unsigned err(glGetError());

			while (err != GL_NO_ERROR) {
				string error;

				switch (err) {
				case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
				case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
				case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
				case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
				}

				CriticalLog::Write("GL_" + error, error);
				err = glGetError();
			}
		}
		static GameManager& Instance()
		{
			static GameManager gameManager;
			return gameManager;
		}

		friend class WindowGLFW;
	private:
		Shader* meshVS;
		Shader* meshFS;
		Program* meshPO;

		GLuint VAO, VBO;

		Terrain* terrain;

	};

	class Window
	{
	public:
		virtual void WindowLoop() = 0;

		virtual const pair<int, int> GetWindowSize() = 0;
	//private:
		virtual void InitWindow() = 0;
	private:
		virtual void DestroyWindow() = 0;
		virtual void Keyboard() = 0;

		
	};

	class WindowGLFW : public Window
	{
	private:
		WindowGLFW(const WindowGLFW&);
		WindowGLFW& operator =(const WindowGLFW&);
		WindowGLFW() : window(0), monitor(0)
		{
		}
		~WindowGLFW()
		{
			DestroyWindow();
		}
		void DestroyWindow()
		{
			glfwDestroyWindow(window);
			glfwTerminate();
		}
	public:
		void InitWindow()
		{
			window = glfwCreateWindow(mWidthWindow, mHeightWindow, "Alpha Version", NULL, NULL);
			if (!window)
				CriticalLog::Write("could not open window", "could not open window");
			glfwMakeContextCurrent(window);
			monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* vmode = glfwGetVideoMode(monitor);
			//glfwSetKeyCallback(window, KeyCallback);
			glfwSetCursorPosCallback(window, PassiveMouseCallback);
			glfwSetMouseButtonCallback(window, MouseCallback);
			glewInit();
		}

	
		void WindowLoop()
		{
			Timer FPStimer;
			unsigned int NumKeys = 0;
			while (!glfwWindowShouldClose(window)) {
				if (FPStimer.GetElapsedSeconds() >= 1.0)
				{
					std::stringstream sout;
					sout << "FPS: " << NumKeys << "; DirLight: " << GameEngine::Terrain::light->GetDirection();
					glfwSetWindowTitle(window, sout.str().c_str());

					FPStimer.ResetTimer();
					NumKeys = 0;
				}
				else
					NumKeys++;
		
				Keyboard();
				GameEngine::GameManager::Instance().RenderScene();

				glfwSwapBuffers(window);
				glfwPollEvents();
			}
		}
		const pair<int, int> GetWindowSize()
		{
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			return pair<int, int>(width, height);
		}

		static WindowGLFW& Instance()
		{
			static WindowGLFW window;
			return window;
		}


		/*static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{

		}*/
	private:
		static void PassiveMouseCallback(GLFWwindow* window, double x, double y)
		{
			if (mouseButtonRightClicked)
			{
				float rx = (x - lastMousePos.x()) / mWidthWindow;
				float ry = (y - lastMousePos.y()) / mHeightWindow;

				Camera::Instance().Rotate(rx*100, ry*100);

				lastMousePos = math::Vector2f(x, y);
			}
		}

		static void MouseCallback(GLFWwindow* window, int button, int action, int mods)
		{
			double x, y;
			glfwGetCursorPos(window, &x, &y);

			if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				lastMousePos = math::Vector2f(x, y);
				mouseButtonRightClicked = action == GLFW_PRESS ? GL_TRUE : GL_FALSE;
			}

		}
		void Keyboard()
		{
			if (glfwGetKey(window, GLFW_KEY_A)) {
				Camera::Instance().SetPosition(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(50.0f * timer.GetElapsedSeconds(), 0.0f, 0.0f) + Camera::Instance().Position()));
			}
			else if (glfwGetKey(window, GLFW_KEY_D)) {
				Camera::Instance().SetPosition(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(-50.0f * timer.GetElapsedSeconds(), 0.0f, 0.0f) + Camera::Instance().Position()));
			}
			else if (glfwGetKey(window, GLFW_KEY_F)) {
				Camera::Instance().SetPosition(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(0.0f, 0.0f, 15.0f * timer.GetElapsedSeconds()) + Camera::Instance().Position()));
			}
			else if (glfwGetKey(window, GLFW_KEY_R)) {
				Camera::Instance().SetPosition(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(0.0f, 0.0f, -15.0f * timer.GetElapsedSeconds()) + Camera::Instance().Position()));
			}
			else if (glfwGetKey(window, GLFW_KEY_W)) {
				Camera::Instance().SetPosition(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(0.0f, 50.0f * timer.GetElapsedSeconds(), 0.0f) + Camera::Instance().Position()));
			}
			else if (glfwGetKey(window, GLFW_KEY_S)) {
				Camera::Instance().SetPosition(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(Eigen::Matrix<float, 3, 1, Eigen::DontAlign>(0.0f, -50.0f * timer.GetElapsedSeconds(), 0.0f) + Camera::Instance().Position()));
			}

			// light
			else if (glfwGetKey(window, GLFW_KEY_J)) {
				Terrain::light->SetPosition(math::Vector3f(Camera::Instance().Position().x(), Camera::Instance().Position().y(), -Camera::Instance().Position().z()));
			}
			else if (glfwGetKey(window, GLFW_KEY_T)) {
				Terrain::light->SetColor(math::Vector3f(1.0f, 0.0f, 0.0f));
			}
			else if (glfwGetKey(window, GLFW_KEY_Y)) {
				Terrain::light->SetColor(math::Vector3f(1.0f, 1.0f, 1.0f));
			}
	
			timer.ResetTimer();
		}
		GLFWwindow* window;
		GLFWmonitor* monitor;

		Timer timer;

	};
}
#define	M_PI	3.14159265358979323846

#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
int main(int argc, char *argv[])
{
	if (!glfwInit()) {
		MessageBox(NULL, "FATAL ERROR: could not start GLFW3", NULL, MB_ICONHAND | MB_OK);
		exit(EXIT_FAILURE);
	}
	GameEngine::WindowGLFW::Instance().InitWindow();
	GameEngine::WindowGLFW::Instance().WindowLoop();

	return 0;
}