cmake_minimum_required(VERSION 3.0)
set(NAME ntl_ps)
project(${NAME})


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DROOT_PATH=\\\"${CMAKE_SOURCE_DIR}\\\"")
set(CMAKE_BUILD_PARALLEL_LEVEL true)
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    link_directories(/usr/lib/)
    link_directories($ENV{HOME}/.brew/lib/)
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-Wall -Werror -Wextra -Wvla -Wpedantic -ansi -pedantic -O3)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
endif()

find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init extern/stb extern/glad extern/imgui
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if (NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif ()
endif ()

if (WIN32)
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
    find_package(OpenGL 3.3 REQUIRED)
    #OpenCL_INCLUDE_DIR = "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v8.0/include"
elseif (UNIX AND NOT APPLE)
elseif (APPLE)
    link_directories(/usr/lib/)
    link_directories($ENV{HOME}/.brew/lib/)
    include_directories($ENV{HOME}/.brew/include)

    include_directories(/System/Library/Frameworks)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    find_library(IOKit_LIBRARY IOKit)
    find_library(CoreVideo_LIBRARY CoreVideo)
    mark_as_advanced(COCOA_LIBRARY OpenGL_LIBRARY)
    set(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
    set(OPENGL_LIBRARIES ${LIBS} ${APPLE_LIBS})
else ()
    find_package(OpenGL 3.3 REQUIRED)
endif ()

if(NOT WIN32)
    set(GLAD_LIBRARIES dl)
endif()

set(BOOST_ROOT $ENV{HOME}/.brew/)
find_package(Boost QUIET COMPONENTS program_options filesystem)
if (Boost_FOUND)
    message("EXTERN : Boost finded")
elseif (NOT TARGET Boost::boost)
    message("EXTERN : Unable to find Boost, cloning...")
    if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init extern/boost-cmake
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if (NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif ()
    endif ()
    add_subdirectory(extern/boost-cmake)
endif()


#list(APPEND CMAKE_MODULE_PATH "~/.brew/lib/cmake/glm")
#find_package(glm QUIET)
#if (glm_FOUND)
#    message("EXTERN : glm finded")
#elseif (NOT TARGET glm)
#    message("EXTERN : Unable to find glm, cloning...")
    if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init extern/glm
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if (NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif ()
    endif ()
    option(GLM_BUILD_DOCS OFF)
    option(GLM_BUILD_EXAMPLES OFF)
    option(GLM_TEST_ENABLE OFF)
    add_subdirectory(extern/glm)
#endif()

list(APPEND CMAKE_MODULE_PATH "~/.brew/lib/cmake/glfw3")
find_package(glfw QUIET)
if (glfw_FOUND)
    message("EXTERN : glfw finded")
elseif (NOT TARGET glfw)
    message("EXTERN : Unable to find glfw, cloning...")
    if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init extern/glfw
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if (NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif ()
    endif ()
    option(GLFW_BUILD_DOCS OFF)
    option(GLFW_BUILD_EXAMPLES OFF)
    option(GLFW_BUILD_TESTS OFF)
    add_subdirectory(extern/glfw)

endif()

list(APPEND CMAKE_MODULE_PATH "~/.brew/lib/cmake/assimp")
find_package(assimp QUIET)
if (assimp_FOUND)
    message("EXTERN : assimp finded")
elseif (NOT TARGET assimp)
    message("EXTERN : Unable to find assimp, cloning...")
    if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init extern/assimp
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if (NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif ()
    endif ()
    option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
    option(ASSIMP_BUILD_SAMPLES OFF)
    option(ASSIMP_BUILD_TESTS OFF)
    add_subdirectory(extern/assimp)
endif()

find_package(OpenCL REQUIRED)
set(OPENCL_INCLUDE_DIR "${OpenCL_INCLUDE_DIR}/Headers/")
set(OPENCL_LIB_DIR "${OpenCL_LIBRARY}/Libraries/")
message("OPENCL  : ${OpenCL_LIBRARY}")
option(BUILD_DOCS OFF)
option(BUILD_EXAMPLES OFF)
option(BUILD_TESTS OFF)
#add_subdirectory(extern/OpenCL-CLHPP)

message("EXTERN : libnoise cloning...")
if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init extern/json
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if (NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif ()
endif ()
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(extern/json)


message("EXTERN : libnoise cloning...")
if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init extern/libnoise
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if (NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif ()
endif ()
option(BUILD_LIBNOISE_DOCUMENTATION OFF)
option(BUILD_LIBNOISE_UTILS ON)
option(BUILD_LIBNOISE_EXAMPLES ON)
add_subdirectory(extern/libnoise)

add_subdirectory(extern/lua)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_USER_CONFIG=\"<../imconfig-sfml.h>\"")
#add_subdirectory(extern/imgui)
add_subdirectory(extern/)

message(STATUS ${OpenCL_INCLUDE_DIR})
include_directories(${OpenCL_INCLUDE_DIR}/Headers/)
include_directories(${OpenCL_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(SYSTEM
        ${CMAKE_SOURCE_DIR}/extern/glad/include/
        ${CMAKE_SOURCE_DIR}/extern/glfw/include/
        ${CMAKE_SOURCE_DIR}/extern/glm/
        ${CMAKE_SOURCE_DIR}/extern/stb/
        ${LIBNOISE_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}/extern/OpenCL-CLHPP/include/
        ${CMAKE_SOURCE_DIR}/extern/imgui/
        ${CMAKE_SOURCE_DIR}/extern/imgui/examples/
        ${CMAKE_SOURCE_DIR}/extern/lua/src/
        ${CMAKE_SOURCE_DIR}/extern/json/include/
)


add_executable(${NAME}
        include/noiseutils.cpp

        extern/glad/src/glad.c
        src/Engine/ModelEngine/ActorModel.cpp
        src/Engine/Camera.hpp
        src/Engine/Graphic/Light.cpp
        src/Engine/Display/DisplayWindow.hpp
        src/Engine/Graphic/Skybox.hpp
        src/Engine/Graphic/Material.hpp
        src/Engine/Graphic/Material.cpp
        src/Engine/Display/KeyState.hpp
        src/Engine/Display/KeyState.cpp
        src/Engine/Display/DisplayWindow.cpp
        src/Engine/ModelEngine/Transform.hpp
        src/Engine/Shader.hpp
        src/Engine/ModelEngine/Model.cpp
        src/Engine/Display/KeyStateManager.hpp
        src/Engine/ModelEngine/ActorModel.hpp
        src/Engine/Display/KeyStateManager.cpp
        src/Engine/ModelEngine/Model.hpp
        src/Engine/ModelEngine/Transform.cpp
        src/Engine/ModelEngine/Mesh.hpp
        src/Engine/ModelEngine/Mesh.cpp
        src/main.cpp
        src/Engine/Graphic/Light.hpp
        src/Engine/MainGraphic.cpp
        src/Engine/Graphic/Skybox.cpp
        src/Engine/MainGraphic.hpp
        src/Engine/Camera.cpp
        src/Engine/Shader.cpp

        src/Particle/PaticleEmitter/ParticleEmitterMesh.cpp src/Particle/PaticleEmitter/ParticleEmitterMesh.hpp
        src/Particle/ParticleSystem.cpp src/Particle/ParticleSystem.hpp
        src/Particle/ParticleModule/AParticleModule.hpp src/Particle/ParticleModule/ModuleRequired.cpp src/Particle/ParticleModule/ModuleRequired.hpp
        src/Particle/ParticleModule/ParticleSpawnModule.cpp src/Particle/ParticleModule/ParticleSpawnModule.hpp src/Cl/ClContext.cpp
        src/Cl/ClContext.hpp src/Cl/ClQueue.cpp src/Cl/ClQueue.hpp src/OCGL_Buffer.cpp src/OCGL_Buffer.hpp src/Cl/ClProgram.cpp src/Cl/ClProgram.hpp src/Cl/ClError.cpp src/Cl/ClError.hpp src/Particle/ParticleData.hpp src/Particle/ParticleModule/ModuleAttractor.cpp src/Particle/ParticleModule/ModuleAttractor.hpp src/Particle/PaticleEmitter/ParticleEmitterPoint.cpp src/Particle/PaticleEmitter/ParticleEmitterPoint.hpp src/Particle/PaticleEmitter/AParticleEmitter.hpp src/Particle/PaticleEmitter/AParticleEmitter.cpp src/Particle/ParticleModule/ModuleColor.cpp src/Particle/ParticleModule/ModuleColor.hpp src/Particle/PaticleEmitter/ParticleEmitterSprite.cpp src/Particle/PaticleEmitter/ParticleEmitterSprite.hpp src/Particle/PaticleEmitter/ParticleSpriteAtlas.cpp src/Particle/PaticleEmitter/ParticleSpriteAtlas.hpp src/Engine/Graphic/Texture.cpp src/Engine/Graphic/Texture.hpp src/Engine/Time.cpp src/Engine/Time.hpp src/Particle/ParticleModule/ModuleMovement.cpp src/Particle/ParticleModule/ModuleMovement.hpp include/Range.hpp include/OpenCGL_Tools.hpp include/OpenCGL_Tools.cpp src/Engine/Random.hpp src/Engine/Rancom.cpp src/Gui/Gui.cpp src/Gui/Gui.hpp src/Gui/AWidget.cpp src/Gui/AWidget.hpp src/Gui/WidgetOption.cpp src/Gui/WidgetOption.hpp src/Engine/RenderBuffer.cpp src/Engine/RenderBuffer.hpp src/Gui/WidgetRender.cpp src/Gui/WidgetRender.hpp src/Gui/WidgetEditor.cpp src/Gui/WidgetEditor.hpp src/Particle/ParticleModule/AParticleModule.cpp src/Particle/ParticleModule/ModuleSizeOverLifetime.cpp src/Particle/ParticleModule/ModuleSizeOverLifetime.hpp include/NTL.hpp include/NTL_Debug.cpp include/cl_type.cpp src/Voxel/Chunk.cpp src/Voxel/Chunk.hpp src/Voxel/VoxelWorld.cpp src/Voxel/VoxelWorld.hpp src/Engine/ShaderManager.cpp src/Engine/ShaderManager.hpp src/Engine/TestParticle.cpp src/Engine/TestParticle.hpp src/Engine/ModelEngine/MainGraphicExtendModel.cpp src/Engine/ModelEngine/MainGraphicExtendModel.hpp src/Engine/Frustum.cpp src/Engine/Frustum.hpp src/Engine/CameraManager.cpp src/Engine/CameraManager.hpp include/ALifeTimeObject.hpp include/ADebugObject.hpp src/Engine/DebugGraphic.cpp src/Engine/DebugGraphic.hpp include/ADebugObject.cpp include/IGuiEntity.hpp include/ANommable.hpp src/Gui/EditorCamera.cpp src/Gui/EditorCamera.hpp src/Gui/AEditor.cpp src/Gui/AEditor.hpp src/Gui/GuiSettings.hpp src/Engine/TimerOnConstructOffDestruct.cpp src/Engine/TimerOnConstructOffDestruct.hpp src/Cl/ClKernel.cpp src/Cl/ClKernel.hpp src/Engine/Debug.cpp src/Engine/Debug.hpp src/Particle/ParticleModule/ModuleMeshParticulizer.cpp src/Particle/ParticleModule/ModuleMeshParticulizer.hpp src/Particle/ParticleModule/ModuleTarget.cpp src/Particle/ParticleModule/ModuleTarget.hpp src/Particle/ParticleModule/ModuleMoveToTarget.cpp src/Particle/ParticleModule/ModuleMoveToTarget.hpp src/PathManager.cpp src/PathManager.hpp include/json.hpp include/json.cpp src/Engine/ModelEngine/ModelManager.cpp src/Engine/ModelEngine/ModelManager.hpp src/json/JsonParticleParser.cpp src/json/JsonParticleParser.hpp src/Particle/ParticleSystemManager.cpp src/Particle/ParticleSystemManager.hpp src/test.cpp src/Engine/RadixCl.cpp src/Engine/RadixCl.hpp src/Particle/ParticleModule/ModuleSPH.cpp src/Particle/ParticleModule/ModuleSPH.hpp src/Cl/ClTools.cpp src/Cl/ClTools.hpp src/Gui/EditorParticleSystems.cpp src/Gui/EditorParticleSystems.hpp
        src/Engine/AABB.cpp src/Engine/AABB.hpp
        src/Engine/Sphere.cpp src/Engine/Sphere.hpp
        src/Particle/PaticleEmitter/ParticleEmitterSPH.hpp
        src/Particle/PaticleEmitter/ParticleEmitterSPH.cpp
        src/Gui/WidgetRayMarch.cpp src/Gui/WidgetRayMarch.hpp
        )

add_definitions(-DGLFW_INCLUDE_NONE -DCL_SILENCE_DEPRECATION
        -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")


target_link_libraries(${NAME} PRIVATE lua_static)
target_link_libraries(${NAME} PRIVATE Boost::filesystem)
target_link_libraries(${NAME} PRIVATE Boost::program_options)
target_link_libraries(${NAME} PRIVATE assimp)
target_link_libraries(${NAME} PRIVATE glfw)
target_link_libraries(${NAME} PRIVATE noise)
target_link_libraries(${NAME} PRIVATE ${GLFW_LIBRARIES})
target_link_libraries(${NAME} PRIVATE ${GLAD_LIBRARIES})
target_link_libraries(${NAME} PRIVATE ${OpenCL_LIBRARY})
target_link_libraries(${NAME} PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(${NAME} PRIVATE imgui)
target_link_libraries(${NAME} PRIVATE nlohmann_json::nlohmann_json)


set_target_properties(${NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

#target_compile_options