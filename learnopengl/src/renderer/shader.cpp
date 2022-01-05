#include <renderer/shader.h>

#include <fstream>
#include <sstream>

/* ParseShader parses the shader source code from the filepath */
const std::string Shader::ParseShader(const std::string &filePath) {
    std::ifstream stream(filePath);
    if (stream.fail()) {
        spdlog::error("[Shader Error] Shader file '{}' does not exist", filePath);
        throw "Shader file not found";
    }

    std::string line;
    std::stringstream ss;
    while (getline(stream, line)) {
        ss << line << '\n';
    }

    return ss.str();
}

/* CompileShader compiles the shader based on type from the provided source string and returns the ID */
unsigned int Shader::CompileShader(const unsigned int type, const std::string &sourceVal) {
    unsigned int id = glCreateShader(type);
    const char *src = sourceVal.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Check shader compilation error and print if any
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        char *message = (char *)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        spdlog::error("[Shader Error] Failed to compile shader (type {}): {}", type, message);
        throw "Shader compilation failed";
    }

    return id;
}

unsigned int Shader::CreateProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    unsigned int program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    int result;
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        char *message = (char *)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, length, &length, message);
        spdlog::error("[Shader Error] Failed to link shader program: {}", message);
        throw "Shader program linkage failed";
    }

    glValidateProgram(program);
    glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        char *message = (char *)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, length, &length, message);
        spdlog::error("[Shader Error] Failed to validate shader program: {}", message);
        throw "Shader program validation failed";
    }

    return program;
}

/* Shader compiles the shaders from provided sources and links it to a program */
Shader::Shader(const std::string &vertexFilePath, const std::string &fragmentFilePath) : m_ReferenceID(0) {
    // Parse and compile each shader
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, Shader::ParseShader(vertexFilePath));
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, Shader::ParseShader(fragmentFilePath));

    // Create shader program
    unsigned int program = CreateProgram(vs, fs);

    // We can clear shader intermediates after linking them to program
    glDeleteShader(vs);
    glDeleteShader(fs);

    m_ReferenceID = program;
}

Shader::~Shader() {
    glDeleteProgram(m_ReferenceID);
}

void Shader::Bind() const {
    glUseProgram(m_ReferenceID);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

void Shader::SetUniform1f(const std::string &name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform1i(const std::string &name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3) {
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

int Shader::GetUniformLocation(const std::string &name) {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        return m_UniformLocationCache[name];
    }

    int location = glGetUniformLocation(m_ReferenceID, name.c_str());
    if (location == -1) {
        spdlog::warn("[Shader Warning] Uniform '{}' doesn't exist!", name);
    }

    m_UniformLocationCache[name] = location;
    return location;
}