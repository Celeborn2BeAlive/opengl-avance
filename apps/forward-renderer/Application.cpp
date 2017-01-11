#include "Application.hpp"

#include <iostream>

#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto viewportSize = m_GLFWHandle.framebufferSize();
        const auto seconds = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // CUBE Rendering 
        glBindVertexArray(m_cubeVAO);

        // projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(70.f), float(viewportSize.x)/viewportSize.y, 0.01f, 100.f);

        // view matrix
        glm::mat4 view = glm::lookAt(glm::vec3(5,5,5), glm::vec3(0,0,0), glm::vec3(0,1,0));

        // model matrix
        glm::mat4 modelCube = glm::mat4(1);
        modelCube = glm::translate(modelCube, glm::vec3(-5, 0, 0));
        modelCube= glm::scale(modelCube, glm::vec3(1.2, 1.2, 1.2)); 

        // mv matrix
        glm::mat4 MVMatrix = view * modelCube;
        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(MVMatrix));

        // normal matrix
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(uniform_normal, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        // mvp matrix
        glm::mat4 MVPMatrix = projection * view * modelCube;
        glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(MVPMatrix));

        glDrawElements(GL_TRIANGLES, m_sizeCubeIBO , GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        

        // SPHERE Rendering
        glBindVertexArray(m_sphereVAO);
    
        // model matrix
        glm::mat4 modelSphere = glm::mat4(1);

        // mv matrix
        MVMatrix = view * modelSphere;

        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(MVMatrix));

        // normal matrix
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(uniform_normal, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        // mvp matrix
        MVPMatrix = projection * view * modelSphere;
        glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(MVPMatrix));

        glDrawElements(GL_TRIANGLES, m_sizeSphereIBO , GL_UNSIGNED_INT, 0);

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

    // Here we load and compile shaders from the library
    
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });
    
    // Here we use glGetAttribLocation(program, attribname) to obtain attrib locations; We could also directly use locations if they are set in the vertex shader (cf. triangle app)
    const GLint positionAttrLocation = glGetAttribLocation(m_program.glId(), "aPosition");
    const GLint normalAttrLocation = glGetAttribLocation(m_program.glId(), "aNormal");
    const GLint texAttrLocation = glGetAttribLocation(m_program.glId(), "aTexCoords");

    /******* CUBE *******/
    
    glmlv::SimpleGeometry cube = glmlv::makeCube();

    // VBO
    glGenBuffers(1, &m_cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);

    glBufferStorage(GL_ARRAY_BUFFER, cube.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f),  cube.vertexBuffer.data(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // IBO
    glGenBuffers(1, &m_cubeIBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO);

    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, cube.indexBuffer.size() * sizeof(uint32_t), cube.indexBuffer.data(), 0);

    m_sizeCubeIBO = cube.indexBuffer.size();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //VAO 
    glGenVertexArrays(1, &m_cubeVAO);

    glBindVertexArray(m_cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

    glEnableVertexAttribArray(normalAttrLocation);
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));

    glEnableVertexAttribArray(texAttrLocation);
    glVertexAttribPointer(texAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);



    /******* SPHERE *******/
    glmlv::SimpleGeometry sphere = glmlv::makeSphere(32);

    // VBO
    glGenBuffers(1, &m_sphereVBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);

    glBufferStorage(GL_ARRAY_BUFFER, sphere.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f),  sphere.vertexBuffer.data(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // IBO
    glGenBuffers(1, &m_sphereIBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphereIBO);

    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, sphere.indexBuffer.size() * sizeof(uint32_t), sphere.indexBuffer.data(), 0);

    m_sizeSphereIBO = sphere.indexBuffer.size();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //VAO 
    glGenVertexArrays(1, &m_sphereVAO);
    
    glBindVertexArray(m_sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_sphereVBO);

    glEnableVertexAttribArray(positionAttrLocation);
    glVertexAttribPointer(positionAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, position));

    glEnableVertexAttribArray(normalAttrLocation);
    glVertexAttribPointer(normalAttrLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, normal));

    glEnableVertexAttribArray(texAttrLocation);
    glVertexAttribPointer(texAttrLocation, 2, GL_FLOAT, GL_FALSE, sizeof(glmlv::Vertex3f3f2f), (const GLvoid*) offsetof(glmlv::Vertex3f3f2f, texCoords));
    

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphereIBO);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);




    uniform_mv = m_program.getUniformLocation("uMVMatrix");

    uniform_mvp = m_program.getUniformLocation("uMVPMatrix");

    uniform_normal = m_program.getUniformLocation("uNormalMatrix");

    m_program.use();

    glEnable(GL_DEPTH_TEST);

    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file
}