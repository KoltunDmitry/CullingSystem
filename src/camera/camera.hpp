#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>

#include <memory>

enum Direction
{
	NO_MOVEMENT = 0,
	UP = 1 << 0,
	DOWN = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3,
	UP_ROTATION = 1 << 4,
	DOWN_ROTATION = 1 << 5,
	LEFT_ROTATION = 1 << 6,
	RIGHT_ROTATION = 1 << 7
};

class Camera {
public:
	Camera(GLFWwindow* window);
	std::shared_ptr<glm::mat4> getViewProjection() const { return m_viewProjection; }
	bool handleKeyEvent();
	float getDeltaHorizontalRotation() const { return m_deltaHorizontalRotation; }
	float& speedCamera() { return m_speedCamera; }
	float getFovy() const { return m_fovy; }
	float getAspect() const { return m_aspect; }

	glm::vec3 getPosition() const { return m_position; }
	int getDirection() const { return m_direction; }

	void tick(float dTime = 0.0f);
    void fakeTick(float dTime = 0.0f);
	void onChanged();
private:
	std::shared_ptr<glm::mat4> m_viewProjection;
	glm::vec3 m_position;
	glm::vec3 m_lookAt;
	glm::vec3 m_head;
	float m_horizontalAngle;
	float m_verticalAngle;
	float m_deltaHorizontalRotation;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;
	float m_speedCamera;
	GLFWwindow* m_window;
	int m_direction;

	float m_fovy = 60.f;
	float m_aspect = 4 / 3;

};