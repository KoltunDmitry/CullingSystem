#pragma once
#include <glm/glm.hpp>
#include <array>

struct Point
{
	glm::vec3 point;
};

struct Sphere
{
	glm::vec3 center;
	float radius;
};

struct AABB
{
	glm::vec3 min;
	glm::vec3 max;
	std::array<glm::vec3,8> getVertices() const;
};