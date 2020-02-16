#include "bounding_figures.hpp"

std::array<glm::vec3, 8> AABB::getVertices() const
{
	std::array<glm::vec3, 8> arr;
	arr[0] = min;
	arr[1] = max;
	arr[2] = glm::vec3(min.x, min.y, max.z);
	arr[3] = glm::vec3(min.x, max.y, min.z);
	arr[4] = glm::vec3(max.x, min.y, min.z);
	arr[5] = glm::vec3(min.x, max.y, max.z);
	arr[6] = glm::vec3(max.x, min.y, max.z);
	arr[7] = glm::vec3(max.x, max.y, min.z);
	return arr;
}
