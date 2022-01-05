#pragma once

#include <common.h>
#include <core/input.h>

#include <map>
#include <string>

class Window {
   private:
    void *m_Window;
    std::map<std::string, InputSystem *> m_InputSystems;

   public:
    Window(const int width, const int height, const std::string &name);
    ~Window();

    void PollEvents() const;
    void SwapBuffers() const;
    bool ShouldClose() const;
    void Close() const;

    void SetVSync(bool on) const;
    void AddInputSystem(InputSystem &is, const std::string &name);
    InputSystem *GetInputSystem(const std::string &name);

    inline std::map<std::string, InputSystem *> GetInputSystems() {
        return m_InputSystems;
    }
};
