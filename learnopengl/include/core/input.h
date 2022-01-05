#pragma once

#include <common.h>

#include <map>

enum class Action {
    Release = GLFW_RELEASE,
    Press = GLFW_PRESS,
    Repeat = GLFW_REPEAT,
};

enum class Key {
    Esc = GLFW_KEY_ESCAPE,
    Up = GLFW_KEY_UP,
    Down = GLFW_KEY_DOWN,
    Left = GLFW_KEY_LEFT,
    Right = GLFW_KEY_RIGHT,

    Unknown = GLFW_KEY_UNKNOWN,
    Last = GLFW_KEY_LAST,
};

enum class ModifierBits {
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

struct KeyRegistry {
    Action Action;
    ModifierBits Modifiers;
};

class InputSystem {
   private:
    bool m_Enabled;
    std::map<Key, KeyRegistry> m_KeysReg;

   public:
    InputSystem(const std::vector<Key> keysToMonitor);

    const bool IsKeyPressed(Key key, ModifierBits mods = ModifierBits::None);
    void AddKeyRegistry(Key key, Action action, ModifierBits mods);

    inline void SetEnabled(bool on) {
        m_Enabled = on;
    }

    inline bool GetEnabled() {
        return m_Enabled;
    }
};
