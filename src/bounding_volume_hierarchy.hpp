#pragma once
#include "figures/drawable.hpp"
#include "figures/bounding_figures.hpp"

#include <vector>
#include <algorithm>
#include <memory>

enum class AXIS
{
	X, Y, Z
};

struct Node
{
	Node(const std::vector<Drawable*>& objects);
	std::unique_ptr<Node> left;
	std::unique_ptr<Node> right;
	std::vector<Drawable*> objects;
	AABB aabb;
};

AABB boundAABBs(const std::vector<Drawable*>& objects);
AXIS findLongestAxis(const AABB& aabb);

bool sortXAxis(const Drawable* a, const Drawable* b);
bool sortYAxis(const Drawable* a, const Drawable* b);
bool sortZAxis(const Drawable* a, const Drawable* b);

class BoundingVolumeHierarchy
{
public:
	BoundingVolumeHierarchy(const std::vector<Drawable*>& objects, int leafSize = 128);
	Node* root() { return m_root.get(); }
private:
	void build(Node* root);
private:
	std::unique_ptr<Node> m_root;
	int m_leafSize;
};
