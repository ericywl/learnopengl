#pragma once

#include <common.h>
#include <core/input.h>

#include <string>

class Window {
   private:
    std::pair<int, int> m_RestoredPosition = {0, 0};
    std::pair<int, int> m_RestoredSize = {0, 0};
    std::pair<int, int> m_Size = {0, 0};
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

    void SetViewport(int width, int height);
    void SetVSync(bool on);
    void SetInputSystem(bool on);
    void SetCaptureCursor(bool on);
    void SetMSAASamples(unsigned int samples);

    inline int GetWidth() const {
        return m_Size.first;
    }

    inline int GetHeight() const {
        return m_Size.second;
    }

    inline void SetSize(std::pair<int, int> size) {
        m_Size = size;
    }

   private:
    void fetchRestoreWindowSizeAndPosition();
};
