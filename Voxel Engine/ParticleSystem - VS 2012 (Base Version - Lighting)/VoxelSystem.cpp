#include "VoxelSystem.h"
#include "TextureMgr.h"
#include "Effects.h"
#include "Camera.h"
#include <random>
#include <chrono>

VoxelSystem::VoxelSystem()
: mInitVB(0), mDrawVB(0), mStreamOutVB(0), mTexArraySRV(0), mRandomTexSRV(0)
{
	mFirstRun = true;
	mSwitch   = true;
	mGameTime = 0.0f;
	mTimeStep = 0.0f;
	mAge      = 0.0f;
	
	mUpdate = false;

	mEyePosW  = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);
}

VoxelSystem::~VoxelSystem()
{
	ReleaseCOM(mInitVB);
	ReleaseCOM(mDrawVB);
	ReleaseCOM(mStreamOutVB);
}

float VoxelSystem::GetAge()const
{
	return mAge;
}

void VoxelSystem::SetEyePos(const XMFLOAT3& eyePosW)
{
	mEyePosW = eyePosW;
}

void VoxelSystem::SetEmitPos(const XMFLOAT3& emitPosW)
{
	mEmitPosW = emitPosW;
}

void VoxelSystem::SetEmitDir(const XMFLOAT3& emitDirW)
{
	mEmitDirW = emitDirW;
}

int startVal = 1;

void VoxelSystem::Init(ID3D11Device* device, ParticleEffect* fx, ID3D11ShaderResourceView* texArraySRV, 
	                      ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles, const Camera& cam)
{
	man = ChunkManager(startVal);
	man.cameraFrustum.ExtractPlanes(cam.ViewProj());	// Update frustum in chunkManager

	mMaxParticles = maxParticles;

	mFX = fx;

	mTexArraySRV  = texArraySRV;
	mRandomTexSRV = randomTexSRV; 

	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	BuildVB(device);
}

void VoxelSystem::Reset()
{
	mFirstRun = true;
	if(mAge > 0.1f) mSwitch = !mSwitch;
	mAge      = 0.0f;
}

float keyPassed = 0.0f;

void VoxelSystem::Update(float dt, float gameTime)
{
	mGameTime = gameTime;
	mTimeStep = dt;

	mAge += dt;

	if(keyPassed > 0) 
		keyPassed -= dt;
}

int size = 0;

void VoxelSystem::UpdateBuffer(ID3D11Device* device, bool datasetChange)
{
	if(datasetChange)
		man.Update(mTimeStep, mEyePosW, mEyePosW);	// TODO
	else 
		man.UpdateAlt(mTimeStep, mEyePosW, mEyePosW);	// TODO
	size = man.m_renderList.size();

	if(man.m_renderList.size() != 0)
	{
		int tempsize = man.m_renderList.size();
	
		//
		// Create the buffer to kick-off the particle system.
		//

		vbd.ByteWidth = sizeof(Vertex::Voxel) * size;	// Update buffer desc size
		
		vinitData.pSysMem = &man.m_renderList[0];	// Pointer to the vector containing the data
	
		ReleaseCOM(mInitVB);

		HR(device->CreateBuffer(&vbd, &vinitData, &mInitVB));
	}
}

void VoxelSystem::ToggleValues(int key)
{
	if(keyPassed <= 0.0f)
	{
		if(key == 1)
		{
			man.ToggleFrustumCulling();
		}
		if(key == 2)
		{
			man.ToggleInactiveCells();
		}
		if(key == 3)
		{
			man.ToggleSurroundedCells();
		}

		if(key == 4)
		{
			man.ChangeDataset(0);
		}
		if(key == 5)
		{
			man.ChangeDataset(1);
		}
		if(key == 6)
		{
			man.ChangeDataset(2);
		}
		if(key == 7)
		{
			man.ChangeDataset(3);
		}
		if(key == 8)
		{
			man.ChangeDataset(4);
		}

		UpdateBuffer(g_device, true);

		keyPassed = 0.2f;
	}
}

void VoxelSystem::ToggleUpdate()
{
	if(keyPassed <= 0.0f)
	{
		mUpdate = !mUpdate;
		keyPassed = 0.2f;
	}
}

void VoxelSystem::SetFrustum(const Camera& cam)
{
	man.cameraFrustum.ExtractPlanes(cam.ViewProj());	// Update frustum in chunkManager
}

void VoxelSystem::Draw(ID3D11Device* device, ID3D11DeviceContext* dc, const Camera& cam)
{
	XMMATRIX VP = cam.ViewProj();

	man.cameraFrustum.ExtractPlanes(cam.ViewProj());	// Update frustum in chunkManager

	g_device = device;

	if(mUpdate)
		UpdateBuffer(device, false);

	//
	// Set constants.
	//
	mFX->SetViewProj(VP);
	mFX->SetGameTime(mGameTime);
	mFX->SetTimeStep(mTimeStep);
	mFX->SetEyePosW(mEyePosW);
	mFX->SetEmitPosW(mEmitPosW);
	mFX->SetEmitDirW(mEmitDirW);
	mFX->SetTexArray(mTexArraySRV);
	mFX->SetRandomTex(mRandomTexSRV);

	//
	// Set IA stage.
	//
	dc->IASetInputLayout(InputLayouts::Voxel);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(Vertex::Voxel);
    UINT offset = 0;

	// On the first pass, use the initialization VB. We only use this VB now
	
	dc->IASetVertexBuffers(0, 1, &mInitVB, &stride, &offset);

	//
	// Draw the current particle list using stream-out only to update them.  
	// The updated vertices are streamed-out to the target VB. 
	//
	dc->SOSetTargets(1, &mStreamOutVB, &offset);

    D3DX11_TECHNIQUE_DESC techDesc;
	mFX->StreamOutTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mFX->StreamOutTech->GetPassByIndex( p )->Apply(0, dc);
		
		dc->Draw(size, 0);
    }

	// done streaming-out--unbind the vertex buffer
	ID3D11Buffer* bufferArray[1] = {0};
	dc->SOSetTargets(1, bufferArray, &offset);

	// ping-pong the vertex buffers
	std::swap(mDrawVB, mStreamOutVB);

	//
	// Draw the updated particle system we just streamed-out. 
	//
	dc->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	mFX->DrawTech->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mFX->DrawTech->GetPassByIndex( p )->Apply(0, dc);
        
		dc->DrawAuto();
    }
}

std::string VoxelSystem::GetStatus()
{
	std::string text;

	if(man.dataSet == 0)
		text += std::string("Data Set (R)(t)(y)(u)(i) : Cubes\n");
	else if(man.dataSet == 1)
		text += std::string("Data Set (r)(T)(y)(u)(i) : Perlin\n");
	else if(man.dataSet == 2)
		text += std::string("Data Set (r)(t)(Y)(u)(i) : Simplex\n");
	else if(man.dataSet == 3)
		text += std::string("Data Set (r)(t)(y)(U)(i) : Custom\n");
	else if(man.dataSet == 4)
		text += std::string("Data Set (r)(t)(y)(u)(I) : Sphere\n");

	text += std::string("Number of Cubes Rendering : ") + std::to_string(size) + std::string("\n");

	if(man.cullFrustum)
		text += std::string("1 - Frustum Culling : On\n");
	else text += std::string("1 - Frustum Culling : Off\n");

	if(man.renderInactive)
		text += std::string("2 - Render Inactive Voxels : On\n");
	else text += std::string("2 - Render Inactive Voxels : Off\n");

	if(man.renderSurrounded)
		text += std::string("3 - Surrounded Voxels : On\n");
	else text += std::string("3 - Surrounded Voxels : Off\n");
	text += std::string("Camera Pos :" ) + std::to_string((int)mEyePosW.x) + std::string(", " ) + std::to_string((int)mEyePosW.y) + std::string(", ") + std::to_string((int)mEyePosW.z);

	//std::wstring ws;
	//ws.assign(text.begin(), text.end());

	return text;
}

void VoxelSystem::BuildVB(ID3D11Device* device)
{
	UpdateBuffer(device, true);
	/*
	man.Update(mTimeStep, mEyePosW, mEyePosW);	// TODO

	// Old way of doing it
	//auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	//std::default_random_engine e(millis.count());
	//std::uniform_int_distribution<int> distribution(0, 1);
	
	size = man.m_renderList.size();

	int tempsize = man.m_renderList.size();
	
	//
	// Create the buffer to kick-off the particle system.
	//

	vbd.ByteWidth = sizeof(Vertex::Voxel) * size;	// Update buffer Desc size

	//vinitData.pSysMem = &man.m_pChunkList[0];
	vinitData.pSysMem = &man.m_renderList[0];		// Pointer to the vector containing the data

	HR(device->CreateBuffer(&vbd, &vinitData, &mInitVB));*/

	//
	// Create the ping-pong buffers for stream-out and drawing.
	//
	vbd.ByteWidth = sizeof(Vertex::Voxel) * mMaxParticles;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	
    HR(device->CreateBuffer(&vbd, 0, &mDrawVB));
	HR(device->CreateBuffer(&vbd, 0, &mStreamOutVB));
}