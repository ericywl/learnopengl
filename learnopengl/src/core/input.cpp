#include <core/input.h>

InputInst Input::s_Instance;

bool Input::AreModifiersPressed(ModifierBits mods) {
    if ((mods & ALL_SUPPORTED_MODIFIER_BITS) == ModifierBits::None) {
        return false;
    }

    bool pressed = true;
    if ((mods & ModifierBits::Shift) != ModifierBits::None) {
        pressed = pressed && (IsKeyPressed(Key::LShift) || IsKeyPressed(Key::RShift));
    }
    if ((mods & ModifierBits::Ctrl) != ModifierBits::None) {
        pressed = pressed && (IsKeyPressed(Key::LCtrl) || IsKeyPressed(Key::RCtrl));
    }
    if ((mods & ModifierBits::Alt) != ModifierBits::None) {
        pressed = pressed && (IsKeyPressed(Key::LAlt) || IsKeyPressed(Key::RAlt));
    }

    return pressed;
}

bool Input::IsKeyPressed(Key key, ModifierBits mods) {
    if (key <= Key::Unknown || key >= Key::Last) {
        throw std::runtime_error("Invalid key pressed");
    }

    bool pressed = s_Instance.m_KeyPressed[(int)key];
    if (!pressed) {
        return false;
    }

    if (mods != ModifierBits::None) {
        return AreModifiersPressed(mods);
    }

    return true;
}

bool Input::IsKeyJustPressed(Key key, ModifierBits mods) {
    if (key <= Key::Unknown || key >= Key::Last) {
        throw std::runtime_error("Invalid key pressed");
    }

    bool pressed = s_Instance.m_KeyJustPressed[(int)key];
    if (!pressed) {
        return false;
    }

    if (mods != ModifierBits::None) {
        return AreModifiersPressed(mods);
    }

    return true;
}

bool Input::IsMouseButtonPressed(MouseButton mb, ModifierBits mods) {
    if (mb <= MouseButton::Unknown || mb >= MouseButton::Last) {
        throw std::runtime_error("Invalid mouse button pressed");
    }

    bool pressed = s_Instance.m_MouseButtonPressed[(int)mb];
    if (!pressed) {
        return false;
    }

    if (mods != ModifierBits::None) {
        return AreModifiersPressed(mods);
    }

    return true;
}

bool Input::IsMouseButtonJustPressed(MouseButton mb, ModifierBits mods) {
    if (mb <= MouseButton::Unknown || mb >= MouseButton::Last) {
        throw std::runtime_error("Invalid mouse button pressed");
    }

    bool pressed = s_Instance.m_MouseButtonJustPressed[(int)mb];
    if (!pressed) {
        return false;
    }

    if (mods != ModifierBits::None) {
        return AreModifiersPressed(mods);
    }

    return true;
}

void Input::AddKeyRegistry(Key key, Action action, ModifierBits mods) {
    if (key <= Key::Unknown || key >= Key::Last) {
        return;
    }

    if (action == Action::Press) {
        s_Instance.m_KeyPressed[(int)key] = true;
        s_Instance.m_KeyJustPressed[(int)key] = true;
    } else if (action == Action::Release) {
        s_Instance.m_KeyPressed[(int)key] = false;
        s_Instance.m_KeyJustPressed[(int)key] = false;
    }
}

void Input::AddMouseButtonRegistry(MouseButton mb, Action action, ModifierBits mods) {
    if (mb <= MouseButton::Unknown || mb >= MouseButton::Last) {
        return;
    }

    if (action == Action::Press) {
        s_Instance.m_MouseButtonPressed[(int)mb] = true;
        s_Instance.m_MouseButtonJustPressed[(int)mb] = true;
    } else if (action == Action::Release) {
        s_Instance.m_MouseButtonPressed[(int)mb] = false;
        s_Instance.m_MouseButtonJustPressed[(int)mb] = false;
    }
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
