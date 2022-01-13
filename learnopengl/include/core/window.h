#pragma once

#include <common.h>
#include <core/input.h>

#include <string>

class Window {
   private:
    std::pair<int, int> m_Position = {0, 0};
    std::pair<int, int> m_Size = {0, 0};
    std::pair<int, int> m_ViewportSize = {0, 0};
    GLFWwindow *m_Window = nullptr;
    GLFWmonitor *m_Monitor = nullptr;
    bool m_InputStarted = false;
    bool m_VSync = false;

   public:
    Window(const int width, const int height, const std::string &name);
    ~Window();

    void PollEvents() const;
    void SwapBuffers() const;
    bool ShouldClose() const;
    void Close() const;

    bool IsFullScreen() const;
    void ToggleFullScreen();

    void SetVSync(bool on);
    void SetInputSystem(bool on);
    void SetCaptureCursor(bool on);
    void SetMSAASamples(unsigned int samples);

   private:
    void fetchWindowSizeAndPosition();
};
