#pragma once
#include "d3dUtil.h"

class Plane
{
public:
        Plane();
        ~Plane();
		XMFLOAT3 n;
        float d;

		float dot(const XMFLOAT3 &pt);						// Does dot product based on THIS plane
        static float dot(const Plane &p, const XMFLOAT3 &pt);		// Does dot product based on defined plane
		static float dot(const XMFLOAT3 &p, const XMFLOAT3 &pt);	// Does dot product of two vectors
		static XMFLOAT3 cross(const XMFLOAT3 &p, const XMFLOAT3 &pt);
		static float length(const XMFLOAT3 &pt);
        Plane(float a, float b, float c, float d);
        Plane(const XMFLOAT3 &pt, const XMFLOAT3 &normal);
        Plane(const XMFLOAT3 &pt1, const XMFLOAT3 &pt2, const XMFLOAT3 &pt3);
        const XMFLOAT3 &normal() const;
        XMFLOAT3 &normal();
        void normalize();
        void set(float a, float b, float c, float d);
        void fromPointNormal(const XMFLOAT3 &pt, const XMFLOAT3 &normal);
        void fromPoints(const XMFLOAT3 &pt1, const XMFLOAT3 &pt2, const XMFLOAT3 &pt3);
};