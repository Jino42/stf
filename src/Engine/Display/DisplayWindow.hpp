#pragma once

#include <memory>
#include <string>
#include <map>
#include <list>
#include <glad/glad.h>
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <cstdint>
#include "KeyStateManager.hpp"

#define DISPLAY_GLFW_WIN_WIDTH 1024
#define DISPLAY_GLFW_WIN_HEIGHT 720

class DisplayWindow : public KeyStateManager {
public:
    class ConstructorException : public std::runtime_error {
    public:
        ConstructorException() noexcept = delete;
        explicit ConstructorException(std::string const &s) noexcept : runtime_error(s) {};
        ~ConstructorException() noexcept override = default;
        ConstructorException(ConstructorException const &src) noexcept = delete;
        ConstructorException &operator=(ConstructorException const &rhs) noexcept = delete;
    };

	DisplayWindow(std::string const &name, unsigned int width, unsigned int height);
	virtual ~DisplayWindow();
	DisplayWindow() = delete;
	DisplayWindow(DisplayWindow const &shader) = delete;
	DisplayWindow &operator=(DisplayWindow const &shader) = delete;

	void update() override;
	void render();
    void clear();
	bool exit() const;

    GLFWwindow *getWindow();
    void swapCursor();


    unsigned int getWidthWindow() const;
	void		 setWidthWindow(unsigned int width);
    unsigned int getHeightWindow() const;
	void		 setHeightWindow(unsigned int height);

    static void Init(const char *windowName, unsigned int width, unsigned int height);
    static DisplayWindow &Get();

//protected:
	bool cursor_;
    unsigned int width_;
    unsigned int height_;

    static float offsetX_;
	static float offsetY_;
	static float lastX_;
	static float lastY_;
	static bool mouseCallbackCalled_;

private:
	GLFWwindow *window_;

	void clean_();

	static std::map<GLFWwindow *, DisplayWindow &> glfwByWindow_;
	static bool firstMouse_;

	static void mouseCallback_(GLFWwindow *, double xpos, double ypos);
	static void callbackKey_(GLFWwindow *window, int key, int, int action, int);
	static void windowSizeCallback_(GLFWwindow* window, int width, int height);
	static void FramebufferSizeCallback_(GLFWwindow* window, int width, int height);
	static void WindowContentScaleCallback_(GLFWwindow* window, float xscale, float yscale);
	static void callbackError_(int, const char *errorMessage);

    static std::unique_ptr<DisplayWindow> instance_;

};