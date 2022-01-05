#pragma once

#include <common.h>

#include <string>
#include <unordered_map>

class Shader {
   private:
    unsigned int m_ReferenceID;
    std::unordered_map<std::string, int> m_UniformLocationCache;

   public:
    Shader(const std::string &vertexFilePath, const std::string &fragmentFilePath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetUniform1f(const std::string &name, float value);
    void SetUniform1i(const std::string &name, int value);
    void SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3);

   private:
    int GetUniformLocation(const std::string &name);
    const std::string ParseShader(const std::string &filepath);
    unsigned int CompileShader(const unsigned int type, const std::string &sourceVal);
    unsigned int CreateProgram(unsigned int vertexShader, unsigned int fragmentShader);
};
