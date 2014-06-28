#pragma once
#include "Frustum.h"
//#include "Vertex.h"

Frustum::Frustum(void)
{
}


Frustum::~Frustum(void)
{
}


void Frustum::ExtractPlanes(XMMATRIX  &clip)
{
	//XMFLOAT4X4 clip;	
	//XMStoreFloat4x4(&clip, matrix);

	Plane *pPlane = 0;
    pPlane = &planes[FRUSTUM_PLANE_NEAR];
    pPlane->set(clip.m[0][3] + clip.m[0][3], 
				clip.m[1][3] + clip.m[1][3], 
				clip.m[2][3] + clip.m[2][3], 
				clip.m[3][3] + clip.m[3][3]);
    pPlane->normalize();

    // Far clipping plane.
    pPlane = &planes[FRUSTUM_PLANE_FAR];
    pPlane->set(clip.m[0][3] - clip.m[0][2], 
				clip.m[1][3] - clip.m[1][2],
				clip.m[2][3] - clip.m[2][2],
				clip.m[3][3] - clip.m[3][2]);
    pPlane->normalize();

    // Left clipping plane.
    pPlane = &planes[FRUSTUM_PLANE_LEFT];
    pPlane->set(clip.m[0][3] + clip.m[0][0], 
				clip.m[1][3] + clip.m[1][0],
				clip.m[2][3] + clip.m[2][0],
				clip.m[3][3] + clip.m[3][0]);
    pPlane->normalize();

	// Right clipping plane
    pPlane = &planes[FRUSTUM_PLANE_RIGHT];
    pPlane->set(clip.m[0][3] - clip.m[0][0], 
				clip.m[1][3] - clip.m[1][0], 
				clip.m[2][3] - clip.m[2][0], 
				clip.m[3][3] - clip.m[3][0]);
    pPlane->normalize();

	// Bottom clipping plane
    pPlane = &planes[FRUSTUM_PLANE_BOTTOM];
    pPlane->set(clip.m[0][3] + clip.m[0][1],
				clip.m[1][3] + clip.m[1][1],
				clip.m[2][3] + clip.m[2][1],
				clip.m[3][3] + clip.m[3][1]);
    pPlane->normalize();

	// Top clipping plane
    pPlane = &planes[FRUSTUM_PLANE_TOP];
    pPlane->set(clip.m[0][3] - clip.m[0][1], 
				clip.m[1][3] - clip.m[1][1], 
				clip.m[2][3] - clip.m[2][1], 
				clip.m[3][3] - clip.m[3][1]);
    pPlane->normalize();

	/*	// Alternative near plane
	m_frustum[4].a = viewProjection._13;
    m_frustum[4].b = viewProjection._23;
    m_frustum[4].c = viewProjection._33;
    m_frustum[4].d = viewProjection._43;
	*/
	/* // Incase there is any problems, remember to check that the matrix values are correct
	Plane *pPlane = 0;
            pPlane = &planes[FRUSTUM_PLANE_NEAR];
            pPlane->set(clip[0].w + clip[0].z, clip[1].w + clip[1].z, clip[2].w + clip[2].z, clip[3].w + clip[3].z);
    pPlane->normalize();

    // Left clipping plane.
    pPlane = &planes[FRUSTUM_PLANE_FAR];
    pPlane->set(clip[0].w - clip[0].z, clip[1].w - clip[1].z,clip[2].w - clip[2].z,clip[3].w - clip[3].z);
    pPlane->normalize();

    // Left clipping plane.
    pPlane = &planes[FRUSTUM_PLANE_LEFT];
    pPlane->set(clip[0].w + clip[0].x,clip[1].w + clip[1].x,clip[2].w + clip[2].x,clip[3].w + clip[3].x);
    pPlane->normalize();

    pPlane = &planes[FRUSTUM_PLANE_RIGHT];
    pPlane->set(clip[0].w - clip[0].x, clip[1].w - clip[1].x, clip[2].w - clip[2].x, clip[3].w - clip[3].x);
    pPlane->normalize();

    pPlane = &planes[FRUSTUM_PLANE_BOTTOM];
    pPlane->set(clip[0].w + clip[0].y,clip[1].w + clip[1].y,clip[2].w + clip[2].y,clip[3].w + clip[3].y);
    pPlane->normalize();

    pPlane = &planes[FRUSTUM_PLANE_TOP];
    pPlane->set(clip[0].w - clip[0].y, clip[1].w - clip[1].y, clip[2].w - clip[2].y, clip[3].w- clip[3].y);
    pPlane->normalize();
	*/
}

Frustum::PlaneIntersectionType Frustum::BoxInFrustum(float minx,float miny,float minz,float maxx,float maxy,float maxz)
{
        BoundingBox box(minx,miny,minz,maxx,maxy,maxz);
        bool flag = false;

        for (int i = 0; i < 6; ++i)
        {
                switch(box.intersect(planes[i]))
                {
                    case BoundingBox::Back:
                    return Outside;

                    case BoundingBox::Front:
                    case BoundingBox::Intersecting:
                        flag = true;
                        break;
                }
        }

        if (!flag)
        {
                return Inside;
        }


        return Intersects;
}

int Frustum::CubeInFrustum(const XMFLOAT3 &center, float x, float y, float z)
{
    // NOTE : This code can be optimized, it is just easier to read and understand as is

    int result = Inside;

    for(int i = 0; i < 6; i++)
    {
        // Reset counters for corners in and out
        int out = 0;
        int in = 0;
		//Plane::dot
		if(planes[i].dot(Math::XMAdd(center, XMFLOAT3(-x, -y, -z))) < 0) {
            out++;
        }
        else {
            in++;
        }

        if(planes[i].dot(Math::XMAdd(center, XMFLOAT3(x, -y, -z))) < 0) {
            out++;
        }
        else {
            in++;
        }

        if(planes[i].dot(Math::XMAdd(center, XMFLOAT3(-x, -y, z))) < 0) {
            out++;
        }
        else {
            in++;
        }

        if(planes[i].dot(Math::XMAdd(center, XMFLOAT3(x, -y, z))) < 0) {
            out++;
        }
        else {
            in++;
        }

        if(planes[i].dot(Math::XMAdd(center, XMFLOAT3(-x, y, -z))) < 0) {
            out++;
        }
        else {
            in++;
        }

        if(planes[i].dot(Math::XMAdd(center, XMFLOAT3(x, y, -z))) < 0) {
            out++;
        }
        else {
            in++;
        }

        if(planes[i].dot(Math::XMAdd(center, XMFLOAT3(-x, y, z))) < 0) {
            out++;
        }
        else {
            in++;
        }

        if(planes[i].dot(Math::XMAdd(center, XMFLOAT3(x, y, z))) < 0) {
            out++;
        }
        else {
            in++;
        }

        // If all corners are out
        if(!in) {
            return Outside;
        }
        // If some corners are out and others are in	
        else if(out) {
            result = Intersects;
        }
    }

    return(result);
}

Frustum::PlaneIntersectionType Frustum::BoxInFrustum(BoundingBox &box)
{
    int flag = 0;
    for (int i = 0; i < 6; ++i)
    {
            switch(box.intersect(planes[i]))
            {
                    case BoundingBox::Front:
                    case BoundingBox::Intersecting:
                    {
                            flag = 1;
                    }break;

                    case BoundingBox::Back:
                    return Outside;
            }
    }

    return Intersects;
}

 bool Frustum::BBoxInFrustum(const BoundingBox &box) const
{
	XMFLOAT3 c = XMFLOAT3(Math::XMMul(Math::XMAdd(box.min, box.max), 0.5f));
	float sizex = box.max.x - box.min.x;
	float sizey = box.max.y - box.min.y;
	float sizez = box.max.z - box.min.z;

	XMFLOAT3 corners[8] =
	{
		XMFLOAT3((c.x - sizex * 0.5f), (c.y - sizey * 0.5f), (c.z - sizez * 0.5f)),
		XMFLOAT3((c.x + sizex * 0.5f), (c.y - sizey * 0.5f), (c.z - sizez * 0.5f)),
		XMFLOAT3((c.x - sizex * 0.5f), (c.y + sizey * 0.5f), (c.z - sizez * 0.5f)),
		XMFLOAT3((c.x + sizex * 0.5f), (c.y + sizey * 0.5f), (c.z - sizez * 0.5f)),
		XMFLOAT3((c.x - sizex * 0.5f), (c.y - sizey * 0.5f), (c.z + sizez * 0.5f)),
		XMFLOAT3((c.x + sizex * 0.5f), (c.y - sizey * 0.5f), (c.z + sizez * 0.5f)),
		XMFLOAT3((c.x - sizex * 0.5f), (c.y + sizey * 0.5f), (c.z + sizez * 0.5f)),
		XMFLOAT3((c.x + sizex * 0.5f), (c.y + sizey * 0.5f), (c.z + sizez * 0.5f))
	};

	for (int i = 0; i < 6; ++i)
	{
		if (Plane::dot(planes[i], corners[0]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[1]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[2]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[3]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[4]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[5]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[6]) > 0.0f)
				continue;

		if (Plane::dot(planes[i], corners[7]) > 0.0f)
				continue;

		return false;
	}

	return true;
}

bool Frustum::PointInFrustum(const XMFLOAT3 &point) //const
{
    for (int i = 0; i < 6; ++i)
    {
        if (Plane::dot(planes[i], point) <= 0.0f)
                return false;
    }

    return true;
}

bool Frustum::PointInFrustum(int plane,const XMFLOAT3 &point,float &distance) const
{
    distance = Plane::dot(planes[plane], point);
    if (distance <= 0.0f)
            return false;

    return true;
}

bool Frustum::SphereInFrustum(const BoundingSphere &sphere) const
{
	for (int i = 0; i < 6; ++i)
	{
		if (Plane::dot(planes[i], sphere.center) <= -sphere.radius)
			return false;
	}

	return true;
}