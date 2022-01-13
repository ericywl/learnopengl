#include <common.h>
#include <core/window.h>

#include <stdexcept>

void GLFWFrameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

Window::Window(const int width, const int height, const std::string &name) {
    // Create a window and its OpenGL context
    GLFWwindow *w = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!w) {
        spdlog::critical("[GLFW Error] Failed to create GLFW window");
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Make the w's context current
    glfwMakeContextCurrent(w);
    glfwSetWindowUserPointer(w, this);

    // Initialize GLAD after making context
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::critical("[GLAD Error] Failed to initialize GLAD");
        throw std::runtime_error("Failed to initialize GLAD");
    }

    // Set viewport
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(w, GLFWFrameBufferSizeCallback);

    m_Window = w;
    m_Monitor = glfwGetPrimaryMonitor();
    fetchWindowSizeAndPosition();
}

Window::~Window() {
    glfwDestroyWindow(m_Window);
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

void Window::SwapBuffers() const {
    glfwSwapBuffers(m_Window);
}

void Window::PollEvents() const {
    if (m_InputStarted) {
        Input::FrameReset();
    }
    glfwPollEvents();
}

void Window::Close() const {
    glfwSetWindowShouldClose(m_Window, GL_TRUE);
}

bool Window::IsFullScreen() const {
    return glfwGetWindowMonitor(m_Window) != nullptr;
}

void Window::ToggleFullScreen() {
    bool fullscreen = !IsFullScreen();

    if (fullscreen) {
        // Backup window position and window size
        fetchWindowSizeAndPosition();
        // Get monitor resolution
        const GLFWvidmode *mode = glfwGetVideoMode(m_Monitor);
        // Switch to full screen
        glfwSetWindowMonitor(m_Window, m_Monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        // Restore last window size and position
        glfwSetWindowMonitor(m_Window, nullptr, m_Position.first, m_Position.second, m_Size.first, m_Size.second, 0);
    }
}

void Window::SetVSync(bool on) {
    if (on) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }
}

void Window::SetInputSystem(bool on) {
    if (on) {
        m_InputStarted = true;
        glfwSetKeyCallback(m_Window, Input::KeyCallback);
        glfwSetMouseButtonCallback(m_Window, Input::MouseButtonCallback);
        glfwSetCursorPosCallback(m_Window, Input::MousePositionCallback);
        glfwSetScrollCallback(m_Window, Input::MouseScrollCallback);
    } else {
        m_InputStarted = false;
        glfwSetKeyCallback(m_Window, NULL);
        glfwSetMouseButtonCallback(m_Window, NULL);
        glfwSetCursorPosCallback(m_Window, NULL);
        glfwSetScrollCallback(m_Window, NULL);
    }
}

void Window::SetCaptureCursor(bool on) {
    if (on) {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void Window::SetMSAASamples(unsigned int samples) {
    glfwWindowHint(GLFW_SAMPLES, samples);
}

void Window::fetchWindowSizeAndPosition() {
    glfwGetWindowPos(m_Window, &m_Position.first, &m_Position.second);
    glfwGetWindowSize(m_Window, &m_Size.first, &m_Size.second);
}
