#pragma once

#include "figures/bounding_figures.hpp"
#include "figures/drawable.hpp"
#include "camera/camera.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <optional>

static const size_t COUNT_PLANE = 6;

struct FrustumView
{
	glm::vec4 planes[COUNT_PLANE];
	//0 - left, 1 - right, 2 - bottom, 3 - top, 4 - near, 5 - far
};

enum class PLANE
{
	LEFT = 0,
	RIGHT = 1,
	BOTTOM = 2,
	TOP = 3,
	NEAR = 4,
	FAR = 5,
	INTERSECTION = 6
};

struct MovementInfo
{
	bool isSideMovement;
	bool isFrontMovement;
	bool isNoMovement;
	PLANE plane;
	// is valid if isSideMovement or isFrontMovement;
	float passedDistanceByPlane;
	void reset()
	{
		isNoMovement = false;
		isSideMovement = false;
		isFrontMovement = false;
		passedDistanceByPlane = 0.0f;
		plane = PLANE::INTERSECTION;
	}
};

struct CullingPlane
{
	CullingPlane(PLANE index, float dist) : index(index), distance(dist) {}
	PLANE index;
	// should be positive
	float distance;
};

class Frustum
{
public:
	Frustum(int count);
	void recalculateFrustumView(Camera& camera, int direction);
	void setNoMovementOptimisation(bool optimization);
	void setRotationCoherency(bool optimization);
	void setTranslationCoherency(bool optimization);

	template<typename T>
	bool isIntersectPoint(T& drawable, bool useCaching);

	template<typename T>
	bool isIntersectSphere(T& drawable, bool useCaching);

	template<typename T>
	bool isIntersectAABBv1(T& drawable, bool useCaching);

	template<typename T>
	bool isIntersectAABBv2(T& drawable, bool useCaching);

	bool isSimpleIntersectPoint(const Point& point);
	bool isSimpleIntersectSphere(const Sphere& sphere);
	bool isSimpleIntersectAABBv1(const AABB& aabb);
	bool isSimpleIntersectAABBv2(const AABB& aabb);

	bool isIntersectPoint(const Point& point, int id = -1);
	bool isIntersectSphere(const Sphere& sphere, int id = -1);
	bool isIntersectAABBv1(const AABB& aabb, int id = -1);
	bool isIntersectAABBv2(const AABB& aabb, int id = -1);

	void onChangedTypeOfCulling();
private:
	float distance(const glm::vec3& point, const glm::vec4 plane);
	float distance(glm::vec4 plane1, glm::vec4 plane2);
	PLANE planeByIndex(int index);
	bool isFrontPlane(int index);
	bool isSidePlane(int index);
	glm::vec3 getPositiveVertex(const AABB& aabb, const glm::vec4& normal);

	bool outsideFrustumTranslationCoherence(int plane, int id);
	bool outsideFrustumRotationCoherence(int id);

	// [index] = plane, INTERSECTION if not culled
	std::vector<CullingPlane> m_culledByPlane;
	std::vector<bool> m_isIntersect;
	std::optional<int> m_needRecalculateRotationCoherency;
	std::optional<float> m_translationMovement;
	FrustumView m_frustumView;
	int m_directionPlane;
	int m_direction;
	MovementInfo m_movementInfo;
	PLANE m_rotationCullingPlane;
	float m_counterRotation;
	float m_angleLeftRightPlanes;
	//----- Bool for optimization -----
	bool m_noMovementOptimisation;
	bool m_rotationCoherency;
	bool m_translationCoherency;
};


template<typename T>
bool Frustum::isIntersectAABBv1(T& drawable, bool useCaching)
{
	return isIntersectAABBv1(drawable.getAABB(), useCaching ? drawable.getID() : -1);
}

template<typename T>
bool Frustum::isIntersectAABBv2(T& drawable, bool useCaching)
{
	return isIntersectAABBv2(drawable.getAABB(), useCaching ? drawable.getID() : -1);
}

template<typename T>
bool Frustum::isIntersectPoint(T& drawable, bool useCaching)
{
	return isIntersectPoint(drawable.getPoint(), useCaching ? drawable.getID() : -1);
}

template<typename T>
bool Frustum::isIntersectSphere(T& drawable, bool useCaching)
{
	return isIntersectSphere(drawable.getSphere(), useCaching ? drawable.getID() : -1);
}
