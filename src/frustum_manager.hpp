#pragma once
#include "figures/drawable.hpp"
#include "frustum.hpp"
#include "bounding_volume_hierarchy.hpp"
#include "camera/camera.hpp"

#include <vector>
#include <memory>

enum Optimization
{
	AABB_SIMPLE_CULLING = 1 << 0,
	AABB_POSITIVE_CULLING = 1 << 1,
	SPHERE_CULLING = 1 << 2,
	PLANE_COHERENCY = 1 << 3,
	ROTATION_COHERENCY = 1 << 4,
	TRANSLATION_COHERENCY = 1 << 5,
	BVH = 1 << 6
};


class FrustumManager
{
public:
	FrustumManager(const std::vector<Drawable*>& objects, int optimization);
	void recalculateFrustumView(Camera& camera, int direction);
	void updateOptimizations(int optimizations);
	void tick(float dt);
	int getCountDrawed() const { return m_countDrawed; };
	int getOptimizations() const { return m_optimizations; };
private:
	void drawObjects(const std::vector<Drawable*>& objects, float dt);
	void traverseBVH(Node* node);
	float m_deltaTime;
	std::vector<Drawable*>m_objects;
	Frustum m_frustum;
	BoundingVolumeHierarchy m_bvh;
	int m_optimizations;
	int m_countDrawed;
};

//void setBVH(bool on);
//void setAABBSimpleCulling(bool on);
//void setAABBPositiveCulling(bool on);
//void setSphereCulling(bool on);
//void setPlaneCoherency(bool on);