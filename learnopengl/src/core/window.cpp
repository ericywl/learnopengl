#include <core/window.h>

#include <sstream>

void GLFWErrorCallback(int, const char *errStr) {
    spdlog::error("[GLFW Error] {}", errStr);
}

void GLFWFrameBufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

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

Window::Window(const int width, const int height, const std::string &name) : m_Window(nullptr) {
    // Initialize the library
    if (!glfwInit()) {
        spdlog::critical("[GLFW Error] Failed to initialize GLFW");
        throw "Failed to initialize GLFW";
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
        throw "Failed to create GLFW window";
    }

    // Make the w's context current
    glfwMakeContextCurrent(w);

    // Initialize GLAD after making context
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        spdlog::critical("[GLAD Error] Failed to initialize GLAD");
        throw "Failed to initialize GLAD";
    }

    spdlog::info("GL version: {}", glGetString(GL_VERSION));
    // Enable OpenGL debug message callback
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    glDebugMessageCallback(GLMessageCallback, NULL);

    // Viewport settings
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(w, GLFWFrameBufferSizeCallback);

    m_Window = w;
}

Window::~Window() {
    glfwDestroyWindow(static_cast<GLFWwindow *>(m_Window));
    glfwTerminate();
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(static_cast<GLFWwindow *>(m_Window));
}

void Window::SwapBuffers() const {
    glfwSwapBuffers(static_cast<GLFWwindow *>(m_Window));
}

void Window::PollEvents() const {
    glfwPollEvents();
}

void Window::Close() const {
    glfwSetWindowShouldClose(static_cast<GLFWwindow *>(m_Window), GL_TRUE);
}

void Window::SetVSync(bool on) const {
    if (on) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }
}

void Window::AddInputSystem(InputSystem &is, const std::string &name) {
    bool notInitYet = m_InputSystems.empty();
    is.SetEnabled(true);
    m_InputSystems[name] = &is;

    if (notInitYet) {
        // Only set this user pointer and callback once
        glfwSetWindowUserPointer(static_cast<GLFWwindow *>(m_Window), this);

        // Callback function for key action
        auto func = [](GLFWwindow *w, int key, int scancode, int action, int mods) {
            // Get all input systems that belong to the window
            std::map<std::string, InputSystem *> inputSystems =
                static_cast<Window *>(glfwGetWindowUserPointer(w))->GetInputSystems();

            // Add key registry into input system
            std::map<std::string, InputSystem *>::iterator it;
            for (it = inputSystems.begin(); it != inputSystems.end(); it++) {
                if (it->second) {
                    it->second->AddKeyRegistry(Key(key), Action(action), ModifierBits(mods));
                }
            }
        };

        // Set callback
        glfwSetKeyCallback(static_cast<GLFWwindow *>(m_Window), func);
    }
}

InputSystem *Window::GetInputSystem(const std::string &name) {
    std::map<std::string, InputSystem *>::iterator it = m_InputSystems.find(name);
    if (it == m_InputSystems.end()) {
        return nullptr;
    }

    return m_InputSystems[name];
}
