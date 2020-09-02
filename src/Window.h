#pragma once

#include <string>
#include <Input.h>
#include <Renderer.h>

struct GLFWwindow;

namespace dgn
{
    enum class CursorMode
    {
        Normal,
        Hidden,
        Disabled
    };

    enum class VsyncMode
    {
        None = 0,
        Single = 1,
        Double = 2
    };

    class Window
    {
    private:
        GLFWwindow* native_window;

        unsigned m_width;
        unsigned m_height;
        std::string m_title;

        unsigned long frame_count;

        Input m_input;
        Renderer m_renderer;

    public:
        Window();
        virtual ~Window();

        bool initialize(unsigned width, unsigned height, std::string title);
        void terminate();

        void makeCurrent();
        bool shouldClose();
        void swapBuffers();

        unsigned getWidth();
        unsigned getHeight();
        std::string getTitle();
        unsigned long getFrameCount();
        double getTime();

        Input& getInput();
        Renderer& getRenderer();

        void setRawCursorMode(bool enabled);
        void setCursorMode(CursorMode cursor_mode);
        void setVsync(VsyncMode sync);
        void setWidth(unsigned new_width);
        void setHeight(unsigned new_height);
        void setSize(unsigned new_width, unsigned new_height);
        void setTitle(std::string new_title);
    };
}
