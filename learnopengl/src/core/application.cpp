#include <core/application.h>

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

Application::Application(const int width, const int height, const std::string &name) {
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

    Window *window = new Window(width, height, name);
    m_Windows.push_back(window);

    spdlog::info("GL version: {}", glGetString(GL_VERSION));

    // Enable OpenGL debug message callback
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    glDebugMessageCallback(GLMessageCallback, NULL);
}

Application::~Application() {
    glfwTerminate();
}

Window &Application::GetWindow(unsigned int i) {
    if (i > m_Windows.size()) {
        throw std::runtime_error("Window index larger than size");
    }

    return *m_Windows[i];
}

Window &Application::NewWindow(const int width, const int height, const std::string &name) {
    Window *window = new Window(width, height, name);
    m_Windows.push_back(window);
    return *window;
}
