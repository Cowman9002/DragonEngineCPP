#pragma once

#include <array>

struct GLFWgamepadstate;
struct GLFWwindow;

namespace dgn
{
    const int KeyUnknown = -1;

    enum class Key
    {
        Space             = 32,
        Apostrophe        = 39,  /* ' */
        Comma             = 44,  /* , */
        Minus             = 45,  /* - */
        Period            = 46,  /* . */
        Slash             = 47,  /* / */
        Zero              = 48,
        One               = 49,
        Two               = 50,
        Three             = 51,
        Four              = 52,
        Five              = 53,
        Six               = 54,
        Seven             = 55,
        Eight             = 56,
        Nine              = 57,
        Semicolon         = 59, /* ; */
        Equal             = 61, /* = */
        A                 = 65,
        B                 = 66,
        C                 = 67,
        D                 = 68,
        E                 = 69,
        F                 = 70,
        G                 = 71,
        H                 = 72,
        I                 = 73,
        J                 = 74,
        K                 = 75,
        L                 = 76,
        M                 = 77,
        N                 = 78,
        O                 = 79,
        P                 = 80,
        Q                 = 81,
        R                 = 82,
        S                 = 83,
        T                 = 84,
        U                 = 85,
        V                 = 86,
        W                 = 87,
        X                 = 88,
        Y                 = 89,
        Z                 = 90,
        LeftBracket       = 91,  /* [ */
        Backslash         = 92,  /* \ */
        RightBracket      = 93,  /* ] */
        GraveAcccent      = 96,  /* ` */
        World1            = 161, /* non-US #1 */
        World2            = 162, /* non-US #2 */

        Escape            = 256,
        Enter             = 257,
        Tab               = 258,
        Backspace         = 259,
        Insert            = 260,
        Delete            = 261,
        Right             = 262,
        Left              = 263,
        Down              = 264,
        Up                = 265,
        PageUp            = 266,
        PageDown          = 267,
        Home              = 268,
        End               = 269,
        CapsLock          = 280,
        ScrollLock        = 281,
        NumLock           = 282,
        PrintScreen       = 283,
        Pause             = 284,
        F1                = 290,
        F2                = 291,
        F3                = 292,
        F4                = 293,
        F5                = 294,
        F6                = 295,
        F7                = 296,
        F8                = 297,
        F9                = 298,
        F10               = 299,
        F11               = 300,
        F12               = 301,
        F13               = 302,
        F14               = 303,
        F15               = 304,
        F16               = 305,
        F17               = 306,
        F18               = 307,
        F19               = 308,
        F20               = 309,
        F21               = 310,
        F22               = 311,
        F23               = 312,
        F24               = 313,
        F25               = 314,
        PadZero           = 320,
        PadOne            = 321,
        PadTwo            = 322,
        PadThree          = 323,
        PadFour           = 324,
        PadFive           = 325,
        PadSix            = 326,
        PadSeven          = 327,
        PadEight          = 328,
        PadNine           = 329,
        PadDecimal        = 330,
        PadDivide         = 331,
        PadMultiply       = 332,
        PadSubtract       = 333,
        PadAdd            = 334,
        PadEnter          = 335,
        PadEqual          = 336,
        LeftShift         = 340,
        LeftControl       = 341,
        LeftAlt           = 342,
        LeftSuper         = 343,
        RightShift        = 344,
        RightControl      = 345,
        RightAlt          = 346,
        RightSuper        = 347,
        Menu              = 348,
        Size
    };

    enum class MouseButton
    {
        Left = 0,
        Right,
        Middle,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Size
    };

    enum class GamepadButtons
    {
        South,
        East,
        West,
        North,
        LeftBumper,
        RightBumper,
        Back,
        Start,
        Guide,
        LeftThumb,
        RightThumb,
        DpadUp,
        DpadRight,
        DpadDown,
        DpadLeft,
        Size
    };

    enum class GamepadAxis
    {
        LeftX,
        LeftY,
        RightX,
        RightY,
        LeftTrigger,
        RightTrigger,
        Size
    };

    const unsigned NumberController = 16;

    class Input
    {
    public:
        Input();
        virtual ~Input();

        void pollEvents();

        void makeCurrent();

        bool getKey(Key key);
        bool getKeyDown(Key key);
        bool getKeyUp(Key key);

        bool getMouseButton(MouseButton button);
        bool getMouseButtonDown(MouseButton button);
        bool getMouseButtonUp(MouseButton button);

        int getMouseX();
        int getMouseY();
        float getMouseXDelta();
        float getMouseYDelta();

        float getGamepadAxis(unsigned char gamepad, GamepadAxis, float deadzone);
        bool getGamepadButton(unsigned char gamepad, GamepadButtons button);

        static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
        static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        static void scrollCallback(GLFWwindow *window, double xscroll, double yscroll);

    private:

        std::array<bool, unsigned(Key::Size)> m_keys;
        std::array<bool, unsigned(Key::Size)> m_keys_l;

        std::array<bool, unsigned(MouseButton::Size)> mouse_buttons;
        std::array<bool, unsigned(MouseButton::Size)> mouse_buttons_l;

        int mouse_x;
        int mouse_y;

        float mouse_x_d;
        float mouse_y_d;

        float scroll_x;
        float scroll_y;

        GLFWgamepadstate *gp_states;

        static Input* s_current_input;
    };
}
