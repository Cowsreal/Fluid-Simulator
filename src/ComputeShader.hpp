#pragma once

#include "Renderer.hpp"

struct ComputeShaderProgramSource
{
    std::string ComputeSource;
};

class ComputeShader
{
public:
    ComputeShader(const std::string& filepath);
    ~ComputeShader();

    void Bind() const;
    void Unbind() const;

    void Dispatch(unsigned int numGroupsX, unsigned int numGroupsY, unsigned int numGroupsZ) const;
private:
    std::string m_FilePath;
    unsigned int m_RendererID;

    ComputeShaderProgramSource ParseShader(const std::string& filepath);
    unsigned int CreateAndCompileShader(const std::string& ComputeShader);
};