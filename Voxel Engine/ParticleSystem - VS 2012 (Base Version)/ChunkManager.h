#include "Chunk.h"
#include "Frustum.h"
#include <vector>

class ChunkManager
{
public:
	ChunkManager();
	ChunkManager(int numChunks);
	~ChunkManager();

	void UpdateAlt(float deltaTime, XMFLOAT3 camPosition, XMFLOAT3 camView);
	void Update(float deltaTime, XMFLOAT3 camPosition, XMFLOAT3 camView);

	bool IsVoxelVisible(Chunk* pChunk, std::vector<Chunk*>::iterator currentIterator, int x, int y, int z);
	bool IsVoxelSurrounded(Chunk* pChunk, std::vector<Chunk*>::iterator currentIterator, int x, int y, int z);

	void ChunkManager::LoadModel();
	std::vector<std::vector<int>> model;

	//void UpdateAsyncChunker();
    void UpdateLoadList();
    void UpdateSetupList();
	//void UpdateRebuildList();
    //void UpdateFlagsList();
    void UpdateUnloadList();
    //void UpdateVisibilityList(XMFLOAT3 cameraPosition);

	void UpdateRenderList();

	bool cullFrustum;
	bool renderInactive;
	bool renderSurrounded;

	int dataSet;

	void ChangeDataset(int setVal);

	int numberofChunks;

	void ToggleFrustumCulling();
	void ToggleInactiveCells();
	void ToggleSurroundedCells();

	Frustum cameraFrustum;

	std::vector<Vertex::Voxel> m_renderList;
	std::vector<Chunk*> m_pChunkList;
private:
	bool m_forceVisibilityUpdate;

	int ASYNC_NUM_CHUNKS_PER_FRAME;
	XMFLOAT3 m_cameraPosition;
	XMFLOAT3 m_cameraView;
	
	std::vector<Chunk*> m_vpChunkLoadList;
	std::vector<Chunk*> m_vpChunkSetupList;
	std::vector<Chunk*> m_vpChunkUnloadList;
	std::vector<Chunk*> m_vpChunkVisibilityList;
	std::vector<Chunk*> m_vpChunkRenderList;
};