#include "DragonEngine/Input.h"

#include "d_internal.h"

#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace dgn
{
    Input* Input::s_current_input = nullptr;

    Input::Input()
    {
        m_keys.fill(false);
        m_keys_l.fill(false);
        mouse_buttons.fill(false);
        mouse_buttons_l.fill(false);

        mouse_x = 0;
        mouse_y = 0;

        mouse_x_d = 0.0f;
        mouse_y_d = 0.0f;

        scroll_x = 0.0f;
        scroll_y = 0.0f;

        gp_states = new GLFWgamepadstate[NumberController];
    }

    Input::~Input()
    {
        delete[](gp_states);
    }

    void Input::pollEvents()
    {
        m_keys_l = m_keys;
        mouse_buttons_l = mouse_buttons;

        for(unsigned i = 0; i < NumberController; i++)
        {
            glfwGetGamepadState(GLFW_JOYSTICK_1 + i, &gp_states[i]);
        }

        mouse_x_d = 0.0f;
        mouse_y_d = 0.0f;

        scroll_x = 0.0f;
        scroll_y = 0.0f;

        glfwPollEvents();
    }

    void Input::makeCurrent()
    {
        s_current_input = this;
    }

    bool Input::getKey(Key key)
    {
        return m_keys[int(key)];
    }

    bool Input::getKeyDown(Key key)
    {
        return m_keys[int(key)] && !m_keys_l[int(key)];
    }

    bool Input::getKeyUp(Key key)
    {
        return !m_keys[int(key)] && m_keys_l[int(key)];
    }

    bool Input::getMouseButton(MouseButton button)
    {
        return mouse_buttons[int(button)];
    }

    bool Input::getMouseButtonDown(MouseButton button)
    {
        return mouse_buttons[int(button)] && !mouse_buttons_l[int(button)];
    }

    bool Input::getMouseButtonUp(MouseButton button)
    {
        return !mouse_buttons[int(button)] && mouse_buttons_l[int(button)];
    }

    int Input::getMouseX()
    {
        return mouse_x;
    }

    int Input::getMouseY()
    {
        return mouse_y;
    }

    float Input::getMouseXDelta()
    {
        return mouse_x_d;
    }

    float Input::getMouseYDelta()
    {
        return mouse_y_d;
    }

    float Input::getGamepadAxis(unsigned char gamepad, GamepadAxis axis, float deadzone)
    {
        if(gamepad >= NumberController) return 0.0f;

        float v = gp_states[gamepad].axes[unsigned(axis)];

        return fabs(v) < deadzone ? 0.0f : v;
    }
    bool Input::getGamepadButton(unsigned char gamepad, GamepadButtons button)
    {
        if(gamepad >= NumberController) return false;

        return gp_states[gamepad].buttons[unsigned(button)];
    }

    void Input::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if(Input::s_current_input == nullptr) return;

        if(key != KeyUnknown)
        {
            if(action == GLFW_PRESS)
            {
                Input::s_current_input->m_keys[key] = true;
            }
            else if(action == GLFW_RELEASE)
            {
                Input::s_current_input->m_keys[key] = false;
            }
        }
    }

    void Input::cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
    {
        if(Input::s_current_input == nullptr) return;

        Input::s_current_input->mouse_x_d = xpos - Input::s_current_input->mouse_x;
        Input::s_current_input->mouse_y_d = ypos - Input::s_current_input->mouse_y;

        Input::s_current_input->mouse_x = int(xpos);
        Input::s_current_input->mouse_y = int(ypos);
    }

    void Input::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        if(Input::s_current_input == nullptr) return;

        if(action == GLFW_PRESS)
        {
            Input::s_current_input->mouse_buttons[button] = true;
        }
        else if(action == GLFW_RELEASE)
        {
            Input::s_current_input->mouse_buttons_l[button] = false;
        }
    }

    void Input::scrollCallback(GLFWwindow *window, double xscroll, double yscroll)
    {
        if(Input::s_current_input == nullptr) return;

        Input::s_current_input->scroll_x = xscroll;
        Input::s_current_input->scroll_y = yscroll;
    }
}
