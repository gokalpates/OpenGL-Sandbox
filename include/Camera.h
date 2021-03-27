#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
	Camera(GLFWwindow* window);

	glm::mat4 getViewMatrix() const;
	void update();

	void setCameraSpeed(float value = 0.002f);
	void setSensivity(float value = 0.1f);

private:
	int windowWidth, windowHeight;
	glm::vec3 cameraPosition, cameraFront, cameraUp;
	float cameraSpeed;
	double mouseXPosition, mouseYPosition;
	double mouseLastXPosition, mouseLastYPosition;
	float sensivity;
	float pitch, yaw;
	GLFWwindow* cameraWindow;

	void processKeyboard(GLFWwindow* window);
	void processMouse(GLFWwindow* window);
};

