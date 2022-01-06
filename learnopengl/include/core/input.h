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

    Up = GLFW_KEY_UP,
    Down = GLFW_KEY_DOWN,
    Left = GLFW_KEY_LEFT,
    Right = GLFW_KEY_RIGHT,

    W = GLFW_KEY_W,
    A = GLFW_KEY_A,
    S = GLFW_KEY_S,
    D = GLFW_KEY_D,

    Unknown = GLFW_KEY_UNKNOWN,
    Last = GLFW_KEY_LAST,
};

enum class ModifierBits : int {
    None = 0,
    Shift = GLFW_MOD_SHIFT,
    Ctrl = GLFW_MOD_CONTROL,
    Alt = GLFW_MOD_ALT,
    Super = GLFW_MOD_SUPER,
};

inline ModifierBits operator|(ModifierBits a, ModifierBits b) {
    return static_cast<ModifierBits>(static_cast<int>(a) | static_cast<int>(b));
}

inline ModifierBits operator&(ModifierBits a, ModifierBits b) {
    return static_cast<ModifierBits>(static_cast<int>(a) & static_cast<int>(b));
}

enum class MouseButton : int {
    Left = GLFW_MOUSE_BUTTON_LEFT,
    Right = GLFW_MOUSE_BUTTON_RIGHT,
    Middle = GLFW_MOUSE_BUTTON_MIDDLE
};

struct InputRegistry {
    Action Action;
    ModifierBits Modifiers;
};

class InputInst {
   public:
    std::map<Key, InputRegistry> m_KeysReg;
    std::map<MouseButton, InputRegistry> m_MouseButtonsReg;
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
        return AddMouseButtonRegistry(MouseButton(button), Action(action), ModifierBits(mods));
    }

    inline static void MousePositionCallback(GLFWwindow* window, double xpos, double ypos) {
        return AddMousePositionRegistry((float)xpos, (float)ypos);
    }

    inline static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        return AddKeyRegistry(Key(key), Action(action), ModifierBits(mods));
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
    }

    static bool IsKeyPressed(Key key, ModifierBits mods = ModifierBits::None);
    static bool IsMouseButtonPressed(MouseButton mb, ModifierBits mods = ModifierBits::None);
    static void AddKeyRegistry(Key key, Action action, ModifierBits mods);
    static void AddMouseButtonRegistry(MouseButton mb, Action action, ModifierBits mods);
    static void AddMousePositionRegistry(float x, float y);
    static void AddMouseScrollRegistry(float xOffset, float yOffset);
};
