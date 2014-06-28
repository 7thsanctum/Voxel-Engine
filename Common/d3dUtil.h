//***************************************************************************************
// d3dUtil.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#ifndef D3DUTIL_H
#define D3DUTIL_H

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
 
#define EPSILON 1e-6f

#include <d3dx11.h>
#include "d3dx11Effect.h"
#include <xnamath.h>
#include <dxerr.h>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "MathHelper.h"
#include "LightHelper.h"

//---------------------------------------------------------------------------------------
// Simple d3d error checker for book demos.
//---------------------------------------------------------------------------------------

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x) (x)
	#endif

#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 


//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------

#define SafeDelete(x) { delete x; x = 0; }

//---------------------------------------------------------------------------------------
// Utility classes.
//---------------------------------------------------------------------------------------

class d3dHelper
{
public:
	///<summary>
	/// 
	/// Does not work with compressed formats.
	///</summary>
	static ID3D11ShaderResourceView* CreateTexture2DArraySRV(
		ID3D11Device* device, ID3D11DeviceContext* context,
		std::vector<std::wstring>& filenames,
		DXGI_FORMAT format = DXGI_FORMAT_FROM_FILE,
		UINT filter = D3DX11_FILTER_NONE, 
		UINT mipFilter = D3DX11_FILTER_LINEAR);

	static ID3D11ShaderResourceView* CreateRandomTexture1DSRV(ID3D11Device* device);
};

class TextHelper
{
public:

	template<typename T>
	static D3DX11INLINE std::wstring ToString(const T& s)
	{
		std::wostringstream oss;
		oss << s;

		return oss.str();
	}

	template<typename T>
	static D3DX11INLINE T FromString(const std::wstring& s)
	{
		T x;
		std::wistringstream iss(s);
		iss >> x;

		return x;
	}
};

// Order: left, right, bottom, top, near, far.
void ExtractFrustumPlanes(XMFLOAT4 planes[6], CXMMATRIX M);


// #define XMGLOBALCONST extern CONST __declspec(selectany)
//   1. extern so there is only one copy of the variable, and not a separate
//      private copy in each .obj.
//   2. __declspec(selectany) so that the compiler does not complain about
//      multiple definitions in a .cpp file (it can pick anyone and discard 
//      the rest because they are constant--all the same).

namespace Colors
{
	XMGLOBALCONST XMVECTORF32 White     = {1.0f, 1.0f, 1.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Black     = {0.0f, 0.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Red       = {1.0f, 0.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Green     = {0.0f, 1.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Blue      = {0.0f, 0.0f, 1.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Yellow    = {1.0f, 1.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Cyan      = {0.0f, 1.0f, 1.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Magenta   = {1.0f, 0.0f, 1.0f, 1.0f};

	XMGLOBALCONST XMVECTORF32 Silver    = {0.75f, 0.75f, 0.75f, 1.0f};
	XMGLOBALCONST XMVECTORF32 LightSteelBlue = {0.69f, 0.77f, 0.87f, 1.0f};
	XMGLOBALCONST XMVECTORF32 SkyBlue = {0.2f, 0.8f, 1.0f, 1.0f};
}

///<summary>
/// Utility class for converting between types and formats.
///</summary>
class Convert
{
public:
	///<summary>
	/// Converts XMVECTOR to XMCOLOR, where XMVECTOR represents a color.
	///</summary>
	static D3DX11INLINE XMCOLOR ToXmColor(FXMVECTOR v)
	{
		XMCOLOR dest;
		XMStoreColor(&dest, v);
		return dest;
	}

	///<summary>
	/// Converts XMVECTOR to XMFLOAT4, where XMVECTOR represents a color.
	///</summary>
	static D3DX11INLINE XMFLOAT4 ToXmFloat4(FXMVECTOR v)
	{
		XMFLOAT4 dest;
		XMStoreFloat4(&dest, v);
		return dest;
	}

	

	static D3DX11INLINE UINT ArgbToAbgr(UINT argb)
	{
		BYTE A = (argb >> 24) & 0xff;
		BYTE R = (argb >> 16) & 0xff;
		BYTE G = (argb >>  8) & 0xff;
		BYTE B = (argb >>  0) & 0xff;

		return (A << 24) | (B << 16) | (G << 8) | (R << 0);
	}

};

class Math 
{
public: 
	/*static D3DX11INLINE float dot(const Plane &p, const XMFLOAT3 &pt)
	{
		// Returns:
		//  > 0 if the point 'pt' lies in front of the plane 'p'
		//  < 0 if the point 'pt' lies behind the plane 'p'
		//    0 if the point 'pt' lies on the plane 'p'
		//
		// The signed distance from the point 'pt' to the plane 'p' is returned.
		float val = (Math::dot(p.n, pt));
		return val + p.d;
	}*/

	static D3DX11INLINE float dot(const XMFLOAT3 &p, const XMFLOAT3 &pt)
	{
		return ((p.x * pt.x) + (p.y * pt.y) + (p.z * pt.z));
	}

	static D3DX11INLINE XMFLOAT3 XMAdd(const XMFLOAT3 &a, const XMFLOAT3 &b)
	{
		XMFLOAT3 c = XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
		return c;
	}

	static D3DX11INLINE XMFLOAT3 XMSub(const XMFLOAT3 &a, const XMFLOAT3 &b)
	{
		XMFLOAT3 c = XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
		return c;
	}

	static D3DX11INLINE XMFLOAT3 XMMul(const XMFLOAT3 &a, const float &b)
	{
		XMFLOAT3 c = XMFLOAT3(a.x * b, a.y * b, a.z * b);
		return c;
	}

	static D3DX11INLINE bool closeEnough(float f1, float f2)
	{
		// Determines whether the two floating-point values f1 and f2 are
		// close enough together that they can be considered equal.

		return fabsf((f1 - f2) / ((f2 == 0.0f) ? 1.0f : f2)) < EPSILON;
	}

	static D3DX11INLINE float distance(const XMFLOAT3 &a, const XMFLOAT3 &b)
	{
		XMFLOAT3 d = XMSub(a, b);

		return sqrt((d.x*d.x) + (d.y*d.y) + (d.z*d.z));
	}

	static D3DX11INLINE XMFLOAT3 clamp(const XMFLOAT3 &a, const XMFLOAT3 &b, const XMFLOAT3 &c)
	{
		// TODO: FIX THIS!
		XMFLOAT3 d = XMSub(a, b);

		return d;
	}
};

#endif // D3DUTIL_H