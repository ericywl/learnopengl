#include <common.h>
#include <core/window.h>

#include <iostream>
#include <stdexcept>

/* GLMessageCallback should get called whenever OpenGL encounters some error, which then prints out the error message in
 * some simple formatting */
void GLAPIENTRY GLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                  const GLchar *message, const void *userParam) {
    // Ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::stringstream ss;
    ss << "[OpenGL Error " << id << "] " << message << '\n';

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            ss << " | Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            ss << " | Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            ss << " | Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            ss << " | Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            ss << " | Source: Application";
            break;
        default:
            ss << " | Source: Other";
            break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            ss << " | Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            ss << " | Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            ss << " | Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            ss << " | Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            ss << " | Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            ss << " | Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            ss << " | Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            ss << " | Type: Pop Group";
            break;
        default:
            ss << " | Type: Other";
            break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            spdlog::critical(ss.str());
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            spdlog::error(ss.str());
            break;
        case GL_DEBUG_SEVERITY_LOW:
            spdlog::warn(ss.str());
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            spdlog::info(ss.str());
            break;
        default:
            spdlog::debug(ss.str());
            break;
    }
}

void GLFWErrorCallback(int, const char *errStr) {
    spdlog::error("[GLFW Error] {}", errStr);
}

void GLFWFrameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    Window *w = static_cast<Window *>(glfwGetWindowUserPointer(window));
    w->SetSize({width, height});
    w->SetViewport(width, height);
}

Window::Window(const int width, const int height, const std::string &name) {
    // Initialize the library
    if (!glfwInit()) {
        spdlog::critical("[GLFW Error] Failed to initialize GLFW");
        throw std::runtime_error("Failed to initialize GLFW");
    };

    // Set GLFW initialization values and error callback
    glfwSetErrorCallback(GLFWErrorCallback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // Create a window and its OpenGL context
    GLFWwindow *w = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (!w) {
        spdlog::critical("[GLFW Error] Failed to create GLFW window");
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Make the w's context current
    glfwMakeContextCurrent(w);
    glfwSetWindowUserPointer(w, static_cast<void *>(this));

    // Initialize GLAD after making context
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::critical("[GLAD Error] Failed to initialize GLAD");
        throw std::runtime_error("Failed to initialize GLAD");
    }

    spdlog::info("GL version: {}", glGetString(GL_VERSION));

    // Enable OpenGL debug message callback
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    glDebugMessageCallback(GLMessageCallback, NULL);

    // Set viewport
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(w, GLFWFrameBufferSizeCallback);

    m_Window = w;
    m_Monitor = glfwGetPrimaryMonitor();
    m_Size = {width, height};
    fetchRestoreWindowSizeAndPosition();
}

Window::~Window() {
    spdlog::debug("Window destroyed");
    glfwDestroyWindow(m_Window);
    glfwTerminate();
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
    if (!IsFullScreen()) {
        // Backup window position and window size
        fetchRestoreWindowSizeAndPosition();
        // Get monitor resolution
        const GLFWvidmode *mode = glfwGetVideoMode(m_Monitor);
        // Switch to full screen
        glfwSetWindowMonitor(m_Window, m_Monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        // Restore last window size and position
        glfwSetWindowMonitor(m_Window, nullptr, m_RestoredPosition.first, m_RestoredPosition.second,
                             m_RestoredSize.first, m_RestoredSize.second, 0);
    }
}

void Window::SetViewport(int width, int height) {
    glViewport(0, 0, width, height);
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

void Window::fetchRestoreWindowSizeAndPosition() {
    glfwGetWindowPos(m_Window, &m_RestoredPosition.first, &m_RestoredPosition.second);
    glfwGetWindowSize(m_Window, &m_RestoredSize.first, &m_RestoredSize.second);
}
