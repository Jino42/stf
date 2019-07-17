#include "DisplayWindow.hpp"
#include "Engine/Camera.hpp"

DisplayWindow::DisplayWindow(std::string const &name, unsigned int width, unsigned int height) :
    cursor_(true),
    width_(width),
    height_(height) {
    glfwSetErrorCallback(DisplayWindow::callbackError_);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    if (!(window_ = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr))) {
        clean_();
		throw (DisplayWindow::ConstructorException("GlfwConstructorException: window was not created"));
    }
    glfwMakeContextCurrent(window_);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);

	glfwSetKeyCallback(window_, DisplayWindow::callbackKey_);
    glfwSetCursorPosCallback(window_, DisplayWindow::mouseCallback_);

    DisplayWindow::glfwByWindow_.insert(std::pair<GLFWwindow*, DisplayWindow&>(window_, *this));
}

DisplayWindow::~DisplayWindow() {
    DisplayWindow::glfwByWindow_.erase(DisplayWindow::glfwByWindow_.find(window_));
    clean_();
}

void    DisplayWindow::clean_() {
    if (window_) {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}

void			DisplayWindow::callbackKey_(GLFWwindow* window, int key, int, int action, int) {
    for (auto &glfw : DisplayWindow::glfwByWindow_) {
        if (glfw.first == window) {
            if (action == GLFW_REPEAT) {
				glfw.second.callbackKey(key, KeyState::kPress);
            }
            else if (action == GLFW_RELEASE) {
				glfw.second.callbackKey(key, KeyState::kUp);
            }
            else if (action == GLFW_PRESS) {
				glfw.second.callbackKey(key, KeyState::kDown);

            }
        }
    }
}
void DisplayWindow::callbackError_(int, const char* errorMessage) {
    throw (DisplayWindow::ConstructorException(errorMessage));
}

void            DisplayWindow::swapCursor() {
    if (cursor_)
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    cursor_ = !cursor_;
}

void            DisplayWindow::update() {

	KeyStateManager::update();

    if (!cursor_  && DisplayWindow::mouseCallbackCalled_) {
        Camera::Get().processMouseMovement(DisplayWindow::offsetX_, DisplayWindow::offsetY_);
        DisplayWindow::mouseCallbackCalled_ = false;
    }
	if (getKeyState(GLFW_KEY_ESCAPE) == KeyState::kDown)
        glfwSetWindowShouldClose(window_, true);
}

void            DisplayWindow::render() {
    glfwSwapBuffers(window_);
    glfwPollEvents();
}

bool            DisplayWindow::exit() const {
    return static_cast<bool>(glfwWindowShouldClose(window_));
}

std::map<GLFWwindow*, DisplayWindow&>     DisplayWindow::glfwByWindow_;

bool        DisplayWindow::firstMouse_ = true;
bool        DisplayWindow::mouseCallbackCalled_ = false;
float       DisplayWindow::lastX_ = DISPLAY_GLFW_WIN_WIDTH / 2.0f;
float       DisplayWindow::lastY_ = DISPLAY_GLFW_WIN_HEIGHT / 2.0f;
float       DisplayWindow::offsetX_ = 0.f;
float       DisplayWindow::offsetY_ = 0.f;

void DisplayWindow::mouseCallback_(GLFWwindow *, double xpos, double ypos) {
    if (DisplayWindow::firstMouse_) {
        DisplayWindow::lastX_ = static_cast<float>(xpos);
        DisplayWindow::lastY_ = static_cast<float>(ypos);
        DisplayWindow::firstMouse_ = false;
    }

    DisplayWindow::offsetX_ = static_cast<float>(xpos - DisplayWindow::lastX_);
    DisplayWindow::offsetY_ = static_cast<float>(DisplayWindow::lastY_ - ypos);

    DisplayWindow::lastX_ = static_cast<float>(xpos);
    DisplayWindow::lastY_ = static_cast<float>(ypos);
    mouseCallbackCalled_ = true;
}

GLFWwindow *DisplayWindow::getWindow() {
    return window_;
}

unsigned int DisplayWindow::getWidthWindow() const {
    return width_;
}

unsigned int DisplayWindow::getHeightWindow() const {
    return height_;
}

void		DisplayWindow::Init(char const *windowName, unsigned int width, unsigned int height) {
    instance_ = std::make_unique<DisplayWindow>(windowName, width, height);
}

DisplayWindow &DisplayWindow::Get() {
    return *DisplayWindow::instance_;
}

void DisplayWindow::clear() {
    glClearColor(0.29f * 0.35f, 0.0f, 0.51f * 0.35f, 0.40f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

std::unique_ptr<DisplayWindow> DisplayWindow::instance_ = nullptr;
