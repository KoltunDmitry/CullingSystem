#include "frustum.hpp"
#include <limits>
#include <unordered_map>
#include <iostream>

#define PI 3.1415

static const std::unordered_map<PLANE, PLANE> REVERSE_PLANE =
{
	{PLANE::FAR, PLANE::NEAR}, {PLANE::NEAR, PLANE::FAR}, {PLANE::LEFT, PLANE::RIGHT}, {PLANE::RIGHT, PLANE::LEFT}
};

Frustum::Frustum(int count) 
	: m_culledByPlane(count, CullingPlane(PLANE::INTERSECTION,0.f))
	, m_isIntersect(count, true)
	, m_noMovementOptimisation(false)
	, m_translationCoherency(false)
	, m_rotationCoherency(false)
{
}

void Frustum::recalculateFrustumView(Camera& camera, int newDirection)
{
	m_movementInfo.reset();
	m_needRecalculateRotationCoherency.reset();
	m_translationMovement.reset();
	m_angleLeftRightPlanes = camera.getAspect() * camera.getFovy();

	m_movementInfo.isFrontMovement = newDirection == UP || newDirection == DOWN;
	m_movementInfo.isSideMovement = newDirection == LEFT || newDirection == RIGHT;

	if (m_movementInfo.isFrontMovement)
	{
		m_movementInfo.plane = newDirection == UP ? PLANE::FAR : PLANE::NEAR;
	}
	else if(m_movementInfo.isSideMovement)
	{
		m_movementInfo.plane = newDirection == LEFT ? PLANE::LEFT : PLANE::RIGHT;
	}

	auto resetRotation = [&]()
	{
		m_needRecalculateRotationCoherency = m_direction == LEFT_ROTATION ? 0 : 1;
		m_counterRotation = 0;
	};

	if ((newDirection == LEFT_ROTATION || newDirection == RIGHT_ROTATION))
	{
		m_counterRotation += abs(camera.getDeltaHorizontalRotation());
		if (m_counterRotation >= PI - m_angleLeftRightPlanes)
		{
			resetRotation();
		}
		if(newDirection == m_direction)
		{
			resetRotation();
		}
	}
	else if (m_direction == LEFT_ROTATION || m_direction == RIGHT_ROTATION)
	{
		resetRotation();
	}

	m_direction = newDirection;

	m_rotationCullingPlane = m_direction == LEFT_ROTATION ? PLANE::RIGHT : RIGHT_ROTATION ? PLANE::LEFT : PLANE::INTERSECTION;
	
	FrustumView prev = m_frustumView;

	/*if(UP movement)
	{
		DOWN object not draw
		UP object recalculate distance;
	}*/
	
	glm::mat4 mat = *camera.getViewProjection().get();
	for (int i = 4; i--; ) m_frustumView.planes[0][i] = mat[i][3] + mat[i][0];
	for (int i = 4; i--; ) m_frustumView.planes[1][i] = mat[i][3] - mat[i][0];
	for (int i = 4; i--; ) m_frustumView.planes[2][i] = mat[i][3] + mat[i][1];
	for (int i = 4; i--; ) m_frustumView.planes[3][i] = mat[i][3] - mat[i][1];
	for (int i = 4; i--; ) m_frustumView.planes[4][i] = mat[i][3] + mat[i][2];
	for (int i = 4; i--; ) m_frustumView.planes[5][i] = mat[i][3] - mat[i][2];

	for(int i = 0; i < COUNT_PLANE; i++)
	{
		glm::normalize(m_frustumView.planes[i]);
	}

	//calculate passedDistanceByPlane
	if(m_movementInfo.isFrontMovement)
	{
		m_movementInfo.passedDistanceByPlane = distance(m_frustumView.planes[static_cast<size_t>(PLANE::NEAR)],
			prev.planes[static_cast<size_t>(PLANE::NEAR)]);
#ifdef DEBUG
		std::cout << "Front movement: " << m_movementInfo.passedDistanceByPlane << std::endl;
#endif
	}
	else if(m_movementInfo.isSideMovement)
	{
		m_movementInfo.passedDistanceByPlane = distance(m_frustumView.planes[static_cast<size_t>(PLANE::NEAR)],
			prev.planes[static_cast<size_t>(PLANE::LEFT)]);
#ifdef DEBUG
		std::cout << "Side movement: " << m_movementInfo.passedDistanceByPlane << std::endl;
#endif
	}
}

void Frustum::setNoMovementOptimisation(bool optimisation)
{
	m_noMovementOptimisation = optimisation;
}

void Frustum::setRotationCoherency(bool optimization)
{
	m_rotationCoherency = optimization;
}

void Frustum::setTranslationCoherency(bool optimization)
{
	m_translationCoherency = optimization;
}

bool Frustum::isIntersectPoint(const Point& point, int id)
{
	return true;
}

bool Frustum::isIntersectSphere(const Sphere& sphere, int id)
{
	int startIndex = id >= 0 ? static_cast<int>(m_culledByPlane[id].index) : 0;
	
	if(m_rotationCoherency && outsideFrustumRotationCoherence(id))
	{
		return false;
	}
	if (m_translationCoherency && outsideFrustumTranslationCoherence(startIndex, id))
	{
		return false;
	}

	for (int i = startIndex; i < startIndex + COUNT_PLANE; i++)
	{
		int indexPlane = i % 6;
		if (float d = distance(sphere.center, m_frustumView.planes[indexPlane]); d < -sphere.radius)
		{
			if (id >= 0)
			{
				m_culledByPlane[id].index = static_cast<PLANE>(indexPlane);
				m_culledByPlane[id].distance = abs(d);
				m_isIntersect[id] = false;
			}
			return false;
		}
	}
	if (id >= 0)
	{
		m_culledByPlane[id].index = PLANE::INTERSECTION;
		m_culledByPlane[id].distance = 0.f;
		m_isIntersect[id] = true;
	}

	return true;
}

bool Frustum::isIntersectAABBv1(const AABB& aabb, int id)
{
	std::array<glm::vec3, 8> vertices = aabb.getVertices();
	int startIndex = id >= 0 ? static_cast<int>(m_culledByPlane[id].index) : 0;

	if (m_rotationCoherency && outsideFrustumRotationCoherence(id))
	{
		return false;
	}
	if (m_translationCoherency && outsideFrustumTranslationCoherence(startIndex, id))
	{
		return false;
	}

	float min = 0.0f;
	for (int i = startIndex; i < startIndex + COUNT_PLANE; i++)
	{
		int indexPlane = i % 6;
		int in = 0;
		int out = 0;
		for(int j = 0; j < 8; j++)
		{
			// can find min of distances
			if(float d = distance(vertices[j], m_frustumView.planes[indexPlane]); d < 0)
			{
				min = std::min(abs(d), min);
				out++;
			}
			else
			{
				in++;
				break;
			}
		}
		if(!in)
		{
			m_culledByPlane[id].index = static_cast<PLANE>(indexPlane);
			m_culledByPlane[id].distance = min;
			m_isIntersect[id] = false;
			return m_isIntersect[id];
		}
	}
	m_culledByPlane[id].index = PLANE::INTERSECTION;
	m_isIntersect[id] = true;
	return m_isIntersect[id];
}

bool Frustum::isIntersectAABBv2(const AABB& aabb, int id)
{
	int startIndex = id >= 0 ? static_cast<int>(m_culledByPlane[id].index) : 0;
	if (m_rotationCoherency && outsideFrustumRotationCoherence(id))
	{
		return false;
	}
	if (m_translationCoherency && outsideFrustumTranslationCoherence(startIndex, id))
	{
		return false;
	}
	for(int i = startIndex; i < startIndex + COUNT_PLANE; i++)
	{
		int indexPlane = i % 6;
		if(float d = distance(getPositiveVertex(aabb, m_frustumView.planes[indexPlane]), m_frustumView.planes[indexPlane]); d < 0)
		{
			m_culledByPlane[id].index = static_cast<PLANE>(indexPlane);
			m_culledByPlane[id].distance = abs(d);
			m_isIntersect[id] = false;
			return m_isIntersect[id];
		}
	}
	m_culledByPlane[id].index = PLANE::INTERSECTION;
	m_isIntersect[id] = true;
	return m_isIntersect[id];

}

bool Frustum::isSimpleIntersectPoint(const Point& point)
{
	for (int i = 0; i < 6; i++)
	{
		if (distance(point.point, m_frustumView.planes[i]) < 0)
		{
			return false;
		}
	}
	return true;
}

bool Frustum::isSimpleIntersectSphere(const Sphere& sphere)
{
	for (int i = 0; i < 6; i++)
	{
		if (distance(sphere.center, m_frustumView.planes[i]) < -sphere.radius)
		{
			return false;
		}
	}
	return true;
}

bool Frustum::isSimpleIntersectAABBv1(const AABB& aabb)
{
	std::array<glm::vec3, 8> vertices = aabb.getVertices();

	for (int i = 5; i >= 0; i--)
	{
		int in = 0;
		int out = 0;
		for (int j = 0; j < 8; j++)
		{
			if (distance(vertices[j], m_frustumView.planes[i]) < 0)
			{
				out++;
			}
			else
			{
				in++;
				break;
			}
		}
		if (!in)
		{
			return false;
		}
	}
	return true;
}

bool Frustum::isSimpleIntersectAABBv2(const AABB& aabb)
{
	for (int i = 0; i < 6; i++)
	{
		if (distance(getPositiveVertex(aabb, m_frustumView.planes[i]), m_frustumView.planes[i]) < 0)
		{
			return false;
		}
	}
	return true;
}

float Frustum::distance(const glm::vec3& point, const glm::vec4 plane)
{
	return (point.x * plane[0] + point.y * plane[1] + point.z * plane[2] + plane[3]) / sqrt(plane[0]*plane[0] + plane[1]*plane[1] + plane[2]*plane[2]); 
}

float Frustum::distance(glm::vec4 plane1, glm::vec4 plane2)
{
	// need that a1 == a2, b1 == b2, c1 == c2
	// https://onlinemschool.com/math/library/analytic_geometry/plane_plane/
	auto isAlmostEqual = [](float a, float b)
	{
		static constexpr float eps = std::numeric_limits<float>().epsilon();
		if (abs(a - b) < eps)
		{
			return true;
		}
		return false;
	};

	float k = 0.f;
	if(!isAlmostEqual(plane2.x, 0.f))
	{
		k = plane1.x / plane2.x;
	}
	else if(!isAlmostEqual(plane2.y, 0.f))
	{
		k = plane1.y / plane2.y;
	}
	else if(!isAlmostEqual(plane2.z, 0.f))
	{
		k = plane1.z / plane2.z;
	}
	else
	{
		assert(false, "it is not plane");
	}
	plane2 *= k;
	return abs(plane2.w - plane1.w) / sqrt(plane1.x * plane1.x + plane1.y * plane1.y + plane1.z * plane1.z);
}

PLANE Frustum::planeByIndex(int index)
{
	return static_cast<PLANE>(index);
}

bool Frustum::isFrontPlane(int index)
{
	return index == static_cast<int>(PLANE::NEAR) || index == static_cast<int>(PLANE::FAR);
}

bool Frustum::isSidePlane(int index)
{
	return index == static_cast<int>(PLANE::LEFT) || index == static_cast<int>(PLANE::RIGHT);
}

glm::vec3 Frustum::getPositiveVertex(const AABB& aabb, const glm::vec4& normal)
{
	glm::vec3 positive = aabb.min;
	if (normal.x >= 0)
		positive.x = aabb.max.x;
	if (normal.y >= 0)
		positive.y = aabb.max.y;
	if (normal.z >= 0)
		positive.z = aabb.max.z;
	return positive;
}

bool Frustum::outsideFrustumTranslationCoherence(int plane, int id)
{
	if (m_movementInfo.plane != PLANE::INTERSECTION && (isFrontPlane(plane) || isSidePlane(plane)))
	{
		if (REVERSE_PLANE.at(planeByIndex(plane)) == m_movementInfo.plane)
		{
			float currentDistance = m_culledByPlane[id].distance + m_movementInfo.passedDistanceByPlane;
			return true;
		}
		else if (planeByIndex(plane) == m_movementInfo.plane)
		{
			float currentDistance = m_culledByPlane[id].distance - m_movementInfo.passedDistanceByPlane;
			if (currentDistance > 0.0f)
			{
				m_culledByPlane[id].distance = currentDistance;
				return true;
			}
		}
	}
	return false;
}

bool Frustum::outsideFrustumRotationCoherence(int id)
{
	if ((m_direction == LEFT_ROTATION || m_direction == RIGHT_ROTATION)
		&& !m_needRecalculateRotationCoherency.has_value() && m_culledByPlane[id].index == m_rotationCullingPlane)
	{
		return true;
	}
	return false;
}

void Frustum::onChangedTypeOfCulling()
{
	for(auto& item : m_culledByPlane)
	{
		item.distance = 0.f;
		item.index = PLANE::INTERSECTION;
	}
}
