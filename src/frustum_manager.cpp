#include "frustum_manager.hpp"
//#include "bounding_volume_hierarchy.hpp"

FrustumManager::FrustumManager(const std::vector<Drawable*>& objects, int optimizations)
	: m_objects(objects)
	, m_frustum(m_objects.size())
	, m_optimizations(optimizations)
	, m_bvh(BoundingVolumeHierarchy(m_objects))

{
	// off all optimizations
}

void FrustumManager::recalculateFrustumView(Camera& camera, int direction)
{
	m_frustum.recalculateFrustumView(camera, direction);
}

void FrustumManager::updateOptimizations(int optimizations)
{
	if(m_optimizations == optimizations)
	{
		return;
	}
	int difference = m_optimizations ^ optimizations;
	m_optimizations = optimizations;
	/*if (difference & static_cast<int>(Optimization::BVH))
	{
		setBVH(m_optimizations & static_cast<int>(Optimization::BVH));
	}*/
	/*if (difference & static_cast<int>(Optimization::AABB_POSITIVE_CULLING))
	{
		setAABBPositiveCulling(m_optimizations & static_cast<int>(Optimization::AABB_POSITIVE_CULLING));
	}
	if (difference & static_cast<int>(Optimization::SPHERE_CULLING))
	{
		setSphereCulling(m_optimizations & static_cast<int>(Optimization::SPHERE_CULLING));
	}
	if (difference & static_cast<int>(Optimization::PLANE_COHERENCY))
	{
		setPlaneCoherency(m_optimizations & static_cast<int>(Optimization::PLANE_COHERENCY));
	*/
	if(difference & static_cast<int>(Optimization::AABB_SIMPLE_CULLING) ||
		difference & static_cast<int>(Optimization::AABB_POSITIVE_CULLING) ||
		difference & static_cast<int>(Optimization::SPHERE_CULLING))
	{
		m_frustum.onChangedTypeOfCulling();
	}
	
	if (difference & static_cast<int>(Optimization::ROTATION_COHERENCY))
	{
		m_frustum.setRotationCoherency(m_optimizations & static_cast<int>(Optimization::ROTATION_COHERENCY));
	}
	if (difference & static_cast<int>(Optimization::TRANSLATION_COHERENCY))
	{
		m_frustum.setRotationCoherency(m_optimizations & static_cast<int>(Optimization::TRANSLATION_COHERENCY));
	}

}

void FrustumManager::tick(float dt)
{
	m_deltaTime = dt;
	m_countDrawed = 0;
	if(m_optimizations& Optimization::BVH)
	{
		traverseBVH(m_bvh.root());
	}
	else
	{
		drawObjects(m_objects, m_deltaTime);
	}
}

void FrustumManager::drawObjects(const std::vector<Drawable*>& objects, float dt)
{
	for (Drawable* drawable : objects)
	{
		bool needDraw = false;
		if (!(m_optimizations & Optimization::AABB_SIMPLE_CULLING ||
			m_optimizations & Optimization::AABB_POSITIVE_CULLING ||
			m_optimizations & Optimization::SPHERE_CULLING))
		{
			m_countDrawed++;
			drawable->draw(dt);
		}
		if (m_optimizations & Optimization::PLANE_COHERENCY)
		{
			if (m_optimizations & Optimization::AABB_SIMPLE_CULLING)
			{
				needDraw |= m_frustum.isIntersectAABBv1(drawable->getAABB(), drawable->getID());
			}
			else if (m_optimizations & Optimization::AABB_POSITIVE_CULLING)
			{
				needDraw |= m_frustum.isIntersectAABBv2(drawable->getAABB(), drawable->getID());
			}
			else if (m_optimizations & Optimization::SPHERE_CULLING)
			{
				needDraw |= m_frustum.isIntersectSphere(drawable->getSphere(), drawable->getID());
			}
		}
		else
		{
			if (m_optimizations & Optimization::AABB_SIMPLE_CULLING)
			{
				needDraw |= m_frustum.isSimpleIntersectAABBv1(drawable->getAABB());
			}
			else if (m_optimizations & Optimization::AABB_POSITIVE_CULLING)
			{
				needDraw |= m_frustum.isSimpleIntersectAABBv2(drawable->getAABB());
			}
			else if (m_optimizations & Optimization::SPHERE_CULLING)
			{
				needDraw |= m_frustum.isSimpleIntersectSphere(drawable->getSphere());
			}
		}
		if (needDraw)
		{
			m_countDrawed++;
			drawable->draw(dt);
		}

	}
}

void FrustumManager::traverseBVH(Node* node)
{
	if(!node)
	{
		return;
	}
	if(!m_frustum.isSimpleIntersectAABBv1(node->aabb))
	{
		return;
	}
	traverseBVH(node->left.get() ? node->left.get() : nullptr);
	if(node->left == nullptr && node->right == nullptr)
	{
		drawObjects(node->objects, m_deltaTime);
	}
	traverseBVH(node->right.get() ? node->right.get() : nullptr);

}
