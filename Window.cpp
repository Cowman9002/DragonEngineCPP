#include "DragonEngine/Window.h"

#include "d_internal.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace dgn
{
    Window::Window() : native_window(nullptr), m_width(0), m_height(0), m_title(""), frame_count(0), m_input() {}

    Window::~Window()
    {
        terminate();
    }

    bool Window::initialize(unsigned width, unsigned height, std::string title)
    {
        if(glfwInit() == GLFW_FALSE)
        {
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        native_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if(native_window == nullptr)
        {
            return false;
        }

        m_width = width;
        m_height = height;
        m_title = title;

        glfwSetKeyCallback(native_window, Input::keyCallback);
        glfwSetMouseButtonCallback(native_window, Input::mouseButtonCallback);
        glfwSetCursorPosCallback(native_window, Input::cursorPosCallback);
        glfwSetScrollCallback(native_window, Input::scrollCallback);

        glfwMakeContextCurrent(native_window);

        if(!m_renderer.initialize())
        {
            return false;
        }

        return true;
    }

    void Window::terminate()
    {
        m_renderer.terminate();

        glfwDestroyWindow(native_window);

        native_window = nullptr;
        m_width = 0;
        m_height = 0;
    }

    void Window::makeCurrent()
    {
        glfwMakeContextCurrent(native_window);
        m_input.makeCurrent();
    }

    bool Window::shouldClose()
    {
        return glfwWindowShouldClose(native_window);
    }

    void Window::swapBuffers()
    {
        glfwSwapBuffers(native_window);

        frame_count++;
    }

    /////////////////////////
    //      GETTERS        //
    /////////////////////////

    unsigned Window::getWidth()
    {
        return m_width;
    }

    unsigned Window::getHeight()
    {
        return m_height;
    }

    std::string Window::getTitle()
    {
        return m_title;
    }

    unsigned long Window::getFrameCount()
    {
        return frame_count;
    }

    double Window::getTime()
    {
        return glfwGetTime();
    }

    Input& Window::getInput()
    {
        return m_input;
    }

    Renderer& Window::getRenderer()
    {
        return m_renderer;
    }

    /////////////////////////
    //      SETTERS        //
    /////////////////////////

    void Window::setRawCursorMode(bool enabled)
    {
        if(glfwRawMouseMotionSupported())
        {
            glfwSetInputMode(native_window, GLFW_RAW_MOUSE_MOTION, enabled);
        }
    }

    void Window::setCursorMode(CursorMode cursor_mode)
    {
        int glfw_mode = 0;

        switch(cursor_mode)
        {
            case CursorMode::Disabled:
            {
                glfw_mode = GLFW_CURSOR_DISABLED;
                break;
            }
            case CursorMode::Hidden:
            {
                glfw_mode = GLFW_CURSOR_HIDDEN;
                break;
            }
            case CursorMode::Normal:
            {
                glfw_mode = GLFW_CURSOR_NORMAL;
                break;
            }
            default:
            {
                logError("UNKNOWN ENUM", "" + int(cursor_mode));
            }
        }

        glfwSetInputMode(native_window, GLFW_CURSOR, glfw_mode);
    }

    void Window::setVsync(VsyncMode sync)
    {
        glfwSwapInterval(int(sync));
    }

    void Window::setWidth(unsigned new_width)
    {
        m_width = new_width;
        glfwSetWindowSize(native_window, m_width, m_height);
    }

    void Window::setHeight(unsigned new_height)
    {
        m_height = new_height;
        glfwSetWindowSize(native_window, m_width, m_height);
    }

    void Window::setSize(unsigned new_width, unsigned new_height)
    {
        m_width = new_width;
        m_height = new_height;
        glfwSetWindowSize(native_window, m_width, m_height);
    }

    void Window::setTitle(std::string new_title)
    {
        m_title = new_title;
        glfwSetWindowTitle(native_window, m_title.c_str());
    }
}
