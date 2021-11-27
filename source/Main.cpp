#include <iostream>
#include <string>

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
    int screenWidth = 2560;
    int screenHeight = 1440;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Window", NULL, NULL);
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

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    MSFramebufferObject msFramebuffer(window, 8u);

    Camera camera(window);
    camera.setCameraSpeed(15.f);

    glm::vec3 cameraPosition = camera.getCameraPosition();
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)screenWidth / (float)screenHeight, 0.1f, 1000.f);

    Shader lightSourceShader("shaders/lightSource.vert", "shaders/lightSource.frag");
    Shader lightingShader("shaders/lighting.vert", "shaders/lighting.frag");
    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Grid grid;

    Model sphere("resources/models/sphere/sphere.obj");
    Model woodenBox("resources/models/Wooden Box/woodenBox.obj");
    Model woodenFloor("resources/models/Wooden Floor/woodenFloor.obj");

    Model temple("resources/models/Temple/Temple.fbx");
    unsigned int templeDiffuse = loadTextureFromDisk("resources/models/Temple/Temple_Marble_BaseColor.png");
    unsigned int templeSpecular = loadTextureFromDisk("resources/models/Temple/Temple_Marble_Metallic.png");

    lightingShader.use();
    lightingShader.setFloat("material.shininess", 8.f);

    lightSourceShader.use();
    glm::vec3 lightColor = glm::vec3(1.f, 1.f, 1.f);
    lightSourceShader.setVec3("light.color", lightColor);

    glm::vec3 lightPosition = glm::vec3(0.f, 6.f, 0.f);
    PointLight pLightOne(lightingShader);
    pLightOne.setPosition(lightPosition);
    pLightOne.setAmbient(glm::vec3(0.01f, 0.01f, 0.01f));

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

        glfwPollEvents();
        if (glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window,GLFW_KEY_X))
            glfwSetWindowShouldClose(window, true);

        camera.update();
        view = camera.getViewMatrix();
        cameraPosition = camera.getCameraPosition();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear default buffer.

        msFramebuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear multisampled buffer.

        lightingShader.use();
        lightingShader.setVec3("viewPosition", cameraPosition);

        glm::mat4 model = glm::mat4(1.f);
        model = glm::translate(model , glm::vec3(0.f, 4.2f, 0.f));
        model = glm::rotate(model, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        lightingShader.setAllMat4(model, view, projection);
        lightingShader.setInt("material.diffuse0", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, templeDiffuse);
        lightingShader.setInt("material.specular0", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, templeSpecular);
        temple.draw(lightingShader);

        model = glm::mat4(1.f);
        model = glm::scale(model, glm::vec3(40.f, 1.f, 40.f));
        lightingShader.setAllMat4(model, view, projection);
        woodenFloor.draw(lightingShader);

        lightSourceShader.use();
        model = glm::mat4(1.f);
        model = glm::translate(model, lightPosition);
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        lightSourceShader.setAllMat4(model, view, projection);
        sphere.draw(lightSourceShader);

        msFramebuffer.unbind();
        
        glBindFramebuffer(GL_READ_FRAMEBUFFER, msFramebuffer.getFramebufferObjectId());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        //------------------SWAP BUFFERS AND RENDER GUI------------------
        glfwSwapBuffers(window);
        counter++;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}