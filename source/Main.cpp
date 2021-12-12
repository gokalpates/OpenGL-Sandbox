#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "MSFramebufferObject.h"
#include "Callback.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Grid.h"
#include "Debug.h"
#include "DirectionalLight.h"
#include "PointLight.h"

float deltaTime = 0.0, currentFrame, lastFrame = 0.f;
float diffTime = 0.0, currentTime, lastTime = 0.f;
int fpsToShow = 0;
size_t counter = 0;

int main()
{
    int screenWidth = 2560, screenHeight = 1440;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Window", nullptr, NULL);
    if (window == NULL)
    {
        printf("ERROR: GLFW could not create a window.\n");
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, windowSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("ERROR: Failed to initialise GLAD.\n");
        glfwTerminate();
        std::exit(EXIT_FAILURE);
    }

    //General States.
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glEnable(GL_DEPTH_TEST);

    //OGL primitives.
    Camera camera(window);
    MSFramebufferObject MSFramebuffer(window);
    Grid grid;

    //Resources.
    std::vector<float> vertices = {
        //Position  //Normal    //Textures
        0.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,
        1.f,0.f,0.f,0.f,0.f,1.f,1.f,0.f,
        1.f,1.f,0.f,0.f,0.f,1.f,1.f,1.f,

        0.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,
        1.f,1.f,0.f,0.f,0.f,1.f,1.f,1.f,
        0.f,1.f,0.f,0.f,0.f,1.f,0.f,1.f
    };

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);

    unsigned int diffuseMap = loadTextureFromDisk("resources/brickwall.jpg");
    unsigned int normalMap = loadTextureFromDisk("resources/brickwall_normal.jpg");

    //Shaders.
    Shader lighting("shaders/basicLight.vert", "shaders/basicLight.frag");
    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");

    //Matrices and variables.
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)screenWidth / (float)screenHeight, 0.1f, 100.f);
    glm::mat4 view = glm::mat4(1.f);
    glm::vec3 cameraPosition = glm::vec3(0.f);

    //Light.
    glm::vec3 lightPosition = glm::vec3(0.5f, 0.5f, 2.f);
    glm::vec3 lightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 lightDiffuse = glm::vec3(0.7f, 0.7f, 0.7f);
    glm::vec3 lightSpecular = glm::vec3(1.f, 1.f, 1.f);

    //Set light uniforms.
    lighting.use();
    lighting.setVec3("light.ambient", lightAmbient);
    lighting.setVec3("light.diffuse", lightDiffuse);
    lighting.setVec3("light.specular", lightSpecular);
    lighting.setVec3("light.position", lightPosition);
    lighting.setFloat("material.shininess", 32.f);

    lighting.setInt("material.diffuse0", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    lighting.setInt("material.normal0", 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normalMap);

    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        currentTime = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        diffTime = currentTime - lastTime;
        lastFrame = currentFrame;
        
        if (diffTime >= 1.0)
        {
            fpsToShow = counter;
            counter = 0;
            lastTime = currentTime;
            glfwSetWindowTitle(window, std::to_string(fpsToShow).c_str());
        }

        camera.update();
        view = camera.getViewMatrix();
        cameraPosition = camera.getCameraPosition();

        lighting.use();
        lighting.setVec3("scene.viewPosition", cameraPosition);
        lightPosition.x = glm::cos(glfwGetTime()) * 5.f;
        lighting.setVec3("light.position", lightPosition);

        glfwPollEvents();
        if (glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window,GLFW_KEY_X))
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window,GLFW_KEY_E))
            lighting.setBool("scene.normalMapEnabled", true);
        if (glfwGetKey(window, GLFW_KEY_R))
            lighting.setBool("scene.normalMapEnabled", false);

        MSFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lighting.use();
        glm::mat4 model = glm::mat4(1.f);
        lighting.setAllMat4(model, view, projection);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        gridShader.use();
        model = glm::mat4(1.f);
        gridShader.setAllMat4(model, view, projection);
        grid.draw(gridShader);

        MSFramebuffer.unbind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFramebuffer.getFramebufferObjectId());
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        //------------------SWAP BUFFERS AND RENDER GUI------------------
        glfwSwapBuffers(window);
        counter++;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}