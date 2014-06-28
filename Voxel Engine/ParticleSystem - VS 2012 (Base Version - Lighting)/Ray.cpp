#include "Ray.h"
#include <limits>

Ray::Ray()
{
}


Ray::Ray(const XMFLOAT3 &orgin, const XMFLOAT3 &direction)
{
	this->orgin = orgin;
	this->direction = direction;
}

Ray::~Ray(void)
{
}

bool Ray::intersected(const BoundingSphere &sphere) const 
{
	XMFLOAT3 w = Math::XMSub(sphere.center, orgin);
	float wsq = Math::dot(w,w);
	float proj = Math::dot(w,direction);
	float rsq = sphere.radius *sphere.radius;

	if(proj < 0.0f && wsq > rsq)
	{
		return false;
	}

	float vsq = Math::dot(direction, direction);

	return vsq*wsq - proj*proj <=vsq *rsq;
}


void Ray::intersected(BoundingBox box, float &distance)
{
	distance = -1.0f;
	float num = 0.0f;
	float num2 = std::numeric_limits<float>::max();
    if (fabs(direction.x) < 1E-06f)
    {
        if (orgin.x < box.min.x || orgin.x > box.max.x)
        {
                return;
        }
	}else
	{
		float num3 = 1.0f / direction.x;
		float num4 = (box.min.x - orgin.x) * num3;
		float num5 = (box.max.x - orgin.x) * num3;
		if(num4 > num5)
		{
			float num6 = num4;
			num4 = num5;
			num5 = num6;
		}

		num = std::max(num4, num);
		num2 = std::min(num5, num2);
		if(num > num2)
		{
			return;
		}
	}
	if(fabs(direction.y) < 1E-06f)
	{
		if(orgin.y < box.min.y || orgin.y > box.max.y)
		{
			return;
		}
	}else 
	{
		float num7 = 1.0f / direction.y;
		float num8 = (box.min.y - orgin.y) *num7;
		float num9 = (box.max.y - orgin.y) * num7;
		if(num8 > num9)
		{
			float num10 = num8;
			num8 = num9;
			num9 = num10;
		}

		num = std::max(num8, num);
		num2 = std::min(num9, num2);
		if(num > num2)
		{
			return;
		}
	}

	if(fabs(direction.z) < 1E-06f)
	{
		if(orgin.z < box.min.z || orgin.z > box.max.z)
		{
			return;
		}
	}
	else
	{
		float num11 = 1.0f/direction.z;
		float num12 = (box.min.z - orgin.z) *num11;
		float num13 = (box.max.z - orgin.z) *num11;
		if(num12 > num13)
		{
			float num14 = num12;
			num12 = num13;
			num13 = num14;
		}
		num = std::max(num12,num);
		num2 = std::min(num13,num2);
		if(num > num2)
		{
			return;
		}
	}
	distance = num;

}

bool Ray::intersected(const BoundingBox &box) const
{
        // References:
        //  Jeffrey Mahovsky and Brian Wyvill, "Fast Ray-Axis Aligned Bounding Box
        //  Overlap Tests with Plücker Coordinates", Journal of Graphics Tools,
        //  9(1):35-46.

        if (direction.x < 0.0f)
        {
                if (direction.y < 0.0f)
                {
                        if (direction.z < 0.0f)
                        {
                                // case MMM: side(R,HD) < 0 or side(R,FB) > 0 or side(R,EF) > 0 or side(R,DC) < 0 or side(R,CB) < 0 or side(R,HE) > 0 to miss

                                if ((orgin.x < box.min.x) || (orgin.y < box.min.y) || (orgin.z < box.min.z))
                                        return false;

                                float xa = box.min.x - orgin.x;
                                float ya = box.min.y - orgin.y;
                                float za = box.min.z - orgin.z;
                                float xb = box.max.x - orgin.x;
                                float yb = box.max.y - orgin.y;
                                float zb = box.max.z - orgin.z;

                                if ((direction.x * ya - direction.y * xb < 0)
                                                || (direction.x * yb - direction.y * xa > 0)
                                                || (direction.x * zb - direction.z * xa > 0)
                                                || (direction.x * za - direction.z * xb < 0)
                                                || (direction.y * za - direction.z * yb < 0)
                                                || (direction.y * zb - direction.z * ya > 0))
                                        return false;

                                return true;
                        }
                        else
                        {
                                // case MMP: side(R,HD) < 0 or side(R,FB) > 0 or side(R,HG) > 0 or side(R,AB) < 0 or side(R,DA) < 0 or side(R,GF) > 0 to miss

                                if ((orgin.x < box.min.x) || (orgin.y < box.min.y) || (orgin.z > box.max.z))
                                        return false;

                                float xa = box.min.x - orgin.x;
                                float ya = box.min.y - orgin.y;
                                float za = box.min.z - orgin.z;
                                float xb = box.max.x - orgin.x;
                                float yb = box.max.y - orgin.y;
                                float zb = box.max.z - orgin.z;

                                if ((direction.x * ya - direction.y * xb < 0.0f)
                                                || (direction.x * yb - direction.y * xa > 0.0f)
                                                || (direction.x * zb - direction.z * xb > 0.0f)
                                                || (direction.x * za - direction.z * xa < 0.0f)
                                                || (direction.y * za - direction.z * ya < 0.0f)
                                                || (direction.y * zb - direction.z * yb > 0.0f))
                                        return false;

                                return true;
                        }
                }
                else
                {
                        if (direction.z < 0.0f)
                        {
                                // case MPM: side(R,EA) < 0 or side(R,GC) > 0 or side(R,EF) > 0 or side(R,DC) < 0 or side(R,GF) < 0 or side(R,DA) > 0 to miss

                                if ((orgin.x < box.min.x) || (orgin.y > box.max.y) || (orgin.z < box.min.z))
                                        return false;

                                float xa = box.min.x - orgin.x;
                                float ya = box.min.y - orgin.y;
                                float za = box.min.z - orgin.z;
                                float xb = box.max.x - orgin.x;
                                float yb = box.max.y - orgin.y;
                                float zb = box.max.z - orgin.z;

                                if ((direction.x * ya - direction.y * xa < 0.0f)
                                                || (direction.x * yb - direction.y * xb > 0.0f)
                                                || (direction.x * zb - direction.z * xa > 0.0f)
                                                || (direction.x * za - direction.z * xb < 0.0f)
                                                || (direction.y * zb - direction.z * yb < 0.0f)
                                                || (direction.y * za - direction.z * ya > 0.0f))
                                        return false;

                                return true;
                        }
                        else
                        {
                                // case MPP: side(R,EA) < 0 or side(R,GC) > 0 or side(R,HG) > 0 or side(R,AB) < 0 or side(R,HE) < 0 or side(R,CB) > 0 to miss

                                if ((orgin.x < box.min.x) || (orgin.y > box.max.y) || (orgin.z > box.max.z))
                                        return false;

                                float xa = box.min.x - orgin.x;
                                float ya = box.min.y - orgin.y;
                                float za = box.min.z - orgin.z;
                                float xb = box.max.x - orgin.x;
                                float yb = box.max.y - orgin.y;
                                float zb = box.max.z - orgin.z;

                                if ((direction.x * ya - direction.y * xa < 0.0f)
                                                || (direction.x * yb - direction.y * xb > 0.0f)
                                                || (direction.x * zb - direction.z * xb > 0.0f)
                                                || (direction.x * za - direction.z * xa < 0.0f)
                                                || (direction.y * zb - direction.z * ya < 0.0f)
                                                || (direction.y * za - direction.z * yb > 0.0f))
                                        return false;

                                return true;
                        }
                }
        }
        else
        {
                if (direction.y < 0.0f)
                {
                        if (direction.z < 0.0f)
                        {
                                // case PMM: side(R,GC) < 0 or side(R,EA) > 0 or side(R,AB) > 0 or side(R,HG) < 0 or side(R,CB) < 0 or side(R,HE) > 0 to miss

                                if ((orgin.x > box.max.x) || (orgin.y < box.min.y) || (orgin.z < box.min.z))
                                        return false;

                                float xa = box.min.x - orgin.x;
                                float ya = box.min.y - orgin.y;
                                float za = box.min.z - orgin.z;
                                float xb = box.max.x - orgin.x;
                                float yb = box.max.y - orgin.y;
                                float zb = box.max.z - orgin.z;

                                if ((direction.x * yb - direction.y * xb < 0.0f)
                                                || (direction.x * ya - direction.y * xa > 0.0f)
                                                || (direction.x * za - direction.z * xa > 0.0f)
                                                || (direction.x * zb - direction.z * xb < 0.0f)
                                                || (direction.y * za - direction.z * yb < 0.0f)
                                                || (direction.y * zb - direction.z * ya > 0.0f))
                                        return false;

                                return true;
                        }
                        else
                        {
                                // case PMP: side(R,GC) < 0 or side(R,EA) > 0 or side(R,DC) > 0 or side(R,EF) < 0 or side(R,DA) < 0 or side(R,GF) > 0 to miss

                                if ((orgin.x > box.max.x) || (orgin.y < box.min.y) || (orgin.z > box.max.z))
                                        return false;

                                float xa = box.min.x - orgin.x;
                                float ya = box.min.y - orgin.y;
                                float za = box.min.z - orgin.z;
                                float xb = box.max.x - orgin.x;
                                float yb = box.max.y - orgin.y;
                                float zb = box.max.z - orgin.z;

                                if ((direction.x * yb - direction.y * xb < 0.0f)
                                                || (direction.x * ya - direction.y * xa > 0.0f)
                                                || (direction.x * za - direction.z * xb > 0.0f)
                                                || (direction.x * zb - direction.z * xa < 0.0f)
                                                || (direction.y * za - direction.z * ya < 0.0f)
                                                || (direction.y * zb - direction.z * yb > 0.0f))
                                        return false;

                                return true;
                        }
                }
                else
                {
                        if (direction.z < 0.0f)
                        {
                                // case PPM: side(R,FB) < 0 or side(R,HD) > 0 or side(R,AB) > 0 or side(R,HG) < 0 or side(R,GF) < 0 or side(R,DA) > 0 to miss

                                if ((orgin.x > box.max.x) || (orgin.y > box.max.y) || (orgin.z < box.min.z))
                                        return false;

                                float xa = box.min.x - orgin.x;
                                float ya = box.min.y - orgin.y;
                                float za = box.min.z - orgin.z;
                                float xb = box.max.x - orgin.x;
                                float yb = box.max.y - orgin.y;
                                float zb = box.max.z - orgin.z;

                                if ((direction.x * yb - direction.y * xa < 0.0f)
                                                || (direction.x * ya - direction.y * xb > 0.0f)
                                                || (direction.x * za - direction.z * xa > 0.0f)
                                                || (direction.x * zb - direction.z * xb < 0.0f)
                                                || (direction.y * zb - direction.z * yb < 0.0f)
                                                || (direction.y * za - direction.z * ya > 0.0f))
                                        return false;

                                return true;
                        }
                        else
                        {
                                // case PPP: side(R,FB) < 0 or side(R,HD) > 0 or side(R,DC) > 0 or side(R,EF) < 0 or side(R,HE) < 0 or side(R,CB) > 0 to miss

                                if ((orgin.x > box.max.x) || (orgin.y > box.max.y) || (orgin.z > box.max.z))
                                        return false;

                                float xa = box.min.x - orgin.x;
                                float ya = box.min.y - orgin.y;
                                float za = box.min.z - orgin.z;
                                float xb = box.max.x - orgin.x;
                                float yb = box.max.y - orgin.y;
                                float zb = box.max.z - orgin.z;

                                if ((direction.x * yb - direction.y * xa < 0.0f)
                                                || (direction.x * ya - direction.y * xb > 0.0f)
                                                || (direction.x * za - direction.z * xb > 0.0f)
                                                || (direction.x * zb - direction.z * xa < 0.0f)
                                                || (direction.y * zb - direction.z * ya < 0.0f)
                                                || (direction.y * za - direction.z * yb > 0.0f))
                                        return false;

                                return true;
                        }
                }
        }

        return false;
}


bool Ray::intersected(const Plane &plane) const
{
	float t;
	XMFLOAT3 intersection;
	return intersected(plane, t, intersection);
}

bool Ray::intersected(const Plane &plane, float &t, XMFLOAT3 &intersection) const 
{
	float denominator = Math::dot(direction, plane.n);

	//check if ray is parallel to palen
	if(Math::closeEnough(fabs(denominator), 0.0f))
	{
		float rayOrginToPlane = Plane::dot(plane, orgin);

		if(Math::closeEnough(rayOrginToPlane, 0.0f))
		{
			t = 0.0f;
			return true;
		}else
		{
			return false;
		}
	}

	t = -Plane::dot(plane,orgin) / denominator;

	if(t < 0.0f)
	{
		return false;
	}

	intersection = Math::XMAdd(orgin, Math::XMMul(direction, t));
	return true;

}