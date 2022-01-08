#pragma once

#include <common.h>

#include <map>

enum class Action : int {
    Release = GLFW_RELEASE,
    Press = GLFW_PRESS,
    Repeat = GLFW_REPEAT,
};

enum class Key : int {
    Esc = GLFW_KEY_ESCAPE,
    Enter = GLFW_KEY_ENTER,
    Space = GLFW_KEY_SPACE,

    Up = GLFW_KEY_UP,
    Down = GLFW_KEY_DOWN,
    Left = GLFW_KEY_LEFT,
    Right = GLFW_KEY_RIGHT,

    W = GLFW_KEY_W,
    A = GLFW_KEY_A,
    S = GLFW_KEY_S,
    D = GLFW_KEY_D,

    LCtrl = GLFW_KEY_LEFT_CONTROL,
    RCtrl = GLFW_KEY_RIGHT_CONTROL,
    LAlt = GLFW_KEY_LEFT_ALT,
    RAlt = GLFW_KEY_RIGHT_ALT,
    LShift = GLFW_KEY_LEFT_SHIFT,
    RShift = GLFW_KEY_RIGHT_SHIFT,

    Unknown = GLFW_KEY_UNKNOWN,
    Last = GLFW_KEY_LAST + 1,
};

enum class MouseButton : int {
    Left = GLFW_MOUSE_BUTTON_LEFT,
    Right = GLFW_MOUSE_BUTTON_RIGHT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE,

    Unknown = -1,
    Last = GLFW_MOUSE_BUTTON_LAST + 1,
};

enum class ModifierBit : int {
    None = 0,
    Shift = GLFW_MOD_SHIFT,
    Ctrl = GLFW_MOD_CONTROL,
    Alt = GLFW_MOD_ALT,
};

inline ModifierBit operator|(ModifierBit a, ModifierBit b) {
    return static_cast<ModifierBit>(static_cast<int>(a) | static_cast<int>(b));
}

inline ModifierBit operator&(ModifierBit a, ModifierBit b) {
    return static_cast<ModifierBit>(static_cast<int>(a) & static_cast<int>(b));
}

const ModifierBit ALL_SUPPORTED_MODIFIER_BITS = ModifierBit::Shift | ModifierBit::Ctrl | ModifierBit::Alt;

struct InputRegistry {
    bool Current;
    bool Previous;
};

class InputInst {
   public:
    bool m_KeyPressed[(int)Key::Last] = {};
    bool m_KeyJustPressed[(int)Key::Last] = {};
    bool m_MouseButtonPressed[(int)MouseButton::Last] = {};
    bool m_MouseButtonJustPressed[(int)MouseButton::Last] = {};
    std::pair<float, float> m_MouseLastPos = {0.0f, 0.0f};
    std::pair<float, float> m_MousePosDelta = {0.0f, 0.0f};
    std::pair<float, float> m_MouseScrollDelta = {0.0f, 0.0f};
    bool m_FirstMouse = true;
};

class Input {
   private:
    static InputInst s_Instance;

   public:
    inline static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        return AddMouseButtonRegistry(MouseButton(button), Action(action), ModifierBit(mods));
    }

    inline static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
        return AddMousePositionRegistry((float)xpos, (float)ypos);
    }

    inline static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        return AddKeyRegistry(Key(key), Action(action), ModifierBit(mods));
    }

    inline static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        return AddMouseScrollRegistry((float)xoffset, (float)yoffset);
    }

    inline static std::pair<float, float> GetMousePosition() {
        return s_Instance.m_MouseLastPos;
    }

    inline static std::pair<float, float> GetMousePositionDelta() {
        return s_Instance.m_MousePosDelta;
    }

    inline static std::pair<float, float> GetMouseScrollDelta() {
        return s_Instance.m_MouseScrollDelta;
    }

    inline static void FrameReset() {
        s_Instance.m_MousePosDelta = {0.0f, 0.0f};
        s_Instance.m_MouseScrollDelta = {0.0f, 0.0f};
        std::fill(s_Instance.m_KeyJustPressed, s_Instance.m_KeyJustPressed + (int)Key::Last, false);
        std::fill(s_Instance.m_MouseButtonJustPressed, s_Instance.m_MouseButtonJustPressed + (int)MouseButton::Last,
                  false);
    }

    static bool IsKeyPressed(Key key, ModifierBit mods = ModifierBit::None);
    static bool IsKeyJustPressed(Key key, ModifierBit mods = ModifierBit::None);
    static bool IsMouseButtonPressed(MouseButton mb, ModifierBit mods = ModifierBit::None);
    static bool IsMouseButtonJustPressed(MouseButton mb, ModifierBit mods = ModifierBit::None);
    static void AddKeyRegistry(Key key, Action action, ModifierBit mods);
    static void AddMouseButtonRegistry(MouseButton mb, Action action, ModifierBit mods);
    static void AddMousePositionRegistry(float x, float y);
    static void AddMouseScrollRegistry(float xOffset, float yOffset);

   private:
    static bool AreModifiersPressed(ModifierBit mods);
};
