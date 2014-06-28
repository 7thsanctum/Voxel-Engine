#include "Plane.h"

Plane::Plane(void)
{

}

Plane::Plane(float a, float b, float c, float d) : n(a,b,c), d(d)
{

}

Plane::Plane(const XMFLOAT3 &pt, const XMFLOAT3 &normal)
{
	fromPointNormal(pt, normal);
}

Plane::~Plane(void)
{
}

Plane::Plane(const XMFLOAT3 &pt1, const XMFLOAT3 &pt2, const XMFLOAT3 &pt3)
{
	fromPoints(pt1, pt2, pt3);
}

float Plane::dot(const XMFLOAT3 &pt)
{
	// Returns:
	//  > 0 if the point 'pt' lies in front of the plane 'p'
	//  < 0 if the point 'pt' lies behind the plane 'p'
	//    0 if the point 'pt' lies on the plane 'p'
	//
	// The signed distance from the point 'pt' to the plane 'p' is returned.
	float val = (dot(n, pt));
	return val + d;
}

float Plane::dot(const Plane &p, const XMFLOAT3 &pt)
{
	// Returns:
	//  > 0 if the point 'pt' lies in front of the plane 'p'
	//  < 0 if the point 'pt' lies behind the plane 'p'
	//    0 if the point 'pt' lies on the plane 'p'
	//
	// The signed distance from the point 'pt' to the plane 'p' is returned.
	float val = (dot(p.n, pt));
	return val + p.d;
}

float Plane::dot(const XMFLOAT3 &p, const XMFLOAT3 &pt)
{
	return ((p.x * pt.x) + (p.y * pt.y) + (p.z * pt.z));
}

float Plane::length(const XMFLOAT3 &pt)
{
	return sqrt((pt.x*pt.x) + (pt.y*pt.y) + (pt.z*pt.z));
}

XMFLOAT3 Plane::cross(const XMFLOAT3 &vg1, const XMFLOAT3 &vg2)
{
	XMFLOAT3 result;
	result.x =  vg1.y * vg2.z - vg1.z * vg2.y; 
    result.y =-(vg1.x * vg2.z - vg1.z * vg2.x);
    result.z =  vg1.x * vg2.y - vg1.y * vg2.z;
	return result;
}

void Plane::fromPointNormal(const XMFLOAT3 &pt, const XMFLOAT3 &normal)
{
        set(normal.x, normal.y, normal.z, -dot(normal, pt));
        normalize();
}

void Plane::fromPoints(const XMFLOAT3 &pt1, const XMFLOAT3 &pt2, const XMFLOAT3 &pt3)
{
	XMFLOAT3 a (pt2.x - pt1.x, pt2.y - pt1.y, pt2.z - pt1.z);
	XMFLOAT3 b (pt3.x - pt1.x, pt3.y - pt1.y, pt3.z - pt1.z);
	n = cross(a, b);
	d = -dot(n, pt1);
	normalize();
}

const XMFLOAT3 &Plane::normal() const
{
        return n;
}

XMFLOAT3 &Plane::normal()
{
        return n;
}

void Plane::normalize()
{
	// l is normalised length
	float l = 1.0f / length(n);

	n.x *= l;
	n.y *= l;
	n.z *= l;

	d *= l;
}

void Plane::set(float a, float b, float c, float d)
{
	n.x = a;
	n.y = b;
	n.z = c;
	this->d = d;
}