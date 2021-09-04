#include <iostream>
#include <string>
#include <chrono>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "FramebufferObject.h"
#include "VertexArrayObject.h"
#include "VertexBufferObject.h"
#include "Callback.h"
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "Grid.h"
#include "Debug.h"
#include "Skybox.h"

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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glEnable(GL_DEPTH_TEST);

    Camera camera(window);
    camera.setCameraSpeed(15.f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.f), (float)screenWidth / (float)screenHeight, 0.1f, 100.f);

    std::vector<std::string> skyboxLocations = {
        "resources/skybox/right.jpg",
        "resources/skybox/left.jpg",
        "resources/skybox/top.jpg",
        "resources/skybox/bottom.jpg",
        "resources/skybox/front.jpg",
        "resources/skybox/back.jpg" //If output is wrong then swap last 2.
    };
    Skybox sky(skyboxLocations, &camera, &projection);

    Shader gridShader("shaders/grid.vert", "shaders/grid.frag");
    Shader modelShader("shaders/model.vert", "shaders/model.frag");

    Model woodenBox("resources/models/Wooden Box/woodenBox.obj");

    Grid grid;

    std::vector<float> vertices = {
            0.f,0.f,0.f,0.f,0.f,0.f,0.f,0.f, //Bottom left
            1.f,0.f,0.f,0.f,0.f,0.f,1.f,0.f, //Bottom right
            0.f,1.f,0.f,0.f,0.f,0.f,0.f,1.f, //Top left

            1.f,0.f,0.f,0.f,0.f,0.f,1.f,0.f, //Top left
            0.f,1.f,0.f,0.f,0.f,0.f,0.f,1.f, //Bottom right
            1.f,1.f,0.f,0.f,0.f,0.f,1.f,1.f //Top right
    };

    GLuint vaoOne;
    glGenVertexArrays(1, &vaoOne);
    glBindVertexArray(vaoOne);
    
    GLuint vboOne;
    glGenBuffers(1, &vboOne);
    glBindBuffer(GL_ARRAY_BUFFER, vboOne);

    //Allocating memory on vram with specific size. NOT BUFFERING DATA
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 0, GL_DYNAMIC_DRAW);

    //Getting pointer from allocated memory.
    void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
    if (ptr != nullptr)
    {
        //Copying data to given address.
        std::memcpy(ptr, vertices.data(), sizeof(float) * vertices.size());
        std::cout << std::hex << "Buffered data to: " << ptr << std::endl;
    }
    //Release mapping on address space.
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Create a second buffer to copy the data into.
    GLuint vboTwo;
    glGenBuffers(1, &vboTwo);
    glBindBuffer(GL_ARRAY_BUFFER, vboTwo);

    glBindBuffer(GL_COPY_READ_BUFFER, vboOne);
    glBindBuffer(GL_COPY_WRITE_BUFFER, vboTwo);

    //Get data size in bytes.
    GLint dataSize = 0;
    glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &dataSize);

    std::cout << std::dec << dataSize << " bytes will be copied to new buffer in vram." << std::endl;

    //Allocate a memory on vram in specified bytes.
    glBufferData(GL_COPY_WRITE_BUFFER, dataSize, 0, GL_STATIC_DRAW);

    //Copying data from read target to write target.
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, dataSize);

    GLuint vaoTwo;
    glGenVertexArrays(1, &vaoTwo);
    glBindVertexArray(vaoTwo);

    glBindBuffer(GL_ARRAY_BUFFER, vboTwo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    stbi_set_flip_vertically_on_load(true);
    GLuint grassTexture = loadTextureFromDisk("resources/grass.png");
    Shader vegetableShader("shaders/vegetable.vert", "shaders/vegetable.frag");

    int keyCount = 0;
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
        }

        glfwPollEvents();
        if (glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) && glfwGetKey(window,GLFW_KEY_X))
            glfwSetWindowShouldClose(window, true);

        if ((glfwGetKey(window,GLFW_KEY_E) == GLFW_PRESS) && (keyCount == 0))
        {
            glBindBuffer(GL_ARRAY_BUFFER, vboOne);
            void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
            std::memset(ptr, 0, sizeof(float) * vertices.size());
            glUnmapBuffer(GL_ARRAY_BUFFER);

            std::cout << "Deleted data at: " << ptr << std::endl;
            keyCount++;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("OpenGL Renderer");
        ImGui::Text("FPS: %d", fpsToShow);
        ImGui::End();

        camera.update();
        view = camera.getViewMatrix();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //DRAW WOODEN BOX.
        modelShader.use();

        glm::mat4 model = glm::mat4(1.f);
        modelShader.setAllMat4(model, view, projection);

        woodenBox.draw(modelShader);

        //DRAW VEGETABLE.
        vegetableShader.use();
        
        model = glm::mat4(1.f);
        model = glm::translate(model, glm::vec3(3.f, 0.f, 0.f));
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.f, 1.f, 0.f));
        vegetableShader.setAllMat4(model, view, projection);

        glBindVertexArray(vaoTwo);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        vegetableShader.setInt("texture_diffuse", 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        //DRAW SKYBOX.
        sky.draw();

        //------------------SWAP BUFFERS AND RENDER GUI------------------
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        counter++;
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}