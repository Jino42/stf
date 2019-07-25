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

int main() {
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

		Gui gui;
        // WidgetOption widgetOption;
        WidgetEditor widgetEditor;
        WidgetRender &widgetRender = WidgetRender::Get();
		
		ClContext::Get();

		int fpsCount = 0;
		std::chrono::time_point<std::chrono::system_clock> time_fps = std::chrono::system_clock::now();
        bool stopAllFrame = false;

        boost::filesystem::path pathRoot(ROOT_PATH);

        ShaderManager::Get().addShader("debugWireFrame");
        ShaderManager::Get().getShader("debugWireFrame").attach((pathRoot / "shader" / "debugWireFrame.vert").generic_string());
        ShaderManager::Get().getShader("debugWireFrame").attach((pathRoot / "shader" / "debugWireFrame.frag").generic_string());
        //ShaderManager::Get().getShader("debugWireFrame").attach((pathRoot / "shader" / "debugWireFrame.geom").generic_string());
        ShaderManager::Get().getShader("debugWireFrame").link();

		MainGraphic::Get().init();

        while (!DisplayWindow::Get().exit()) {

			//Update
			std::cout << 1 << std::endl;
            Time::Get().update();
            DisplayWindow::Get().update();
            gui.update();

			std::cout << 2 << std::endl;
            //Pause management
            if (DisplayWindow::Get().getKey(GLFW_KEY_K) == KeyState::kDown)
                stopAllFrame = !stopAllFrame;
            if (DisplayWindow::Get().getKey(GLFW_KEY_L) == KeyState::kDown)
                Time::Get().pause(!Time::Get().isPause());
            else if (stopAllFrame && !Time::Get().isPause())
                Time::Get().pause(!Time::Get().isPause());
            if (!Time::Get().isPause())
                MainGraphic::Get().update();



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