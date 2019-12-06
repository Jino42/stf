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



//CL CONTEXT REMTTRE OCGL
#include "PathManager.hpp"

#define BITS 32
//Number of buckets necessary
#define BUCK (1 << RADIX)
//Number of bits in the radix
#define RADIX 4
#define ARRLEN 64


void printArray(int *array) {
	std::cout << "CPU-----------------------" << std::endl;
	for (int i = 0; i < ARRLEN; i++) {
		std::cout << "[" << i << "][" << array[i] << "]" << std::endl;
	}
}
void radix() {

	ClError err;
	ClContext::Get();
	ClProgram::Get().addProgram(PathManager::Get().getPath("particleKernels") / "Test.cl");
	cl::CommandQueue queue(ClContext::Get().context, ClContext::Get().deviceDefault);



	//BUFFERS
	size_t globalWorkSize = 32;
	size_t groups = 8;
	size_t localWorkSize = globalWorkSize / groups;
	//////////
	int arrayCpu[ARRLEN];
	cl::Buffer arrayGpu(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(int) * ARRLEN);
	cl::Buffer histoBuffer(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(int) * BUCK * groups * localWorkSize);
	cl::Buffer scanBuffer(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(int) * BUCK * groups * localWorkSize);
	cl::Buffer blocksumBuffer(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(int) * groups);
	cl::Buffer outputBuffer(ClContext::Get().context, CL_MEM_READ_WRITE, sizeof(int) * ARRLEN);


	srand(time(NULL));
	for (int &i : arrayCpu) {
		i = rand() % 32;//(ARRLEN / 8);
	}
	arrayCpu[0] = 0;
	arrayCpu[1] = 0;
	arrayCpu[8] = 0;
	printArray(arrayCpu);


	//KERNELS
	cl::Kernel &kernelCount = ClProgram::Get().getKernel("count");
	cl::Kernel &kernelScan = ClProgram::Get().getKernel("scan");
	cl::Kernel &kernelCoalesce = ClProgram::Get().getKernel("coalesce");
	cl::Kernel &kernelReorder = ClProgram::Get().getKernel("reorder");
	cl::Kernel &kernelBlocksum = ClProgram::Get().getKernel("scan2");




	size_t maxWorkGroupsSize = ClContext::Get().deviceDefault.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
	size_t maxWorkItemSizeInWorkGroup = ClContext::Get().deviceDefault.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
	std::cout << "Max local memory : " << ClContext::Get().deviceDefault.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() << std::endl;
	std::cout << "maxWorkGroupsSize[" << maxWorkGroupsSize << "] maxWorkItemSize[" << maxWorkItemSizeInWorkGroup << "]" << std::endl;

	size_t globalWorkSizeMax = maxWorkGroupsSize * maxWorkItemSizeInWorkGroup;
	size_t localWorkSizeMax = maxWorkItemSizeInWorkGroup;


//
//			------Set kernels constants Args
//
	//Histo
	cl::LocalSpaceArg local_histo = cl::__local(sizeof(int) * BUCK * localWorkSize);
	err.err = kernelCount.setArg(2, local_histo); // Local Histogram
	int sizeArray = ARRLEN;
	err.err |= kernelCount.setArg(4, sizeArray);
	err.clCheckError();

	//SCAN
	size_t ScanGlobalWorkSize = (BUCK * groups * localWorkSize) / 2;
	size_t ScanLocalWorkSize = ScanGlobalWorkSize / groups;
	err.err = kernelScan.setArg(1, scanBuffer);
	cl::LocalSpaceArg local_scan = cl::__local(sizeof(int) * BUCK * localWorkSize);
	err.err |= kernelScan.setArg(2, local_scan);
	err.err |= kernelScan.setArg(3, blocksumBuffer);
	int doBlockSum2 = 1;
	err.err |= kernelScan.setArg(4, doBlockSum2);
	err.clCheckError();

	//Blocksum
	size_t BlocksumGlobalWorkSize = groups / 2;
	size_t BlocksumLocalWorkSize =  groups / 2;
	void* ptr = NULL;
	err.err = kernelBlocksum.setArg(0, blocksumBuffer);
	err.err |= kernelBlocksum.setArg(1, blocksumBuffer);

	cl::LocalSpaceArg local_bloclsum = cl::__local(sizeof(int) * groups);
	err.err |= kernelBlocksum.setArg(2, local_bloclsum);
	err.err |= kernelBlocksum.setArg(3, blocksumBuffer);
	int doBlockSum = 0;
	err.err |= kernelBlocksum.setArg(4, doBlockSum);
	err.clCheckError();


	//Coalesce
	size_t CoalesceGlobalWorkSize = (BUCK * localWorkSize * groups) / 2;
	size_t CoalesceLocalWorkSize = CoalesceGlobalWorkSize / groups;
	err.err = kernelCoalesce.setArg(0, scanBuffer);
	err.err |= kernelCoalesce.setArg(1, blocksumBuffer);
	err.clCheckError();

	//Reorder
	size_t ReorderGlobalWorkSize = groups * localWorkSize;
	size_t ReorderLocalWorkSize = groups;
	err.err = kernelReorder.setArg(1, scanBuffer);
	err.err |= kernelReorder.setArg(4, sizeArray);
	cl::LocalSpaceArg local_histo2 = cl::__local(sizeof(int) * BUCK * ReorderLocalWorkSize);
	err.err |= kernelReorder.setArg(5, local_histo2);
	err.clCheckError();

//
//			------
//



	//Write
	err.err = queue.enqueueWriteBuffer(arrayGpu, CL_TRUE, 0, sizeof(int) * ARRLEN, &arrayCpu);
	err.clCheckError();

	int pass = 0;
	for(pass = 0; pass < BITS/RADIX; pass++) {

		err.err = kernelCount.setArg(0, arrayGpu);
		err.err |= kernelCount.setArg(1, histoBuffer);
		err.err |= kernelCount.setArg(3, pass);
		err.clCheckError();


		std::cout << "start  enqueueNDRangeKernel kernelCount" << std::endl;

		err.err = queue.enqueueNDRangeKernel(kernelCount, cl::NullRange, cl::NDRange(globalWorkSize),
											 cl::NDRange(localWorkSize));
		err.clCheckError();
		queue.finish();
		std::cout << "end  enqueueNDRangeKernel kernelCount" << std::endl;
		//}

		int *countput;
		countput = new int[BUCK * groups * localWorkSize];
		err.err = queue.enqueueReadBuffer(histoBuffer, CL_TRUE, 0, sizeof(int) * BUCK * groups * localWorkSize,
										  countput);
		err.clCheckError();
		queue.finish();

		std::cout << "start  output histo" << std::endl;
		for (int i = 0; i < BUCK * groups * localWorkSize; i++)
			std::cout << i << " : " << countput[i] << std::endl;
		std::cout << "end  output histo" << std::endl;
		//////////



//
//			------SCAN
//
		kernelScan.setArg(0, histoBuffer);
		err.err = queue.enqueueNDRangeKernel(kernelScan, cl::NullRange, cl::NDRange(ScanGlobalWorkSize),
											 cl::NDRange(ScanLocalWorkSize));
		err.clCheckError();
		queue.finish();
		int *scanput = new int[BUCK * groups * localWorkSize];
		err.err = queue.enqueueReadBuffer(scanBuffer, CL_TRUE, 0, sizeof(int) * BUCK * groups * localWorkSize, scanput);
		err.clCheckError();
		queue.finish();

		std::cout << "start  output scanput" << std::endl;
		for (int i = 0; i < BUCK * groups * localWorkSize; i++)
			std::cout << i << " : " << scanput[i] << std::endl;
		std::cout << "end  output scanput" << std::endl;

		int *oblockput = new int[groups];
		err.err = queue.enqueueReadBuffer(blocksumBuffer, CL_TRUE, 0, sizeof(int) * groups, oblockput);
		err.clCheckError();
		queue.finish();


		std::cout << "start  output oblockput" << std::endl;
		for (int i = 0; i < groups; i++)
			std::cout << i << " : " << oblockput[i] << std::endl;
		std::cout << "end  output oblockput" << std::endl;

//
//			------BLOCKSUM
//

		err.err = queue.enqueueNDRangeKernel(kernelBlocksum, cl::NullRange, cl::NDRange(BlocksumGlobalWorkSize),
											 cl::NDRange(BlocksumLocalWorkSize));
		err.clCheckError();
		queue.finish();

		int *blockput = new int[groups];
		err.err = queue.enqueueReadBuffer(blocksumBuffer, CL_TRUE, 0, sizeof(int) * groups, blockput);
		err.clCheckError();
		queue.finish();

		std::cout << "start  output blockput" << std::endl;
		for (int i = 0; i < groups; i++)
			std::cout << i << " : " << blockput[i] << std::endl;
		std::cout << "end  output blockput" << std::endl;

//
//			------BLOCKSUM
//


		err.err = queue.enqueueNDRangeKernel(kernelCoalesce, cl::NullRange, cl::NDRange(CoalesceGlobalWorkSize),
											 cl::NDRange(CoalesceLocalWorkSize));
		err.clCheckError();
		queue.finish();
		int *coalput = new int[BUCK * localWorkSize * groups];
		err.err = queue.enqueueReadBuffer(scanBuffer, CL_TRUE, 0, sizeof(int) * BUCK * localWorkSize * groups, coalput);
		err.clCheckError();
		queue.finish();
		std::cout << "start  output coalput" << std::endl;
		for (int i = 0; i < BUCK * localWorkSize * groups; i++)
			std::cout << i << " : " << coalput[i] << std::endl;
		std::cout << "end  output coalput" << std::endl;

//
//			------REORDER
//

		//Reorder arguments
		err.err = kernelReorder.setArg(0, arrayGpu);
		err.err |= kernelReorder.setArg(2, outputBuffer);
		err.err |= kernelReorder.setArg(3, pass);
		err.clCheckError();

		std::cout << "ReorderGlobalWorkSize : " << ReorderGlobalWorkSize << std::endl;
		std::cout << "ReorderLocalWorkSize : " << ReorderLocalWorkSize << std::endl;
		err.err = queue.enqueueNDRangeKernel(kernelReorder, cl::NullRange, cl::NDRange(ReorderGlobalWorkSize),
											 cl::NDRange(ReorderLocalWorkSize));
		std::cout << "ReorderLocalWorkSize : " << ReorderLocalWorkSize << std::endl;
		err.clCheckError();
		std::cout << "ReorderLocalWorkSize : " << ReorderLocalWorkSize << std::endl;
		queue.finish();
		std::cout << "ReorderLocalWorkSize : " << ReorderLocalWorkSize << std::endl;


		int *output = new int[ARRLEN];
		err.err = queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(int) * ARRLEN, output);
		err.clCheckError();
		queue.finish();

		std::cout << "start  output output" << std::endl;
		for (int i = 0; i < ARRLEN; i++)
			std::cout << i << " : " << output[i] << std::endl;
		std::cout << "end  output output" << std::endl;





		/*
		//Swap current array with newest array
		cl_mem tmp = array_buffer;
		array_buffer = output_buffer;
		output_buffer = tmp;
		 */
		cl::Buffer tmp = arrayGpu;
		arrayGpu = outputBuffer;
		outputBuffer = tmp;
	}

	std::cout << "start  DEB output" << std::endl;
	for (int i = 0; i < ARRLEN; i++)
		std::cout << i << " : " << arrayCpu[i] << std::endl;
	std::cout << "end  DEB output" << std::endl;

	int *output = new int[ARRLEN];
	err.err = queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, sizeof(int) * ARRLEN, output);
	err.clCheckError();
	queue.finish();

	std::cout << "start  output output" << std::endl;
	for (int i = 0; i < ARRLEN; i++)
		std::cout << i << " : " << output[i] << std::endl;
	std::cout << "end  output output" << std::endl;
}

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

	try {
		radix();
	}
	catch (cl::Error const &e) {
		std::cout << e.what() << "(" << e.err() << ") [" << ClError::getErrorString(e.err()) << "]"  << std::endl;
	}
	catch (std::exception const &e) {
		std::cout << e.what() << std::endl;
	}

	return 0;

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