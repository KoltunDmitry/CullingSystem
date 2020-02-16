#pragma once

#include "bounding_figures.hpp"
#include "drawable.hpp"

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>

#include <utility>
#include <memory>
#include <vector>

static const int COUNT_VERTEX_CUBE = 36 * 3;

class Cube : public Drawable {
public:
	Cube(std::shared_ptr<glm::mat4> viewProjection, GLuint programID);
	void setWorldTransform(glm::mat4 matrix) override;
	void draw(float dt = 0.0f) override;
	AABB getAABB() const override { return m_aabb; }
	Sphere getSphere() const override { return m_sphere; }
private:
	void computeAABB();
	void computeSphere();
private:
	GLuint m_programID;
	GLuint m_matrixID;

	GLfloat m_vertexBufferData[COUNT_VERTEX_CUBE];
	GLfloat m_vertexColorData[COUNT_VERTEX_CUBE];
	GLuint m_vertexBuffer;
	GLuint m_colorBuffer;

	std::shared_ptr<glm::mat4> m_viewProjection;
	glm::mat4 m_worldTransform;
	AABB m_aabb;
	Sphere m_sphere;
};