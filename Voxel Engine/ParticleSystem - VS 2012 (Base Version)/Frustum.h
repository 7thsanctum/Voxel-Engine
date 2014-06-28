#pragma once
#include "d3dUtil.h"
#include "Plane.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"

class Frustum
{
public:
    Frustum();
    virtual ~Frustum();
    enum
    {
        FRUSTUM_PLANE_LEFT   = 0,
        FRUSTUM_PLANE_RIGHT  = 1,
        FRUSTUM_PLANE_BOTTOM = 2,
        FRUSTUM_PLANE_TOP    = 3,
        FRUSTUM_PLANE_NEAR   = 4,
        FRUSTUM_PLANE_FAR    = 5
    };

    enum PlaneIntersectionType 
	{	
		Outside = 0, 
		Inside = 1, 
		Intersects = 2 
	};

    Plane planes[6];
		
    void ExtractPlanes(XMMATRIX &matrix);

	bool BBoxInFrustum(const BoundingBox &box) const;
    bool PointInFrustum(const XMFLOAT3 &point); //const;
    bool PointInFrustum(int plane,const XMFLOAT3 &point,float &distance) const;
	bool SphereInFrustum(const BoundingSphere &sphere) const;

	int CubeInFrustum(const XMFLOAT3 &center, float x, float y, float z);

	PlaneIntersectionType BoxInFrustum(BoundingBox &box);
	PlaneIntersectionType BoxInFrustum(float minx,float miny,float minz,float maxx,float maxy,float maxz);

};