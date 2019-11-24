#include <iostream>
#include "Engine/MainGraphic.hpp"
#include <chrono>
#include "Cl/ClContext.hpp"
#include "Cl/ClProgram.hpp"
#include "Particle/ParticleSystem.hpp"
#include "Engine/Time.hpp"
#include "Gui/Gui.hpp"
#include "Gui/WidgetOption.hpp"
#include "Gui/WidgetRender.hpp"
#include "Gui/WidgetEditor.hpp"
#include "NTL_Debug.hpp"
#include <Engine/ShaderManager.hpp>
#include <noise/noise.h>
#include "noiseutils.h"
#include <Engine/CameraManager.hpp>
#ifdef __cplusplus
extern "C" {
#endif
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#ifdef __cplusplus
}
#endif

void demoGui() {
    Gui gui;

    gui.setDemo(true);
    WidgetEditor widgetEditor;

    while (!DisplayWindow::Get().exit()) {
        //Update
        DisplayWindow::Get().update();
        gui.update();


        ImGui::SetNextWindowPos(gui.positionByPercent(ImVec2(0, 0)));
        ImGui::SetNextWindowSize(gui.positionByPercent(ImVec2(50, 100)));
        widgetEditor.render(true);
        gui.render();
        DisplayWindow::Get().render();
        DisplayWindow::Get().clear();
    }

}

int displaySum(lua_State *L)
{
	int sum = (int) lua_tonumber(L, 1) + (int) lua_tonumber(L, 2);
	printf("La somme est egale a : %d", sum);
	return 0 ;
}

void Lua() {
	lua_State *lua = luaL_newstate();
	luaL_openlibs(lua);
	luaL_dofile(lua, (std::string(ROOT_PATH) + "/script.lua").c_str());
	lua_getglobal(lua, "display_sum");
// Enregistrement dans la fonction dans Lua
	lua_register(lua, "displaySum", displaySum);
	lua_call(lua, 0, 0);
	lua_close(lua);
	return ;
}


int main(int argc, char **argv) {
//	Lua();
//	return 0;
    noise::module::Perlin myModule;
/*
    utils::NoiseMap heightMap;
    utils::NoiseMapBuilderPlane heightMapBuilder;
    heightMapBuilder.SetSourceModule (myModule);
    heightMapBuilder.SetDestNoiseMap (heightMap);
    heightMapBuilder.SetDestSize (256, 256);
    heightMapBuilder.SetBounds (2.0, 6.0, 1.0, 5.0);
    heightMapBuilder.Build ();

    utils::RendererImage renderer;
    utils::Image image;
    renderer.SetSourceNoiseMap (heightMap);
    renderer.SetDestImage (image);
    renderer.Render ();

    utils::WriterBMP writer;
    writer.SetSourceImage (image);
    writer.SetDestFilename ("tutorial.bmp");
    writer.WriteDestFile ();
*/
	srand(time(NULL));
	try {
		std::string windowTitle = "ft_vox";
		DisplayWindow::Init(windowTitle.c_str(), 1024, 720);

		if (argc > 1 && !strcmp(argv[1], "-gd")) {
		    demoGui();
		    return 0;
        }

		Gui gui;
        // WidgetOption widgetOption;
        WidgetEditor widgetEditor;
        WidgetRender &widgetRender = WidgetRender::Get();

		ClContext::Get();
		MainGraphic::Get();
		ShaderManager::Get();
		CameraManager::Get();

		CameraManager::Get().init();
		MainGraphic::Get().init();

		int fpsCount = 0;
		std::chrono::time_point<std::chrono::system_clock> time_fps = std::chrono::system_clock::now();
        bool stopAllFrame = false;

        boost::filesystem::path pathRoot(ROOT_PATH);


        ShaderManager::Get().addShader("debugWireFrame");
        ShaderManager::Get().getShader("debugWireFrame").attach((pathRoot / "shader" / "debugWireFrame.vert").generic_string());
        ShaderManager::Get().getShader("debugWireFrame").attach((pathRoot / "shader" / "debugWireFrame.frag").generic_string());
        //ShaderManager::Get().getShader("debugWireFrame").attach((pathRoot / "shader" / "debugWireFrame.geom").generic_string());
        ShaderManager::Get().getShader("debugWireFrame").link();


		CameraManager::Get().addCamera("DebguFrustum");

        while (!DisplayWindow::Get().exit()) {
			//Update
            Time::Get().update();
            DisplayWindow::Get().update();
			CameraManager::Get().update();
            gui.update();

            //Pause management
            if (DisplayWindow::Get().getKey(GLFW_KEY_K) == KeyState::kDown)
                stopAllFrame = !stopAllFrame;
            if (DisplayWindow::Get().getKey(GLFW_KEY_L) == KeyState::kDown)
                Time::Get().pause(!Time::Get().isPause());
            else if (stopAllFrame && !Time::Get().isPause())
                Time::Get().pause(!Time::Get().isPause());
            if (!Time::Get().isPause())
                MainGraphic::Get().update();


			if (DisplayWindow::Get().getKey(GLFW_KEY_Z) == KeyState::kDown)
				CameraManager::Get().setFocus("Default");
			if (DisplayWindow::Get().getKey(GLFW_KEY_X) == KeyState::kDown)
				CameraManager::Get().setFocus("DebguFrustum");
            if (DisplayWindow::Get().getKey(GLFW_KEY_Y) == KeyState::kDown)
                CameraManager::Get().setFocus("Voxel");
			if (DisplayWindow::Get().getKey(GLFW_KEY_C) == KeyState::kDown)
				Camera::focus->getFrustum().setDebug(!Camera::focus->getFrustum().isDebug());

			//Logical Loop
			while (Time::Get().shouldUpdateLogic()) {
			}

			//Render
            if (!Time::Get().isPause())
                MainGraphic::Get().render();
            DisplayWindow::Get().render();
            DisplayWindow::Get().clear();

            //FPS Count
            fpsCount++;
			if (std::chrono::system_clock::now() - time_fps > std::chrono::seconds(1)) {
				// Disabled to avoid flooding stdout
				// std::cout << fpsCount << "fps" << std::endl;
				windowTitle = "ft_vox | " + std::to_string(fpsCount) + " fps";
				glfwSetWindowTitle(DisplayWindow::Get().getWindow(), windowTitle.c_str());
				time_fps = std::chrono::system_clock::now();
				std::cout << fpsCount << "fps" << std::endl;
                fpsCount = 0;
            }

			//Reload OpenCL Kernel
			if (DisplayWindow::Get().getKey(GLFW_KEY_R) == KeyState::kDown) {
				try {
					ClProgram::Get().buildProgram();
				} catch (ClProgram::BuildException const &e) {
					std::cerr << e.what() << std::endl;
				}
			}
			//Reload Opengl Shader
            if (DisplayWindow::Get().getKey(GLFW_KEY_F) == KeyState::kDown) {
                try {
                    ShaderManager::Get().reload();
                } catch (std::exception const &e) {
                    Time::Get().pause(true);
                    std::cerr << e.what() << std::endl;
                }
            }

			// GUI
            // widgetOption.render(true);
            ImGui::SetNextWindowPos(gui.positionByPercent(ImVec2(0, 0)));
            ImGui::SetNextWindowSize(gui.positionByPercent(ImVec2(50, 100)));
            WidgetRender::Get().render(true);
            ImGui::SetNextWindowPos(gui.positionByPercent(ImVec2(50, 0)));
            ImGui::SetNextWindowSize(gui.positionByPercent(ImVec2(50, 100)));
            widgetEditor.render(true);
			gui.render();
        }
	}
	catch (cl::Error const &e) {
		std::cout << e.what() << "(" << e.err() << ") [" << ClError::getErrorString(e.err()) << "]"  << std::endl;
	}
	catch (std::exception const &e) {
		std::cerr << e.what() << std::endl;
	}

    return 0;

}