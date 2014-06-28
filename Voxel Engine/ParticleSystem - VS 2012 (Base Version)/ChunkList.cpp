#include "Chunk.cpp"

class ChunkList
{
public:
    ChunkList();
    ~ChunkList();

    void Update(float dt);

    void Render();

    static const int ChunkList_SIZE = 16;

private:
    // The blocks data
    Chunk*** m_pBlocks;
};

ChunkList::ChunkList()
{
    // Create the blocks
    m_pBlocks = new Chunk**[ChunkList_SIZE];
    for(int i = 0; i < ChunkList_SIZE; i++)
    {
        m_pBlocks[i] = new Chunk*[ChunkList_SIZE];

        for(int j = 0; j < ChunkList_SIZE; j++)
        {
            m_pBlocks[i][j] = new Chunk[ChunkList_SIZE];
        }
    }
}

ChunkList::~ChunkList()
{
    // Delete the blocks
    for (int i = 0; i < ChunkList_SIZE; ++i)
    {
        for (int j = 0; j < ChunkList_SIZE; ++j)
        {
            delete [] m_pBlocks[i][j];
        }

        delete [] m_pBlocks[i];
    }
    delete [] m_pBlocks;
}