#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <string>

#include "Renderer.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexBufferLayout.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Camera.hpp"
#include "Controls.hpp"
#include "FrameBuffer.hpp"
#include "GlobalFunctions.hpp"
#include "render_geom/CoordinateAxis/CoordinateAxis.hpp"
#include "render_geom/Plane/Plane.hpp"
#include "render_geom/Sphere/Sphere.hpp"
#include "render_geom/Circle/Circle.hpp"
#include "render_geom/Hopf/Hopf.hpp"
#include "render_geom/Points/Points.hpp"
#include "render_geom/Line/Line.hpp"
#include "Fluid/Fluid.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/color_space.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#define PI 3.14159265358979323846

int fullscreenWidth = 1920;
int fullscreenHeight = 1080;

int windowedWidth = 1920;
int windowedHeight = 1080;

int main()
{
    bool drawCoordinateAxis = false;
    bool fullscreen = false;
    bool vsync = true;
    float escCooldown = 0.5f;
    double lastKeyPressTime = 0;
    std::array<float, 4> boxDims = {-500.0f, 500.0f, -500.0f, 500.0f};

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);	//set the major version of OpenGL to 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);	//set the minor version of OpenGL to 3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//set the OpenGL profile to core

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowedWidth, windowedHeight, "TDSE", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;

    //INITIALIZATION OPTIONS

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_FRONT));
    GLCall(glFrontFace(GL_CCW));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glViewport(0, 0, windowedWidth, windowedHeight));
    //GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

    // IMGUI INITIALIZATION
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();
    {
        Renderer renderer;

        // CREATE SHADERS

        Shader shader("res/shaders/Basic.shader"); //create a shader
        Shader shader2("res/shaders/Points.shader"); //create a shader
        Shader shader3("res/shaders/Fluid.shader"); //create a shader

        // CREATE OBJECTS

        float nearPlaneDistance = 0.1;
        Camera camera(45.0f, (float)windowedWidth / (float)windowedHeight, nearPlaneDistance, 50000.0f, window, true);
        camera.SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
        camera.SetPerspective(false);

        Axis axis(2000.0f);

        //glfwSetWindowUserPointer(window, &camera);
        //glfwSetCursorPosCallback(window, mouse_callback);
        
        // MOVEMENT SETTINGS

        float fov = 45.0f;
        float speed = 2.5f;
        float sensitivity = 0.1f;

        // Box 

        Line top(glm::vec3(boxDims[0], boxDims[3], 0), glm::vec3(boxDims[1], boxDims[3], 0));
        Line bottom(glm::vec3(boxDims[0], boxDims[2], 0), glm::vec3(boxDims[1], boxDims[2], 0));
        Line left(glm::vec3(boxDims[0], boxDims[2], 0), glm::vec3(boxDims[0], boxDims[3], 0));
        Line right(glm::vec3(boxDims[1], boxDims[2], 0), glm::vec3(boxDims[1], boxDims[3], 0));        
        std::vector<Line> boxLines = {top, bottom, left, right};

        Fluid fluid(500, 50);

        Circle circle(fluid.GetParticles());

        fluid.BindCircle(&circle);

        // RENDERING LOOP
        while (!glfwWindowShouldClose(window))
        {
            float time = glfwGetTime(); // Get the current time in seconds

            glm::mat4 viewMatrix = camera.GetViewMatrix();
            glm::mat4 projectionMatrix = camera.GetProjectionMatrix();
            /* Render here */

            ImGui_ImplGlfwGL3_NewFrame();
            GLCall(glClearColor(0.529f, 0.828f, 0.952f, 1.0f));
            renderer.Clear();

            // STATUS WINDOW

            ImGui::SetNextWindowPos(ImVec2(20, 20));
            ImGui::SetNextWindowSize(ImVec2(400, 80));
            ImGui::Begin("Status:");
            ImGui::Text("Camera Position: %.3f, %.3f, %.3f", camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
            ImGui::Text("Camera Front: %.3f, %.3f, %.3f", camera.getFront().x, camera.getFront().y, camera.getFront().z);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("Time: %.3fs", time);
            ImGui::End();

            ImGui::Begin("Settings:");
            if(ImGui::SliderFloat4("Box Sides", &boxDims[0], 0.0f, 1000.0f))
            {
                left.UpdateVertices(glm::vec3(boxDims[0], boxDims[2], 0), glm::vec3(boxDims[0], boxDims[3], 0));
                right.UpdateVertices(glm::vec3(boxDims[1], boxDims[2], 0), glm::vec3(boxDims[1], boxDims[3], 0));
                top.UpdateVertices(glm::vec3(boxDims[0], boxDims[3], 0), glm::vec3(boxDims[1], boxDims[3], 0));
                bottom.UpdateVertices(glm::vec3(boxDims[0], boxDims[2], 0), glm::vec3(boxDims[1], boxDims[2], 0));
            }
            if(ImGui::Button("Print Velocities"))
            {
                fluid.PrintVelocities();
            }

            ImGui::End();

            if(drawCoordinateAxis)
            {
                //Coordinate axis
                shader.Bind();
                glm::mat4 model = glm::mat4(1.0f); //create a model matrix
                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f); //set the uniform
                glLineWidth(3.0f);
                axis.Draw();
            }

            
            for(int i = 0; i < boxLines.size(); i++)
            {
                shader.Bind();
                glm::mat4 model = glm::mat4(1.0f); //create a model matrix
                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                shader.SetUniformMat4f("u_MVP", mvp); //set the uniform
                shader.SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f); //set the uniform
                glLineWidth(3.0f);
                boxLines[i].Draw();
            }
            fluid.Update();


            {
                shader3.Bind();
                glm::mat4 model = glm::mat4(1.0f); //create a model matrix
                glm::mat4 mvp = projectionMatrix * viewMatrix * model;
                shader3.SetUniformMat4f("u_MVP", mvp); //set the uniform
                circle.Draw();
            }

            
            // CAMERA CONTROLS
            camera.SetFOV(fov);
            camera.SetSpeed(speed);
            camera.SetSensitivity(sensitivity);

            if(glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && time - lastKeyPressTime > escCooldown)
            {
                if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                }
                else
                {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
                lastKeyPressTime = time;
            }

            if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS && time - lastKeyPressTime > escCooldown)
            {
                fullscreen = !fullscreen; // Toggle the fullscreen flag
                if (fullscreen) {
                    // Store the window size to restore later
                    glfwGetWindowSize(window, &fullscreenWidth, &fullscreenHeight);

                    // Get the primary monitor's resolution
                    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

                    // Switch to fullscreen
                    glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
                    glViewport(0, 0, mode->width, mode->height);
                    SetVsync(vsync);
                } 
                else 
                {
                    // Switch to windowed mode
                    glfwSetWindowMonitor(window, nullptr, 100, 100, windowedWidth, windowedHeight, GLFW_DONT_CARE);
                    glViewport(0, 0, windowedWidth, windowedHeight);
                    SetVsync(vsync);
                }
                lastKeyPressTime = time;
            }

            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}