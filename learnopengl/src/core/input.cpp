#include <core/input.h>

InputInst Input::s_Instance;

bool Input::IsKeyPressed(Key key, ModifierBits mods) {
    std::map<Key, InputRegistry>::iterator it = s_Instance.m_KeysReg.find(key);
    if (it == s_Instance.m_KeysReg.end()) {
        return false;
    }

    InputRegistry keyReg = s_Instance.m_KeysReg[key];
    bool keyPressed = keyReg.Action != Action::Release;
    if (mods != ModifierBits::None) {
        keyPressed = keyPressed && ((keyReg.Modifiers & mods) != ModifierBits::None);
    }

    return keyPressed;
}

bool Input::IsMouseButtonPressed(MouseButton mb, ModifierBits mods) {
    std::map<MouseButton, InputRegistry>::iterator it = s_Instance.m_MouseButtonsReg.find(mb);
    if (it == s_Instance.m_MouseButtonsReg.end()) {
        return false;
    }

    InputRegistry mbReg = s_Instance.m_MouseButtonsReg[mb];
    bool mbPressed = mbReg.Action != Action::Release;
    if (mods != ModifierBits::None) {
        mbPressed = mbPressed && ((mbReg.Modifiers & mods) != ModifierBits::None);
    }

    return mbPressed;
}

void Input::AddKeyRegistry(Key key, Action action, ModifierBits mods) {
    s_Instance.m_KeysReg[key] = {action, mods};
}

void Input::AddMouseButtonRegistry(MouseButton mb, Action action, ModifierBits mods) {
    s_Instance.m_MouseButtonsReg[mb] = {action, mods};
}

void Input::AddMousePositionRegistry(float x, float y) {
    if (s_Instance.m_FirstMouse) {
        s_Instance.m_MouseLastPos = {x, y};
        s_Instance.m_FirstMouse = false;
    }

    s_Instance.m_MousePosDelta = {x - s_Instance.m_MouseLastPos.first, s_Instance.m_MouseLastPos.second - y};
    s_Instance.m_MouseLastPos = {x, y};
}

void Input::AddMouseScrollRegistry(float xOffset, float yOffset) {
    s_Instance.m_MouseScrollDelta = {xOffset, yOffset};
}
