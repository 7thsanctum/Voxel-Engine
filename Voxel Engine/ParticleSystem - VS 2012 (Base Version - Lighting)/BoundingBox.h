#pragma once

#include "d3dUtil.h"
#include "Plane.h"

class BoundingBox
{
public:
	enum PointValidity { Invalid = 0, Valid =1};
	enum PlaneIntersectionType { Front = 0, Back = 1, Intersecting = 2};
	PointValidity Validity;

	XMFLOAT3 min;
	XMFLOAT3 max;

	BoundingBox();
	BoundingBox(const XMFLOAT3 &minN, const XMFLOAT3 &maxN);
	BoundingBox(float minx, float miny, float minz, float maxx, float maxy, float maxz);
	~BoundingBox();

	void expandToInclude(XMFLOAT3 pt);

	//intersections
	bool contains(XMFLOAT3 pt);
	bool intersect(BoundingBox &box);
	PlaneIntersectionType intersect (Plane &plane);
	XMFLOAT3 onBorder(XMFLOAT3 pt);

	XMFLOAT3 getCenter() const;
	float getRadius() const;
	float getSize() const;

};

