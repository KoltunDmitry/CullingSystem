#include "bounding_volume_hierarchy.hpp"

#include <cassert>

Node::Node(const std::vector<Drawable*>& objects) : objects(objects)
{
	aabb = boundAABBs(objects);
}

BoundingVolumeHierarchy::BoundingVolumeHierarchy(const std::vector<Drawable*>& objects, int leafSize)
	: m_root(std::make_unique<Node>(objects))
	, m_leafSize(leafSize)
{
	build(m_root.get());
}

void BoundingVolumeHierarchy::build(Node* root)
{
	if(root->objects.size() < m_leafSize)
	{
		return;
	}
	AXIS axis = findLongestAxis(root->aabb);
	std::vector<Drawable*> objects = root->objects;
	if(axis == AXIS::X)
	{
		std::sort(objects.begin(), objects.end(), sortXAxis);
	}
	if (axis == AXIS::Y)
	{
		std::sort(objects.begin(), objects.end(), sortYAxis);
	}
	if (axis == AXIS::Z)
	{
		std::sort(objects.begin(), objects.end(), sortZAxis);
	}

	std::vector<Drawable*> left = { objects.begin(), objects.begin() + objects.size() / 2 };
	std::vector<Drawable*> right = { objects.begin() + objects.size()/2, objects.end()};
	root->left = std::make_unique<Node>(left);
	root->right = std::make_unique<Node>(right);
	build(root->left.get());
	build(root->right.get());
}


AABB boundAABBs(const std::vector<Drawable*>& objects)
{
	assert(!objects.empty(), "objects is empty!");
	AABB aabb = objects.front()->getAABB();
	for(auto& item : objects)
	{
		aabb.min.x = std::min(item->getAABB().min.x, aabb.min.x);
		aabb.min.y = std::min(item->getAABB().min.y, aabb.min.y);
		aabb.min.z = std::min(item->getAABB().min.z, aabb.min.z);
		aabb.max.x = std::max(item->getAABB().max.x, aabb.max.x);;
		aabb.max.y = std::max(item->getAABB().max.y, aabb.max.y);;
		aabb.max.z = std::max(item->getAABB().max.z, aabb.max.z);;
	}
	return aabb;
}

AXIS findLongestAxis(const AABB& aabb)
{
	float x = abs(aabb.min.x - aabb.max.x);
	float y = abs(aabb.min.y - aabb.max.y);
	float z = abs(aabb.min.z - aabb.max.z);
	if (x > y && x > z)
	{
		return AXIS::X;
	}
	if(y > x && y > z)
	{
		return AXIS::Y;
	}
	return AXIS::Z;
}

bool sortXAxis(const Drawable* a, const Drawable* b)
{
	return (a->getAABB().min.x + a->getAABB().max.x) / 2.f < (b->getAABB().min.x + b->getAABB().max.x) / 2.f;
}

bool sortYAxis(const Drawable* a, const Drawable* b)
{
	return (a->getAABB().min.y + a->getAABB().max.y) / 2.f < (b->getAABB().min.y + b->getAABB().max.y) / 2.f;
}

bool sortZAxis(const Drawable* a, const Drawable* b)
{
	return (a->getAABB().min.z + a->getAABB().max.z) / 2.f < (b->getAABB().min.z + b->getAABB().max.z) / 2.f;
}
