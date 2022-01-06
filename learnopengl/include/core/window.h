#pragma once

#include <common.h>
#include <core/input.h>

#include <map>
#include <string>

class Window {
   private:
    GLFWwindow *m_Window;

   public:
    Window(const int width, const int height, const std::string &name);
    ~Window();

    void PollEvents() const;
    void SwapBuffers() const;
    bool ShouldClose() const;
    void Close() const;

    void SetVSync(bool on) const;
    void StartInputSystem();
};
