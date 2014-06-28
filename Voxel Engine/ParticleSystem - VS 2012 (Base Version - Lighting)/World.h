/*#pragma once
#include "Utility.h"
#include "Blocks.h"
#include "MeshChunk.h"
#include "Frustum.h"

typedef struct 
{
        int distance;
        int chunk;
} TransparentOrder;

class World
{
public:
        World();
        ~World();
        
        block_t& getBlock(const int x, const int y, const int z);
        block_t blockAtPoint(glm::vec3 pos);

        //create world
        void initWorld(int worldSize, int chunkSize);
        void setTextureSize(int texture, int chunk);

        void getBlockVerts(int i, Box* blockType);
        void buildBlcoksVerts();

        //chunks handling
        void createChunks(const int startX, const int startY, const int startZ);
        void rebuildChunk(int id);
        void rebuildNearestChunks(int id, glm::vec3 pos);
        void createWorldChunks();
        void setAllChunksToUpdate();

        void getNormalBlock(int x, int y, int z, int &iVertex, MeshChunk* meshChunk, block_t block, bool transparent);
        void LoadWord(const char* filename);

        void buildMap();
        void destroyMap();

        //render
        void drawWorld(Frustum &camFrustum, bool camUpdate);
        void drawCubes(int i);

        //picking
        int groundHeight(const int x, const int z);
        int getChunkId(glm::vec3 pos);
        bool blockEditable(const int x, const int y, const int z);
        bool isSolid(const int x, const int y, const int z);
        void addChunkToUpdate(const int x, const int y, const int z);

        int getDrawnTrianglesCount() { return drawnTriangles;}
        glm::vec3 getPlayersPos();
        void updatePlayersZoneBB(glm::vec3 playerPosition);
        int getBlockTypesCount();
        
        void getNormalBlockVerts(int i,Box *blockType);
        
        char name[50];
        int createdChunksCount;
        glm::vec3 fogColor;
        int worldVersion;

private:
        int WORLD_SIZE;
        int CHUNK_SIZE;

        int textureSize;
        int textureChunk;
        float percent;
        std::vector<MeshChunk*> mChunks;

        block_t* m_Blocks;

        //player aabb
    BoundingBox playerZoneBB;
        glm::vec3 playerZoneSize;
    glm::vec3 playerPos;
    int chunksCreatedInFrameCount;
    float updateChunkTimer;
    bool updateChunksSwitch;

         //information
     int drawnTriangles;

};
*/