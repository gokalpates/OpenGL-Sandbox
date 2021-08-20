#include "Camera.h"

#include <iostream>

extern float deltaTime;

Camera::Camera(GLFWwindow* window) :
	windowWidth(0),
	windowHeight(0),
	cameraPosition(0.f, 0.f, 0.f),
	cameraFront(0.f, 0.f, -1.f),
	cameraUp(0.f, 1.f, 0.f),
	cameraSpeed(2.5f),
	mouseXPosition(0.f),
	mouseYPosition(0.f),
	mouseLastXPosition(0.f),
	mouseLastYPosition(0.f),
	sensivity(0.1f),
	pitch(0.f),
	yaw(0.f),
	firstTouch(true),
	cameraWindow(window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	mouseLastXPosition = (double)windowWidth / 2.0;
	mouseLastYPosition = (double)windowHeight / 2.0;
};

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
}

glm::vec3 Camera::getCameraPosition() const
{
	return cameraPosition;
}

glm::vec3 Camera::getCameraFront() const
{
	return cameraFront;
}

void Camera::update()
{
	processMouse(cameraWindow);
	processKeyboard(cameraWindow);
}

void Camera::setCameraPosition(glm::vec3 position)
{
	cameraPosition = position;
}

void Camera::setCameraSpeed(float value)
{
	cameraSpeed = value;
}

void Camera::setSensivity(float value)
{
	sensivity = value;
}

void Camera::setFirstTouch(bool value)
{
	firstTouch = value;
}

void Camera::processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPosition += cameraFront * (cameraSpeed * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPosition -= cameraFront * (cameraSpeed * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (cameraSpeed * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * (cameraSpeed * deltaTime);
}

void Camera::processMouse(GLFWwindow* window)
{
	glfwGetCursorPos(window, &mouseXPosition, &mouseYPosition);
	if ((mouseXPosition != mouseLastXPosition) || (mouseYPosition != mouseLastYPosition))
	{
		if (firstTouch)
		{
			mouseLastXPosition = mouseXPosition;
			mouseLastYPosition = mouseYPosition;
			firstTouch = false;
		}
		double xOffset = mouseXPosition - mouseLastXPosition;
		double yOffset = mouseLastYPosition - mouseYPosition; // Reversed because y coordinates goes top to bottom.
		mouseLastXPosition = mouseXPosition;
		mouseLastYPosition = mouseYPosition;

		xOffset *= sensivity;
		yOffset *= sensivity;

		yaw += xOffset;
		pitch += yOffset;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}
}
