#include "Application.hpp"

#include <iostream>

#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glmlv/Image2DRGBA.hpp>

int Application::run()
{
    float clearColor[3] = { 0, 0, 0 };
    float cubeColor[3] = { 1, 1, 0 };
    float sphereColor[3] = { 0, 1, 1 };
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
        glm::mat4 view = camera.getViewMatrix();

        // light 
        glUniform3fv(uniform_DirectionalLightDir, 1, glm::value_ptr(glm::normalize(glm::vec3(view*glm::vec4(1,1,1,0)))));

        glUniform3f(uniform_DirectionalLightIntensity, 1, 1, 1);

        glUniform3fv(uniform_PointLightPosition, 1, glm::value_ptr(glm::vec3(view*glm::vec4(1,1,1,1))));
        
        glUniform3f(uniform_PointLightIntensity, 10, 10, 10);

        glUniform3f(uniform_Kd, cubeColor[0], cubeColor[1], cubeColor[2]);

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


        // Textures 
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(uniform_KdSampler, 0); // Set the uniform to 0 because we use texture unit 0
        glBindSampler(0, samplerObject); // Tell to OpenGL what sampler we want to use on this texture unit

        glBindTexture(GL_TEXTURE_2D, tex1);

        //glDrawElements(GL_TRIANGLES, m_sizeCubeIBO , GL_UNSIGNED_INT, 0);

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

        glUniform3f(uniform_Kd, sphereColor[0], sphereColor[1], sphereColor[2]);


        // Textures 
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(uniform_KdSampler, 0); // Set the uniform to 0 because we use texture unit 0
        glBindSampler(0, samplerObject); // Tell to OpenGL what sampler we want to use on this texture unit

        glBindTexture(GL_TEXTURE_2D, tex2);

       // glDrawElements(GL_TRIANGLES, m_sizeSphereIBO , GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        
        // SCENE RENDERING 

        projection = glm::perspective(70.f, float(viewportSize.x) / viewportSize.y, 0.01f * m_SceneSize, m_SceneSize); // near = 1% de la taille de la scene, far = 100%
        
        glBindVertexArray(m_SceneVAO);
    
        // model matrix
        glm::mat4 modelScene = glm::mat4(1);

        // mv matrix
        MVMatrix = view * modelScene;

        glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(MVMatrix));

        // normal matrix
        NormalMatrix = glm::transpose(glm::inverse(MVMatrix));
        glUniformMatrix4fv(uniform_normal, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        // mvp matrix
        MVPMatrix = projection * view * modelScene;
        glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(MVPMatrix));

        glUniform3f(uniform_Kd, 1, 0, 0);


        // Textures 
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(uniform_KdSampler, 0); // Set the uniform to 0 because we use texture unit 0
        glBindSampler(0, samplerObject); // Tell to OpenGL what sampler we want to use on this texture unit

        glBindTexture(GL_TEXTURE_2D, tex2);
        glBindVertexArray(m_SceneVAO);
        auto indexOffset = 0;
        for (const auto indexCount: data.indexCountPerShape)
        {
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (const GLvoid*) (indexOffset * sizeof(GLuint)));
            indexOffset += indexCount
        }
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
            if (ImGui::ColorEdit3("cube", cubeColor)) {
            }
            if (ImGui::ColorEdit3("sphere", sphereColor)) {
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
            camera.update(float(ellapsedTime));
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
    m_AssetsRootPath { m_AppPath.parent_path() / "assets" }

{

    // Here we load and compile shaders from the library
    
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });

    // Here we use glGetAttribLocation(program, attribname) to obtain attrib locations; We could also directly use locations if they are set in the vertex shader (cf. triangle app)
    const GLint positionAttrLocation = glGetAttribLocation(m_program.glId(), "aPosition");
    const GLint normalAttrLocation = glGetAttribLocation(m_program.glId(), "aNormal");
    const GLint texAttrLocation = glGetAttribLocation(m_program.glId(), "aTexCoords");

    // Textures CUBE & SPHERE
    glmlv::Image2DRGBA textures[2];
    textures[0] = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "tex1.jpg");
    textures[1] = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "tex2.jpg");
    
    glActiveTexture(GL_TEXTURE0);
    
    glGenTextures(1, &tex1);

    glBindTexture(GL_TEXTURE_2D, tex1);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, textures[0].width(), textures[0].height());
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textures[0].width(), textures[0].height(), GL_RGBA, GL_UNSIGNED_BYTE, textures[0].data());
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE0);

    glGenTextures(1, &tex2);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, textures[1].width(), textures[1].height());
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, textures[1].width(), textures[1].height(), GL_RGBA, GL_UNSIGNED_BYTE, textures[1].data());
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenSamplers(1, &samplerObject);
    glSamplerParameteri(samplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    uniform_KdSampler = glGetUniformLocation(m_program.glId(), "uKdSampler");
        

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


    /******* SCENE *******/
    glmlv::ObjData data;
    loadObj(m_AssetsRootPath / m_AppName / "models" / "sponza.obj", data);
    m_SceneSize = glm::length(data.bboxMax - data.bboxMin);
    
    // Textures SCENE
    glActiveTexture(GL_TEXTURE0);

    // Upload all textures to the GPU
    std::vector<GLint> textureIds;
    for (const auto & texture : data.textures)
    {
        GLuint texId = 0;
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, texture.width(), texture.height());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.width(), texture.height(), GL_RGBA, GL_UNSIGNED_BYTE, texture.data());
        glBindTexture(GL_TEXTURE_2D, 0);

        textureIds.emplace_back(texId);
    }

    /*glGenSamplers(1, &samplerObject);
    glSamplerParameteri(samplerObject, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(samplerObject, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    uniform_KdSampler = glGetUniformLocation(m_program.glId(), "uKdSampler");*/



 
 

    const auto sceneDiagonalSize = glm::length(data.bboxMax - data.bboxMin);
    camera.setSpeed(sceneDiagonalSize * 0.1f); // 10% de la scene parcouru par seconde
    const auto projMatrix = glm::perspective(70.f, float(viewportSize.x) / viewportSize.y, 0.01f * m_SceneSize, m_SceneSize); // near = 1% de la taille de la scene, far = 100%

    // VBO
    glGenBuffers(1, &m_SceneVBO);

    glBindBuffer(GL_ARRAY_BUFFER, m_SceneVBO);

    glBufferStorage(GL_ARRAY_BUFFER, data.vertexBuffer.size() * sizeof(glmlv::Vertex3f3f2f),  data.vertexBuffer.data(), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // IBO
    glGenBuffers(1, &m_SceneIBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_SceneIBO);

    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, data.indexBuffer.size() * sizeof(uint32_t), data.indexBuffer.data(), 0);

    m_sizeSphereIBO = data.indexBuffer.size();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //VAO 
    glGenVertexArrays(1, &m_SceneVAO);
    
    glBindVertexArray(m_SceneVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_SceneVBO);

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

    uniform_DirectionalLightDir = m_program.getUniformLocation("uDirectionalLightDir");
    uniform_DirectionalLightIntensity = m_program.getUniformLocation("uDirectionalLightIntensity");
    uniform_PointLightPosition = m_program.getUniformLocation("uPointLightPosition");
    uniform_PointLightIntensity = m_program.getUniformLocation("uPointLightIntensity");
    uniform_Kd = m_program.getUniformLocation("uKd");

    m_program.use();

    glEnable(GL_DEPTH_TEST);

    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

    camera.setViewMatrix(glm::lookAt(glm::vec3(5,5,5), glm::vec3(0,0,0), glm::vec3(0,1,0)));

    camera.setSpeed(m_SceneSize * 0.1f); // Let's travel 10% of the scene per second
}