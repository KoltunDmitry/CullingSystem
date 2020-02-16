#pragma once
#include "bounding_figures.hpp"

class Drawable
{
public:
	Drawable();
	int getID() const { return m_id; }
	virtual void setWorldTransform(glm::mat4 matrix) = 0;
	virtual void draw(float dTime) = 0;
	virtual AABB getAABB() const = 0;
	virtual Sphere getSphere() const = 0;

private:
	int m_id;
};