#pragma once

#include "d3dUtil.h"
#include "BoundingBox.h"

class BoundingSphere
{
public:

	XMFLOAT3 center;
	float radius;

	BoundingSphere();
	BoundingSphere(const XMFLOAT3 &center, float radius);
	~BoundingSphere();

	bool hasCollided(const BoundingSphere &other) const;
	bool intersect(BoundingBox &box);
	bool inside(BoundingBox &box,XMFLOAT3 &axis, float &distance);
	bool PointCollided(XMFLOAT3 &point);
};

