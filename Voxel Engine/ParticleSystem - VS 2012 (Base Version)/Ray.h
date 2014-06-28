#pragma once
#define NOMINMAX

#include "d3dUtil.h"

#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "Plane.h"

class Ray
{
public:
	XMFLOAT3 orgin;
	XMFLOAT3 direction;

	Ray();
	Ray(const XMFLOAT3 &orgin, const XMFLOAT3 &direction);
	~Ray();

	bool intersected(const BoundingSphere &sphere) const;
	bool intersected(const BoundingBox &box) const;
	void intersected(BoundingBox box,float &distance);
	bool intersected(const Plane &plane) const;
	bool intersected(const Plane &plane, float &t, XMFLOAT3 &intersection) const;
};

