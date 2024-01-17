#include "ComputeShader.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

ComputeShader::ComputeShader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    ComputeShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateAndCompileShader(source.ComputeSource);
    GLCall(glUseProgram(m_RendererID));
}

ComputeShader::~ComputeShader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

ComputeShaderProgramSource ComputeShader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);
    std::string line;
    std::stringstream stringStream[1];
    while (getline(stream, line))
    {
        stringStream[0] << line << '\n';
    }
    return { stringStream[0].str() };
}

unsigned int ComputeShader::CreateAndCompileShader(const std::string& ComputeShader)
{
    GLCall(unsigned int id = glCreateShader(GL_COMPUTE_SHADER));

    const char* src = ComputeShader.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    GLCall(unsigned int program = glCreateProgram());
    GLCall(glAttachShader(program, id));
    GLCall(glLinkProgram(program));

    int result;
    GLCall(glGetProgramiv(program, GL_LINK_STATUS, &result));

    if (!result)
    {
        char error[1024];
        GLCall(glGetProgramInfoLog(program, 1024, NULL, error));
        std::cout << "Failed to link program." << std::endl;
        std::cout << error << std::endl;
        GLCall(glDeleteProgram(program));
    }

    GLCall(glValidateProgram(program));
    GLCall(glDeleteShader(id));

    return program;
}

void ComputeShader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void ComputeShader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void ComputeShader::Dispatch(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ) const
{
    GLCall(glDispatchCompute(numGroupsX, numGroupsY, numGroupsZ));
    GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
}
