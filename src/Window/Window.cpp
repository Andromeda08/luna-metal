#include "Window.h"

#include <cassert>

Window::Window(const uint32_t width, const uint32_t height, const std::string& title): IWindow()
{
    assert(glfwInit());

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWmonitor* display = nullptr;
    mWindow = glfwCreateWindow(static_cast<int32_t>(width), static_cast<int32_t>(height),
                               title.c_str(), display, nullptr);

    assert(mWindow);

    glfwSetKeyCallback(mWindow, Window::defaultKeyHandler);
}

Window::~Window()
{
    if (mWindow)
    {
        glfwDestroyWindow(mWindow);
    }
    glfwTerminate();
}

bool Window::willClose() const
{
    return glfwWindowShouldClose(mWindow);
}

void Window::defaultKeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}
