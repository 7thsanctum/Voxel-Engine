#include "Vertex.h"
#include "Effects.h"

#pragma region InputLayoutDesc

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Voxel[3] = 
{
	// { "Name of Variable", 0, "Variable Type", 0, "Spacing in bytes", D3D11_INPUT_PER_VERTEX_DATA, 0 }
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	//{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

#pragma endregion

#pragma region InputLayouts

ID3D11InputLayout* InputLayouts::Voxel = 0;

void InputLayouts::InitAll(ID3D11Device* device)
{
	D3DX11_PASS_DESC passDesc;

	//
	// Particle
	//

	Effects::RainFX->StreamOutTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Voxel, 3, passDesc.pIAInputSignature, 
		passDesc.IAInputSignatureSize, &Voxel));
}

void InputLayouts::DestroyAll()
{
	ReleaseCOM(Voxel);
}

#pragma endregion
