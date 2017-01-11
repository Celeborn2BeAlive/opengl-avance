#include "Application.hpp"

#include <iostream>

#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex
{
    glm::vec2 position;
    glm::vec3 color;

    Vertex(glm::vec2 position, glm::vec3 color):
        position(position), color(color)
    {}
};

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Put here rendering code
        glm::mat4 ProjMatrix, MVMatrix, NormalMatrix, ViewMatrix;
        glm::mat4 CubeMatrix, SphereMatrix;
        
        CubeMatrix = glm::mat4(1.0f);
        SphereMatrix = glm::mat4(1.0f);
        //La convention OpenGL est de regarder du coté négatif de l'axe Z dans l'espace View: 
        ViewMatrix = glm::lookAt(glm::vec3(0.0,0.0,0.0), glm::vec3(0.0,0.0,-5.0), glm::vec3(0.0,1.0,0.0)); // eye, center, up
		ProjMatrix = glm::perspective(glm::radians(70.f), 1280.0f/720.0f, 0.1f, 100.0f); //(float fovy, float aspect, float zNear, float zFar)
		
		//MVMatrix = glm::translate(glm::mat4(1.0f),glm::vec3(0.0,0.0,-5.0));
        
        
        glBindVertexArray(m_frVAO);

		GLint nb_sommets = 0;
		glBindBuffer(GL_ARRAY_BUFFER, m_frIBO);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &nb_sommets);

		MVMatrix = glm::translate(ViewMatrix*CubeMatrix, glm:vec3(0.0,0.0,-5.0));
		NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glUniformMatrix4fv(ulocMVPMat, 1, GL_FALSE, glm::value_ptr(ProjMatrix*MVMatrix));
		glUniformMatrix4fv(ulocMVMat, 1, GL_FALSE, glm::value_ptr(MVMatrix));
		glUniformMatrix4fv(ulocNormMat, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
		
        glDrawElements(GL_TRIANGLES, nb_sommets, GL_UNSIGNED_INT, nullptr);
        
        
        
		
        glBindVertexArray(0);

        // GUI code:
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
            if (ImGui::ColorEdit3("clearColor", clearColor)) {
                glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
            }
            ImGui::End();
        }

        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        ImGui::Render();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            //viewController.update(float(ellapsedTime))
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" }

{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file
    
    //----------Init VBO, sending data to VBO----------
    glGenBuffers(1, &m_frVBO);

    glmlv::SimpleGeometry cube = glmlv::makeCube();

    glBindBuffer(GL_ARRAY_BUFFER, m_frVBO);

    glBufferStorage(GL_ARRAY_BUFFER, cube.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f), cube.vertexBuffer.data(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


	//----------Init IBO, sending data to IBO----------
    glGenBuffers(1, &m_frIBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_frIBO); // Pas ELEMENT (voir wiki Vertex Specification)

    glBufferStorage(GL_ARRAY_BUFFER, sizeof(uint32_t), cube.indexBuffer.data(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


	//----------Init VAO----------
    glGenVertexArrays(1, &m_frVAO);

    // Here we load and compile shaders from the library
    //Exo Transformations
    m_program = glmlv::compileProgram({ m_ShadersRootPath / "forward-renderer-transfo" / "forward.vs.glsl", m_ShadersRootPath / "forward-renderer-transfo" / "forward.fs.glsl" });

    // Here we use glGetAttribLocation(program, attribname) to obtain attrib locations; We could also directly use locations if they are set in the vertex shader (cf. triangle app)
    const GLint positionAttrLocation = glGetAttribLocation(m_program.glId(), "aVertexPosition");
    const GLint colorAttrLocation = glGetAttribLocation(m_program.glId(), "aVertexNormal");
	
	ulocMVPMat = glGetUniformLocation(m_program.glId(), "uMVPMatrix");
	ulocMVMat = glGetUniformLocation(m_program.glId(), "uMVMatrix");
	ulocNormMat = glGetUniformLocation(m_program.glId(), "uNormalMatrix");
	
	glEnable(GL_DEPTH_TEST);
	
	m_program.use();


	//----------Sending data to VAO---------- (comment lire les donnees du VBO)
    glBindVertexArray(m_frVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_frVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

    glEnableVertexAttribArray(colorAttrLocation);
    glVertexAttribPointer(colorAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_frIBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

Application::~Application()
{
    if (m_frVBO) {
        glDeleteBuffers(1, &m_frVBO);
    }

    if (m_frIBO) {
        glDeleteBuffers(1, &m_frIBO);
    }

    if (m_frVAO) {
        glDeleteBuffers(1, &m_frVAO);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
