#include "Vertex.h"
#include <random>
#include <chrono>
#include <noise/noise.h>
#include "noiseutils.h"

class Chunk
{
public:
    Chunk();
    ~Chunk();

    void Update(float dt);

    void Render();

	void Load();
	void Load_Model();
	void Unload();
	bool IsLoaded();
	void Setup();
	void Setup_Sphere(int numChunks);
	void Setup_Landscape(float offset);
	void HeightMapLandscape(float offset, int numChunks, int x, int y, int z);
	void SimplexLandscape(float offset, int numChunks, int x, int y, int z);
	void CustomLandscape(float offset, int numChunks, int x, int y, int z);
	void Model(std::vector<std::vector<int>>& model, int offset, int numChunks, int x, int y, int z);
	void Setup(int type);
	bool IsSetup();

	Vertex::Voxel*** GetData();
	Vertex::Voxel* m_pData;
    Vertex::Voxel*** m_pBlocks;    // The blocks data

	noise::module::Perlin myModule;
	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;

	bool ShouldRender();
	XMFLOAT3 mPosition;
	static const int CHUNK_SIZE = 16;
    //static const int CHUNK_SIZE = 224; // MAX tested size 224

private:
	bool setup;
	bool shouldRender;
	bool loaded;

	std::default_random_engine e;
	std::uniform_int_distribution<int> distribution;	
};