#include "camera.hpp"
#include <glm\gtc\matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#define PI 3.14159265359

Camera::Camera(GLFWwindow* window) 
	: m_window(window)
	, m_position(glm::vec3(0, 5, 0))
	, m_lookAt(glm::vec3(0,0,0))
	, m_head(glm::vec3(0,1,0))
	, m_speedCamera(0.1f)
	, m_horizontalAngle(0)
	, m_verticalAngle(PI / 2)
	, m_deltaHorizontalRotation(0.0f)
{
	m_projectionMatrix = glm::perspective(glm::radians(m_fovy), m_aspect, 0.5f, 1000.f);
	m_viewMatrix = glm::lookAt(
		m_position,
		m_lookAt,
		m_head
	);
	glm::mat4 MVP = m_projectionMatrix * m_viewMatrix;
	m_viewProjection = std::make_shared<glm::mat4>(MVP);
}

bool Camera::handleKeyEvent()
{
	return false;
}

void Camera::tick(float dTime)
{
	glm::mat4 model(1.0f);
	float coefRotation = 0.01;
	m_direction = NO_MOVEMENT;
	m_deltaHorizontalRotation = 0.0f;
	if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_horizontalAngle += coefRotation * m_speedCamera * dTime;
		m_deltaHorizontalRotation = coefRotation * m_speedCamera * dTime;
		m_direction |= LEFT_ROTATION;
	}
	if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_horizontalAngle -= coefRotation * m_speedCamera * dTime;
		m_deltaHorizontalRotation = coefRotation * m_speedCamera * dTime;
		m_direction |= RIGHT_ROTATION;
	}
	if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		m_verticalAngle += coefRotation * m_speedCamera * dTime;
		m_direction |= UP_ROTATION;
	}
	if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		m_verticalAngle -= coefRotation * m_speedCamera * dTime;
		m_direction |= DOWN_ROTATION;
	}
	// TODO::add left and right

	glm::vec3 direction = glm::vec3(
		sin(m_verticalAngle) * sin(m_horizontalAngle),
		cos(m_verticalAngle),
		sin(m_verticalAngle) * cos(m_horizontalAngle));

	glm::vec3 right = glm::vec3(
		sin(m_horizontalAngle + PI / 2.0f),
		0,
		cos(m_horizontalAngle + PI / 2.0f)
	);

	m_head = glm::cross(direction, right);

	if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_position += direction * m_speedCamera * dTime;
		m_direction |= UP;
	}
	if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_position -= direction * m_speedCamera * dTime;
		m_direction |= DOWN;
	}


	m_lookAt = m_position + direction;
	onChanged();
}

void Camera::fakeTick(float dTime /*= 0.0f*/) {
	dTime /= 10.f;
	glm::vec3 direction = glm::vec3(0, 0, 1);
	m_position += direction * m_speedCamera * dTime;
	m_direction |= UP;
	m_lookAt = m_position + direction;
	onChanged();
}

void Camera::onChanged()
{
	m_projectionMatrix = glm::perspective(glm::radians(m_fovy), m_aspect, 0.1f, 1000.f);
	m_viewMatrix = glm::lookAt(
		m_position,
		m_lookAt,
		m_head
	);

	*m_viewProjection.get() = m_projectionMatrix * m_viewMatrix;
}
