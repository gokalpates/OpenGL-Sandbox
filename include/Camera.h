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
	glm::vec3 getCameraPosition() const;
	glm::vec3 getCameraFront() const;
	void update();

	void setCameraPosition(glm::vec3 position);
	void setCameraSpeed(float value = 0.002f);
	void setSensivity(float value = 0.1f);
	void setFirstTouch(bool value = true);

private:
	int windowWidth, windowHeight;
	glm::vec3 cameraPosition, cameraFront, cameraUp;
	float cameraSpeed;
	double mouseXPosition, mouseYPosition;
	double mouseLastXPosition, mouseLastYPosition;
	float sensivity;
	float pitch, yaw;
	bool firstTouch;
	GLFWwindow* cameraWindow;

	void processKeyboard(GLFWwindow* window);
	void processMouse(GLFWwindow* window);
};

