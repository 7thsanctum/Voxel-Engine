#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "d3dUtil.h"
#include "Vertex.h"
#include "ChunkManager.h"
#include <string>
#include <vector>

class Camera;
class ParticleEffect;

class VoxelSystem
{
public:
	
	VoxelSystem();
	~VoxelSystem();

	// Time elapsed since the system was reset.
	float GetAge()const;
	int GetNum()const;

	void SetEyePos(const XMFLOAT3& eyePosW);
	void SetEmitPos(const XMFLOAT3& emitPosW);
	void SetEmitDir(const XMFLOAT3& emitDirW);

	void Init(ID3D11Device* device, ParticleEffect* fx, 
		ID3D11ShaderResourceView* texArraySRV, 
		ID3D11ShaderResourceView* randomTexSRV, 
		UINT maxParticles, const Camera& cam);
	
	void ToggleUpdate();
	void ToggleValues(int key);
	void SetFrustum(const Camera& cam);

	ID3D11Device* g_device;

	void Reset();
	void Update(float dt, float gameTime);
	void UpdateBuffer(ID3D11Device* device, bool datasetChange);
	void Draw(ID3D11Device* device, ID3D11DeviceContext* dc, const Camera& cam);

	std::string VoxelSystem::GetStatus();

private:
	void BuildVB(ID3D11Device* device);

	VoxelSystem(const VoxelSystem& rhs);
	VoxelSystem& operator=(const VoxelSystem& rhs);
 
private:
 
	ChunkManager man;

	D3D11_BUFFER_DESC vbd;	// For changing/updating buffers
	D3D11_SUBRESOURCE_DATA vinitData;

	UINT mMaxParticles;
	bool mFirstRun;
	bool mSwitch;
	bool mUpdate;

	float mGameTime;
	float mTimeStep;
	float mAge;

	XMFLOAT3 mEyePosW;
	XMFLOAT3 mEmitPosW;
	XMFLOAT3 mEmitDirW;

	ParticleEffect* mFX;

	ID3D11Buffer* mInitVB;	
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;
 
	ID3D11ShaderResourceView* mTexArraySRV;
	ID3D11ShaderResourceView* mRandomTexSRV;
};

#endif // PARTICLE_SYSTEM_H