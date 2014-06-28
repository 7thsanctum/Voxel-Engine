#include "BoundingSphere.h"


BoundingSphere::BoundingSphere(void)
{
	center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	radius = 0.0f;
}

BoundingSphere::BoundingSphere(const XMFLOAT3 &center, float radius)
{
	this->center = center;
	this->radius = radius;
}

BoundingSphere::~BoundingSphere(void)
{
}


bool BoundingSphere::hasCollided(const BoundingSphere &other) const
{
	XMFLOAT3 disp = Math::XMSub(other.center, center);
    float lengthSq = (disp.x * disp.x) + (disp.y * disp.y) + (disp.z * disp.z);
    float radiiSq = (other.radius + radius) * (other.radius + radius);

    return (lengthSq < radiiSq) ? true : false;
}

bool BoundingSphere::intersect(BoundingBox &box)
{
    XMFLOAT3 vector = Math::clamp(center, box.min, box.max);
    float num = Math::distance(center, vector);
	return (num <= radius);
	//    return (num <= (radius * radius));
}

bool BoundingSphere::inside(BoundingBox &box, XMFLOAT3 &axis, float &distance)
{
    if(axis.x == 0.0f)
    {
            if(center.x - radius <= box.min.x || center.x + radius >= box.max.x)
                    return false;
    }
    if(axis.y == 0.0f)
    {
            if(center.y - radius <= box.min.y || center.y + radius >= box.max.y)
                    return false;
    }
    if(axis.z == 0.0f)
    {
            if(center.z - radius <= box.min.z || center.z + radius >= box.max.z)
                    return false;
    }

    return true;
                      
}

bool BoundingSphere::PointCollided(XMFLOAT3 &point)
{
	XMFLOAT3 disp = Math::XMSub(point, center);
	float lengthSq = (disp.x * disp.x) + (disp.y * disp.y) + (disp.z * disp.z);
	float radiiSq = (radius) * ( radius);

	return (lengthSq < radiiSq) ? true : false;
}