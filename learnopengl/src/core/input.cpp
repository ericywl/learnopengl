#include <core/input.h>

#include <stdexcept>

InputInst Input::s_Instance;

bool Input::areModifiersPressed(ModifierBit mods) {
    if ((mods & ALL_SUPPORTED_MODIFIER_BITS) == ModifierBit::None) {
        return false;
    }

    bool pressed = true;
    if (static_cast<int>(mods & ModifierBit::Shift)) {
        pressed = pressed && (IsKeyPressed(Key::LShift) || IsKeyPressed(Key::RShift));
    }
    if (static_cast<int>(mods & ModifierBit::Ctrl)) {
        pressed = pressed && (IsKeyPressed(Key::LCtrl) || IsKeyPressed(Key::RCtrl));
    }
    if (static_cast<int>(mods & ModifierBit::Alt)) {
        pressed = pressed && (IsKeyPressed(Key::LAlt) || IsKeyPressed(Key::RAlt));
    }

    return pressed;
}

bool Input::IsKeyPressed(Key key, ModifierBit mods) {
    if (key <= Key::Unknown || key >= Key::Last) {
        throw std::runtime_error("Invalid key pressed");
    }

    bool pressed = s_Instance.m_KeyPressed[(int)key];
    if (!pressed) {
        return false;
    }

    if (mods != ModifierBit::None) {
        return areModifiersPressed(mods);
    }

    return true;
}

bool Input::IsKeyJustPressed(Key key, ModifierBit mods) {
    if (key <= Key::Unknown || key >= Key::Last) {
        throw std::runtime_error("Invalid key pressed");
    }

    bool pressed = s_Instance.m_KeyJustPressed[(int)key];
    if (!pressed) {
        return false;
    }

    if (mods != ModifierBit::None) {
        return areModifiersPressed(mods);
    }

    return true;
}

bool Input::IsMouseButtonPressed(MouseButton mb, ModifierBit mods) {
    if (mb <= MouseButton::Unknown || mb >= MouseButton::Last) {
        throw std::runtime_error("Invalid mouse button pressed");
    }

    bool pressed = s_Instance.m_MouseButtonPressed[(int)mb];
    if (!pressed) {
        return false;
    }

    if (mods != ModifierBit::None) {
        return areModifiersPressed(mods);
    }

    return true;
}

bool Input::IsMouseButtonJustPressed(MouseButton mb, ModifierBit mods) {
    if (mb <= MouseButton::Unknown || mb >= MouseButton::Last) {
        throw std::runtime_error("Invalid mouse button pressed");
    }

    bool pressed = s_Instance.m_MouseButtonJustPressed[(int)mb];
    if (!pressed) {
        return false;
    }

    if (mods != ModifierBit::None) {
        return areModifiersPressed(mods);
    }

    return true;
}

void Input::AddKeyRegistry(Key key, Action action, ModifierBit mods) {
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

void Input::AddMouseButtonRegistry(MouseButton mb, Action action, ModifierBit mods) {
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
