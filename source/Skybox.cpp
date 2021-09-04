#include "Skybox.h"

std::vector<float> Skybox::cubemapVertices = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
};

Skybox::Skybox(std::vector<std::string>& resources, Camera* mainCam, glm::mat4* projection):
    camera(mainCam),
    projectionMatrix(projection)
{
    //CREATING SHADER.
    skyboxShader = new Shader("shaders/skybox.vert", "shaders/skybox.frag");
    
    //CREATING BUFFERS.
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glBufferData(GL_ARRAY_BUFFER, cubemapVertices.size() * sizeof(float), cubemapVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //CREATE CUBEMAP TEXTURE.
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    //LOAD TEXTURES.
    unsigned char* data;
    int width, height, nrOfChannels;
    for (size_t i = 0; i < resources.size(); i++)
    {
        data = stbi_load(resources.at(i).c_str(), &width, &height, &nrOfChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            printf("ERROR: Failed to load cubemap texture: %s.\n", resources.at(i).c_str());
            glfwTerminate();
            std::exit(EXIT_FAILURE);
        }
    }

    //SET TEXTURE PARAMETERS
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Skybox::~Skybox()
{
    skyboxShader->~Shader();
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteTextures(1, &cubemapTexture);
    cubemapVertices.empty();
}

void Skybox::draw() const
{
    glDepthFunc(GL_LEQUAL);
    skyboxShader->use();

    glm::mat4 viewMatrix = camera->getViewMatrix();
    viewMatrix = glm::mat4(glm::mat3(viewMatrix)); //Removing translation.

    skyboxShader->setMat4("projection", *projectionMatrix);
    skyboxShader->setMat4("view", viewMatrix);

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    skyboxShader->setInt("skybox", 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
}
