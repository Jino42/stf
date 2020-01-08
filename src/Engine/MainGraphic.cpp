#include "MainGraphic.hpp"
#include "Debug.hpp"
#include "Engine/CameraManager.hpp"
#include "Engine/Display/DisplayWindow.hpp"
#include "Particle/ParticleSystemManager.hpp"
#include "Time.hpp"
#include "json/JsonParticleParser.hpp"
#include <Engine/ModelEngine/MainGraphicExtendModel.hpp>
#include <Engine/TestParticle.hpp>
#include <Voxel/VoxelWorld.hpp>

/// Rmove
#include "Cl/ClProgram.hpp"
#include "PathManager.hpp"
#include "ShaderManager.hpp"
#include "Particle/ParticleModule/ModuleSPH.hpp"

MainGraphic::MainGraphic()
    : renderBuffer_(DisplayWindow::Get().getWidthWindow() * 0.5f,
                    DisplayWindow::Get().getHeightWindow()),
      renderBufferRayMarch_(DisplayWindow::Get().getWidthWindow() * 0.3f,
                            DisplayWindow::Get().getHeightWindow()),
      deltaTime_(0.014f), doParticle_(true) {
    glEnable(GL_DEPTH_TEST);
    glPointSize(1.0f);
    glLineWidth(3.0f);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glDepthMask(GL_FALSE);

    PathManager::Get();
}

MainGraphic &MainGraphic::Get() {
    if (!instance_)
        instance_ = std::make_unique<MainGraphic>();
    return *MainGraphic::instance_;
}

void MainGraphic::init() {
    CameraManager::Get().getFocus().setPosition(glm::vec3(2.7f, 38.f, 55.f));

    glGenTextures(1, &rayMarchTexture_);
    glBindTexture(GL_TEXTURE_2D, rayMarchTexture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DisplayWindow::Get().getWidthWindow() / 3.f, DisplayWindow::Get().getHeightWindow(), 0, GL_RGBA, GL_FLOAT, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    ClError err;
    rayMarchImage_ = cl::ImageGL(ClContext::Get().context, CL_MEM_READ_WRITE, GL_TEXTURE_2D, 0, rayMarchTexture_, &err.err);
    err.clCheckError();
    ClProgram::Get().addProgram(PathManager::Get().getPath("particleKernels") / "RayMarchSPH.cl");
    ShaderManager::Get().addShader("renderInRect");
    ShaderManager::Get().getShader("renderInRect").attach((PathManager::Get().getPath("shaders") / "renderInRect.vert").generic_string());
    ShaderManager::Get().getShader("renderInRect").attach((PathManager::Get().getPath("shaders") / "renderInRect.frag").generic_string());
    ShaderManager::Get().getShader("renderInRect").link();
    float vertices[] = {
        // positions          // texture coords
         1.0f,  1.0f,  0.0f,        1.0f, 1.0f,   // top right
         1.0f, -1.0f,  0.0f,        1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f,  0.0f,        0.0f, 0.0f, // bottom left
        -1.0f,  1.0f,  0.0f,        0.0f, 1.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    memcpy(vertices_, vertices, sizeof(float) * 20);
    memcpy(indices_, indices, sizeof(float) * 6);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_), indices_, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    MainGraphicExtendModel::Get();
    if (doParticle_)
        TestParticle::Get().init();
    CameraManager::Get().addCamera("Voxel");
    CameraManager::Get().getCamera("Voxel").setFar(682.0f);
    CameraManager::Get().getCamera("Voxel").setFov(59.7f);
    CameraManager::Get().getCamera("Voxel").setPosition(
        glm::vec3(-18.5f, 140.35f, 167.15f));
    // VoxelWorld::Init(CameraManager::Get().getCamera("Voxel"));
    // VoxelWorld::Get().start();

    CameraManager::Get().getCamera("Default").setFar(1000.0f);
    CameraManager::Get().getCamera("Default").setPosition(
    glm::vec3(60, 80.0f, 86.15f));

    if (doParticle_) {
        JsonParticleParser lol(PathManager::Get().getPath("scene") /
                               "Particles.json");
        lol.parse();
        ParticleSystemManager::Get().initAllParticleSystem();
        }
}

void MainGraphic::render() {
    if (DisplayWindow::Get().getKey(GLFW_KEY_RIGHT) ||
        DisplayWindow::Get().getKey(GLFW_KEY_D))
        Camera::focus->processPosition(Camera::Movement::RIGHT, deltaTime_ * 5);
    if (DisplayWindow::Get().getKey(GLFW_KEY_LEFT) ||
        DisplayWindow::Get().getKey(GLFW_KEY_A))
        Camera::focus->processPosition(Camera::Movement::LEFT, deltaTime_ * 5);
    if (DisplayWindow::Get().getKey(GLFW_KEY_DOWN) ||
        DisplayWindow::Get().getKey(GLFW_KEY_S))
        Camera::focus->processPosition(Camera::Movement::BACKWARD, deltaTime_ * 5);
    if (DisplayWindow::Get().getKey(GLFW_KEY_UP) ||
        DisplayWindow::Get().getKey(GLFW_KEY_W))
        Camera::focus->processPosition(Camera::Movement::FORWARD, deltaTime_ * 5);
    if (DisplayWindow::Get().getKey(GLFW_KEY_RIGHT_CONTROL) ||
        DisplayWindow::Get().getKey(GLFW_KEY_Q))
        Camera::focus->processPosition(Camera::Movement::DOWN, deltaTime_ * 5);
    if (DisplayWindow::Get().getKey(GLFW_KEY_RIGHT_SHIFT) ||
        DisplayWindow::Get().getKey(GLFW_KEY_E))
        Camera::focus->processPosition(Camera::Movement::UP, deltaTime_ * 5);

    if (DisplayWindow::Get().getKey(GLFW_KEY_V))
        Debug::Get().flipDebug("ParticleModule");

    if (DisplayWindow::Get().getKey(GLFW_KEY_T)) {
        ParticleSystemManager::Get().deleteAllParticleSystem();
        JsonParticleParser lol(PathManager::Get().getPath("scene") /
                               "Particles.json");
        lol.parse();
        ParticleSystemManager::Get().initAllParticleSystem();
    }
    if (DisplayWindow::Get().getKey(GLFW_KEY_G)) {
        ParticleSystemManager::Get().getParticleSystem("abc").reload();
    }

    renderBufferRayMarch_.bind();
    renderBufferRayMarch_.clear();

    //do raymarch
    ClError err;

    cl::CommandQueue queue(ClContext::Get().context, ClContext::Get().deviceDefault);
    cl::Kernel kernel(ClProgram::Get().getKernel("testKernelWrite"));
    int width = DisplayWindow::Get().getWidthWindow() / 3.f;
    int height = DisplayWindow::Get().getHeightWindow();
    int range = width * height;
    std::cout << "OKAKR " << range << std::endl;
    std::cout << "OKAKW " << width << std::endl;
    std::cout << "OKAKH " << height << std::endl;

    err.err = kernel.setArg(0, rayMarchImage_);
    err.err |= kernel.setArg(1, ParticleSystemManager::Get().getParticleSystem("abc").getEmitter("Lol0").getModule<ModuleSPH>()->getOCGL_BufferParticles_SPH_Data().mem);
    err.err |= kernel.setArg(2, ParticleSystemManager::Get().getParticleSystem("abc").getEmitter("Lol0").getModule<ModuleSPH>()->getGpuBufferModuleParam());
    err.err |= kernel.setArg(3, width);
    err.err |= kernel.setArg(4, height);
    float time = static_cast<float>(Time::Get().sinceWorldStartProgram.count<std::chrono::milliseconds>()) / 1000.0f;
    err.err |= kernel.setArg(5, time);
    err.err |= kernel.setArg(6, glmVec3toClFloat3(Camera::focus->getPosition()));
    err.err |= kernel.setArg(7, glmVec3toClFloat3(Camera::focus->getPosition() + Camera::focus->getFront()));
    err.clCheckError();


    std::vector<cl::Memory> cl_vbos;
    cl_vbos.push_back(rayMarchImage_);
    cl_vbos.push_back(ParticleSystemManager::Get().getParticleSystem("abc").getEmitter("Lol0").getModule<ModuleSPH>()->getOCGL_BufferParticles_SPH_Data().mem);

    cl::Event ev;
    glFinish();
    /*
        err.err = queue.enqueueAcquireGLObjects(&cl_vbos, nullptr, &ev);
        ev.wait();
        err.clCheckError();
        queue.finish();
        err.err = queue.enqueueNDRangeKernel(kernel, cl::NullRange,
                                             cl::NDRange(width - 1, height),
                                             cl::NDRange(20, 20));
        err.clCheckError();
        queue.finish();
        err.err = queue.enqueueReleaseGLObjects(&cl_vbos, nullptr, nullptr);
        err.clCheckError();
        queue.finish();

        ShaderManager::Get().getShader("renderInRect").setInt("texture1", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rayMarchTexture_);
        glGenerateMipmap(GL_TEXTURE_2D);
        ShaderManager::Get().getShader("renderInRect").activate();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    */
    renderBuffer_.bind();
    renderBuffer_.clear();

    MainGraphicExtendModel::Get().update(0.014f);

    if (doParticle_) {
        TestParticle::Get().update(0.014f);

        ParticleSystemManager::Get().updateAllParticleSystem(0.014f);
        ParticleSystemManager::Get().renderAllParticleSystem();
    }
    // VoxelWorld::Get().update();
    // VoxelWorld::Get().render();

    debug_.render();

    renderBuffer_.unbind();
}

void MainGraphic::update() {
    if (DisplayWindow::Get().getKeyState(GLFW_KEY_SPACE) == KeyState::kDown)
        DisplayWindow::Get().swapCursor();
}

std::unique_ptr<MainGraphic> MainGraphic::instance_ = nullptr;

RenderBuffer &MainGraphic::getRenderBuffer() {
    return renderBuffer_;
}

RenderBuffer &MainGraphic::getRenderBufferRayMarch() {
    return renderBufferRayMarch_;
}

DebugGraphic &MainGraphic::getDebugGraphic() {
    return debug_;
}
