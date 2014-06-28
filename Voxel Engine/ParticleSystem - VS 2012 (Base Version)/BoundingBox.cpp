#include "BoundingBox.h"

BoundingBox::BoundingBox(void)
{
	min= XMFLOAT3(0.0f, 0.0f, 0.0f);
	max = XMFLOAT3(0.0f, 0.0f, 0.0f);
	Validity = BoundingBox::Invalid;
}

BoundingBox::BoundingBox(const XMFLOAT3 &min, const XMFLOAT3 &max)
{
        this->min = min;
        this->max= max;
        Validity = BoundingBox::Valid;
}

BoundingBox::BoundingBox(float minx,float miny,float minz,float maxx,float maxy,float maxz)
{
        min.x = minx;
        min.y = miny;
        min.z = minz;

        max.x = maxx;
        max.y = maxy;
        max.z = maxz;

        Validity = BoundingBox::Valid;
}

BoundingBox::~BoundingBox(void)
{
}

void BoundingBox::expandToInclude(XMFLOAT3 pt)
{
	if (Validity == BoundingBox::Valid)
	{
		if (pt.x < min.x) { min.x = pt.x; }
		else if (pt.x > max.x) { max.x = pt.x; }

		if (pt.y < min.y) { min.y = pt.y; }
		else if (pt.y > max.y) { max.y = pt.y; }

		if (pt.z < min.z) { min.z = pt.z; }
		else if (pt.z > max.z) { max.z = pt.z; }
	}
	else
	{
		min = max = pt;
		Validity = BoundingBox::Valid;
	}
}

bool BoundingBox::contains(XMFLOAT3 pt)
{
	return ((pt.x >= min.x) && (pt.x <= max.x) && (pt.y >= min.y) && (pt.y <= max.y) && (pt.z >= min.z) && (pt.z <= max.z));
}

bool BoundingBox::intersect(BoundingBox &box)
{
	if (!((min.x <= box.max.x) && (max.x >= box.min.x)))
		return false;

	if (!((min.y <= box.max.y) && (max.y >= box.min.y)))
		return false;

	return ((min.z <= box.max.z) && (max.z >= box.min.z));
}

 BoundingBox::PlaneIntersectionType BoundingBox::intersect(Plane &plane)
{
	XMFLOAT3 vector;
	XMFLOAT3 vector2;
	vector2.x = (plane.n.x >= 0.0f) ? min.x : max.x;
	vector2.y = (plane.n.y >= 0.0f) ? min.y : max.y;
	vector2.z = (plane.n.z >= 0.0f) ? min.z : max.z;
	vector.x = (plane.n.x >= 0.0f) ? max.x : min.x;
	vector.y = (plane.n.y >= 0.0f) ? max.y : min.y;
	vector.z = (plane.n.z >= 0.0f) ? max.z : min.z;


	float num = Math::dot(plane.n,vector2);
	if ((num + plane.d) >= 0.0f)
	{
		return Front;
	}

	num = Math::dot(plane.n,vector);
	if ((num + plane.d) <= 0.0f)
	{
		return Back;
	}

	return Intersecting;
}

XMFLOAT3 BoundingBox::onBorder(XMFLOAT3 pt)
{
	XMFLOAT3 result(0,0,0);

    int x = min.x - pt.x;
    if(x == 0)
		result.x = -1;

    x = max.x - pt.x;
    if(x == 0)
		result.x = 1;

    int y =  min.y - pt.y;
    if(y == 0)
		result.y = -1;

    y = max.y - pt.y;
    if(y == 0)
		result.y = 1;

    int z =  min.z - pt.z;
    if(z == 0)
		result.z = -1;

    z = max.z - pt.z;
    if(z == 0)
		result.z = 1;

    return result;
}

XMFLOAT3 BoundingBox::getCenter() const
{
	XMFLOAT3 add = XMFLOAT3((min.x + max.x)*0.5f, (min.y + max.y)*0.5f, (min.x + max.y)*0.5f);
	return add;
}

float BoundingBox::getRadius() const
{
	return getSize() * 0.5f;
}

float BoundingBox::getSize() const
{
	XMFLOAT3 add = XMFLOAT3((min.x - max.x), (min.y - max.y), (min.x - max.y));
	

	return sqrt((add.x*add.x) + (add.y*add.y) + (add.z*add.z));
}