#include <core/input.h>

InputSystem::InputSystem(const std::vector<Key> keysToMonitor) : m_Enabled(false) {
    for (Key k : keysToMonitor) {
        m_KeysReg[k] = {Action::Release, ModifierBits::None};
    }
}

const bool InputSystem::IsKeyPressed(Key key, ModifierBits mods) {
    if (!m_Enabled) {
        return false;
    }

    std::map<Key, KeyRegistry>::iterator it = m_KeysReg.find(key);
    if (it == m_KeysReg.end()) {
        return false;
    }

    KeyRegistry keyReg = m_KeysReg[key];
    bool keyPressed = keyReg.Action != Action::Release;
    if (mods != ModifierBits::None) {
        keyPressed = keyPressed && ((keyReg.Modifiers & mods) != ModifierBits::None);
    }

    return keyPressed;
}

void InputSystem::AddKeyRegistry(Key key, Action action, ModifierBits mods) {
    std::map<Key, KeyRegistry>::iterator it = m_KeysReg.find(key);
    if (it == m_KeysReg.end()) {
        return;
    }

    m_KeysReg[key] = {action, mods};
}
