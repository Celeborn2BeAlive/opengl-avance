#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/ViewController.hpp>

class Application
{
public:
    Application(int argc, char** argv);

    int run();
private:
    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;

    GLuint m_cubeVBO = 0;
    GLuint m_cubeIBO = 0;
    GLuint m_cubeVAO = 0;

    GLuint m_sphereVBO = 0;
    GLuint m_sphereIBO = 0;
    GLuint m_sphereVAO = 0;

    GLuint m_SceneVBO = 0;
    GLuint m_SceneIBO= 0;
    GLuint m_SceneVAO = 0;

    uint32_t m_sizeCubeIBO;

    uint32_t m_sizeSphereIBO;

	GLint uniform_mv;
	GLint uniform_normal;
    GLint uniform_mvp;

    // lumière directionnelle
    GLint uniform_DirectionalLightDir;
    GLint uniform_DirectionalLightIntensity;
    // lumière ponctuelle
    GLint uniform_PointLightPosition;
    GLint uniform_PointLightIntensity;
    // couleur diffuse
    GLint uniform_Kd;
    // Sampler
    GLuint samplerObject;
    GLuint uniform_KdSampler;

    GLuint tex1;
    GLuint tex2;

    float m_SceneSize;

    GLuint brickstex;
    GLuint floortex;

    glmlv::GLProgram m_program;
    glmlv::ViewController camera { m_GLFWHandle.window(), 3.f };


};