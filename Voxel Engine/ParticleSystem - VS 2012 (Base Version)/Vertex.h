#ifndef VERTEX_H
#define VERTEX_H

#include "d3dUtil.h"

namespace Vertex
{
	struct Voxel
	{
		XMFLOAT3 InitialPos;
		float Age;
		unsigned int Type;
	};
}

class InputLayoutDesc
{
public:
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC Voxel[3];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* Voxel;
};

#endif // VERTEX_H