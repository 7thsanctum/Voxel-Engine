#include "ChunkManager.h"
Chunk* temp;
/*
ChunkManager::ChunkManager()
{
	temp = new Chunk[2];
	temp[0].mPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	temp[0].Setup();
	m_vpChunkVisibilityList.push_back(&temp[0]);
	temp[1].mPosition = XMFLOAT3(16.0f, 0.0f, 0.0f);
	temp[1].Setup();
	m_vpChunkVisibilityList.push_back(&temp[1]);
}
*/

ChunkManager::ChunkManager()
{
	numberofChunks = 0;
}

// TODO: Parallise this class to decrease loading/processing times

ChunkManager::ChunkManager(int numChunks)
{
	//LoadModel();

	numberofChunks = numChunks;
	temp = new Chunk[numChunks*numChunks*numChunks];

	int x= 0;
	int y= -1;
	int z= -1;
	
	for(int i = 0; i < numChunks*numChunks*numChunks; i++)
	{
		if(x % numChunks == 0)
		{
			x = 0;
			y += 1;
			if(y % numChunks == 0)
			{
				y = 0;
				z += 1;
			}
			if(z % numChunks == 0)
				z = 0;
		}
		temp[i].mPosition = XMFLOAT3(Chunk::CHUNK_SIZE * x, Chunk::CHUNK_SIZE * y, Chunk::CHUNK_SIZE * z);
		//float minx, float miny, float minz, float maxx, float maxy, float maxz);
		temp[i].Setup_Sphere(numChunks);
		//temp[i].Model(model, 0, numberofChunks, x, y, z);
		//temp[i].Setup_Sphere(numChunks);
		//temp[i].Setup(x);
		m_vpChunkVisibilityList.push_back(&temp[i]);
		x += 1;
	}

	cullFrustum = false;
	renderInactive = false;
	renderSurrounded = false;

	dataSet = 0;

}

ChunkManager::~ChunkManager()
{

}

void ChunkManager::ToggleFrustumCulling()
{
	cullFrustum = !cullFrustum;
}

void ChunkManager::ToggleInactiveCells()
{
	renderInactive = !renderInactive;
}

void ChunkManager::ToggleSurroundedCells()
{
	renderSurrounded = !renderSurrounded;
}

float iterations = 0;

void ChunkManager::UpdateAlt(float dt, XMFLOAT3 cameraPosition, XMFLOAT3 cameraView)
{
	UpdateRenderList();

    m_cameraPosition = cameraPosition;
    m_cameraView = cameraView;
}

//TODO: This update should only be used for changing dataset, need to add a seperate update that runs everytime
// but still maintains the data set
void ChunkManager::Update(float dt, XMFLOAT3 cameraPosition, XMFLOAT3 cameraView)
{
    //UpdateAsyncChunker();

    UpdateLoadList();

    UpdateSetupList();

    //UpdateRebuildList();

    //UpdateFlagsList();

    UpdateUnloadList();

    //UpdateVisibilityList(cameraPosition);
	
	//if((m_cameraPosition.x != cameraPosition.x || m_cameraPosition.y != cameraPosition.y || m_cameraPosition.z != cameraPosition.z ) 
	//	|| (m_cameraView.x != cameraView.x || m_cameraView.y != cameraView.y || m_cameraView.z != cameraView.z))
    {
		iterations += 0.025f;

		int x= 0;
		int y= -1;
		int z= -1;
		m_vpChunkVisibilityList.clear();
		for(int i = 0; i < numberofChunks*numberofChunks*numberofChunks; i++)
		{
			if(x % numberofChunks == 0)
			{
				x = 0;
				y += 1;
				if(y % numberofChunks == 0)
				{
					y = 0;
					z += 1;
				}
				if(z % numberofChunks == 0)
					z = 0;
			}
			//temp[i].mPosition = XMFLOAT3((Chunk::CHUNK_SIZE * x), Chunk::CHUNK_SIZE * y, Chunk::CHUNK_SIZE * z);
			//temp[i].Model(model, iterations, numberofChunks, x, y, z);	
			
			if(dataSet == 1)
				temp[i].HeightMapLandscape(iterations, numberofChunks, x, y, z);				
			else if(dataSet == 2)
				temp[i].SimplexLandscape(iterations, numberofChunks, x, y, z);
			else if(dataSet == 3)
				temp[i].CustomLandscape(iterations, numberofChunks, x, y, z);
			else if(dataSet == 4)
				temp[i].Setup_Sphere(numberofChunks);
			else 
				temp[i].Setup(1);
				
			//temp[i].Update_Landscape(iterations);
			m_vpChunkVisibilityList.push_back(&temp[i]);

			x += 1;
		}
        UpdateRenderList();
    }

    m_cameraPosition = cameraPosition;
    m_cameraView = cameraView;
}

void ChunkManager::ChangeDataset(int setVal)
{
	dataSet = setVal;
}

void ChunkManager::LoadModel()
{
	for(int j = 1; j < 110; j++)
	{
		std::streampos size;
		char * memblock;
		std::string add = std::string("MRbrain/MRbrain.") + std::to_string(j);
		//std::string add = std::string("mouse0.raw");
		//if(j >= 10) add += std::to_string(j) + std::string(".tif");
		//else add += std::string("0") + std::to_string(j) + std::string(".tif");
		//std::ifstream file (add, std::ios::in|std::ios::binary|std::ios::ate);
		std::ifstream file (add, std::ios::in|std::ios::binary|std::ios::ate);
		if (file.is_open())
		{
			size = file.tellg();
			memblock = new char [size];
			file.seekg (0, std::ios::beg);
			file.read (memblock, size);
			file.close();
		}

		model.push_back(std::vector<int>());

		int val = 0;
		for(int i = 0; i < size; i += 2)
		{
			//if(i/2 % 22500 == 0 && i != 0)
			//if(i % 65536 == 0 && i != 0)
			//{
			//	model.push_back(std::vector<int>());
			//	j++;
			//}
			val = 0;
			char c = memblock[i];
			char c2 = memblock[i + 1];
			val = (int)memblock[i + 1] + ((int)memblock[i] << 8) ;
			model[j-1].push_back(val);
		}

		delete[] memblock;
	}
}

void ChunkManager::UpdateLoadList()
{
    int lNumOfChunksLoaded = 0;
	
	std::vector<Chunk*>::iterator iterator;
    for(iterator = m_vpChunkLoadList.begin(); iterator != m_vpChunkLoadList.end() &&
                  (lNumOfChunksLoaded != ASYNC_NUM_CHUNKS_PER_FRAME); ++iterator)
    {
        Chunk* pChunk = (*iterator);

        if(pChunk->IsLoaded() == false)
        {
            if(lNumOfChunksLoaded != ASYNC_NUM_CHUNKS_PER_FRAME)
            {
                pChunk->Load();

                // Increase the chunks loaded count
                lNumOfChunksLoaded++;

                m_forceVisibilityUpdate = true;
            }
        }
    }

    // Clear the load list (every frame)
    m_vpChunkLoadList.clear();
}

void ChunkManager::UpdateUnloadList()
{
    // Unload any chunks
	std::vector<Chunk*>::iterator iterator;
    for(iterator = m_vpChunkUnloadList.begin(); iterator != m_vpChunkUnloadList.end(); ++iterator)
    {
        Chunk* pChunk = (*iterator);

        if(pChunk->IsLoaded())
        {
            pChunk->Unload();

            m_forceVisibilityUpdate = true;
        }
    }

    // Clear the unload list (every frame)
    m_vpChunkUnloadList.clear();
}

void ChunkManager::UpdateSetupList()
{
    // Setup any chunks that have not already been setup
    std::vector<Chunk*>::iterator iterator;
    for(iterator = m_vpChunkSetupList.begin(); iterator != m_vpChunkSetupList.end(); ++iterator)
    {
        Chunk* pChunk = (*iterator);

        if(pChunk->IsLoaded() && pChunk->IsSetup() == false)
        {
            pChunk->Setup();

            if(pChunk->IsSetup())
            {
                // Only force the visibility update if we actually setup the chunk, some chunks wait in the pre-setup stage...
                m_forceVisibilityUpdate = true;
            }
        }
    }

    // Clear the setup list (every frame)
    m_vpChunkSetupList.clear();
}

bool  ChunkManager::IsVoxelSurrounded(Chunk* pChunk, std::vector<Chunk*>::iterator currentIterator, int x, int y, int z)
{
	std::vector<Chunk*>::iterator nextChunk;
	Chunk* nChunk;

	if(renderSurrounded) // Overrides the check to force rendering of surrounded voxels
	{
		return true;
	}

	// If voxel is at the edge of a chunk, we might want to render it, but we need to first 
	// check neighbouring voxels
	if(x - 1 < 0)
	{
		if(pChunk->mPosition.x > 0)
		{
			nextChunk = currentIterator - 1;	// Chunk next on X axis Square, TODO: Verify direction
			nChunk = (*nextChunk);
			if(nChunk->m_pBlocks[0][y][z].Type == 0)
			{
				return true;
			}
		}	
		else return true;
	}
	else 
	{
		if(pChunk->m_pBlocks[x-1][y][z].Type == 0)
		{
			return true;
		}
	}

	if(x + 1 >= Chunk::CHUNK_SIZE)
	{
		if(pChunk->mPosition.x <  (Chunk::CHUNK_SIZE * numberofChunks) - Chunk::CHUNK_SIZE)
		{
			nextChunk = currentIterator + 1;	// Chunk next on X axis Square, TODO: Verify direction
			nChunk = (*nextChunk);
			if(nChunk->m_pBlocks[Chunk::CHUNK_SIZE - 1 ][y][z].Type == 0)	// TODO: this might have to be Chunk::CHUNK_SIZE - 1 
			{
				return true;
			}
		}	
		else return true;
	}
	else 
	{
		if(pChunk->m_pBlocks[x+1][y][z].Type == 0)
		{
			return true;
		}
	}

	if(y - 1 < 0)
	{
		if(pChunk->mPosition.y > 0)
		{
			nextChunk = currentIterator - (numberofChunks);	// Chunk next on Y axis Square, TODO: Verify direction
			nChunk = (*nextChunk);
			if(nChunk->m_pBlocks[x][0][z].Type == 0)
			{
				return true;
			}
		}	
		else return true;
	}
	else 
	{
		if(pChunk->m_pBlocks[x][y-1][z].Type == 0)
		{
			return true;
		}
	}

	if(y + 1 >= Chunk::CHUNK_SIZE)
	{
		if(pChunk->mPosition.y <  (Chunk::CHUNK_SIZE * numberofChunks) - Chunk::CHUNK_SIZE)
		{
			nextChunk = currentIterator + (numberofChunks);	// Chunk next on Y axis Square, TODO: Verify direction
			nChunk = (*nextChunk);
			if(nChunk->m_pBlocks[x][Chunk::CHUNK_SIZE - 1][z].Type == 0)
			{
				return true;
			}
		}	
		else return true;
	}
	else 
	{
		if(pChunk->m_pBlocks[x][y+1][z].Type == 0)
		{
			return true;
		}
	}

	if(z - 1 < 0)
	{
		if(pChunk->mPosition.z > 0)
		{
			nextChunk = currentIterator - (numberofChunks * numberofChunks);	// Chunk next on Y axis Square, TODO: Verify direction
			nChunk = (*nextChunk);
			if(nChunk->m_pBlocks[x][y][0].Type == 0)
			{
				return true;
			}
		}	
		else return true;
	}
	else 
	{
		if(pChunk->m_pBlocks[x][y][z-1].Type == 0)
		{
			return true;
		}
	}

	if(z + 1 >= Chunk::CHUNK_SIZE)
	{
		if(pChunk->mPosition.z <  (Chunk::CHUNK_SIZE * numberofChunks) - Chunk::CHUNK_SIZE)
		{
			nextChunk = currentIterator + (numberofChunks * numberofChunks);	// Chunk next on Y axis Square, TODO: Verify direction
			nChunk = (*nextChunk);
			if(nChunk->m_pBlocks[x][y][Chunk::CHUNK_SIZE - 1].Type == 0)
			{
				return true;
			}
		}	
		else return true;
	}
	else 
	{
		if(pChunk->m_pBlocks[x][y][z+1].Type == 0)
		{
			return true;
		}
	}
	/*
	if(	z - 1 < 0  || z + 1 >= Chunk::CHUNK_SIZE)	
	{
		//std::vector<Chunk*>::iterator nextChunk = iterator + (numberofChunks * numberofChunks);	// Chunk next on Z axis Square, TODO: Verify direction
		//nextChunk = iterator - (numberofChunks * numberofChunks);	// Chunk next on Z axis Square, TODO: Verify direction
		//nextChunk = iterator + (numberofChunks);	// Chunk Above Y Square, TODO: Verify direction
		//nextChunk = iterator - (numberofChunks);	// Chunk Below Y Square, TODO: Verify direction
		//nextChunk = iterator + 1;	// Chunk next on X axis Square, TODO: Verify direction
		//
		//Chunk* nChunk = (*nextChunk);

		return true;
	}

	// If voxel has an exposed side we want to render it
	if(pChunk->m_pBlocks[x][y][z-1].Type == 0 || pChunk->m_pBlocks[x][y][z+1].Type == 0)
	{
		return true;
	}*/

	return false;
}

bool ChunkManager::IsVoxelVisible(Chunk* pChunk, std::vector<Chunk*>::iterator currentIterator, int x, int y, int z)
{
	if(!cullFrustum || cameraFrustum.PointInFrustum(pChunk->m_pBlocks[x][y][z].InitialPos) )
	{
		if(IsVoxelSurrounded(pChunk, currentIterator, x, y, z))
		{				
			if(renderInactive || pChunk->m_pBlocks[x][y][z].Type > 0)
			{
				return true;
			}			
		}		
	}


	return false;
}

void ChunkManager::UpdateRenderList()
{
    // Clear the render list each frame BEFORE we do our tests to see what chunks should be rendered
    //m_vpChunkRenderList.clear();
	m_renderList.clear();
    std::vector<Chunk*>::iterator iterator;
    for(iterator = m_vpChunkVisibilityList.begin(); iterator != m_vpChunkVisibilityList.end(); ++iterator)
    {
        Chunk* pChunk = (*iterator);

		if(pChunk->IsLoaded() && pChunk->IsSetup())
		{
			if(pChunk->ShouldRender()) // Early flags check so we don't always have to do the frustum check...
			{
					
				float c_offset = (Chunk::CHUNK_SIZE);

				XMFLOAT3 chunkCenter = XMFLOAT3(pChunk->mPosition.x + c_offset,
												pChunk->mPosition.y + c_offset,
												pChunk->mPosition.z + c_offset);

				// Check if this chunk is inside the camera frustum
				if(cameraFrustum.CubeInFrustum(chunkCenter, 8.0f, 8.0f, 8.0f) != 0)
				{
					// TODO: Calculate the position of the chunk next to this current one
					//		 also get the individual voxel so that way we can have more accurate filtering
					//std::vector<Chunk*>::iterator nextChunk = iterator + (numberofChunks * numberofChunks);	// Adjacent Square, TODO: Verify direction
					//std::vector<Chunk*>::iterator nextChunk = iterator + (numberofChunks * numberofChunks);	// Chunk next on Z axis Square, TODO: Verify direction
					//nextChunk = iterator - (numberofChunks * numberofChunks);	// Chunk next on Z axis Square, TODO: Verify direction
					//nextChunk = iterator + (numberofChunks);	// Chunk Above Y Square, TODO: Verify direction
					//nextChunk = iterator - (numberofChunks);	// Chunk Below Y Square, TODO: Verify direction
					//nextChunk = iterator + 1;	// Chunk next on X axis Square, TODO: Verify direction
					//nextChunk = iterator - 1;	// Chunk next on X axis Square, TODO: Verify direction
					//Chunk* nChunk = (*nextChunk);
					// ####################################################

					float c_size = Chunk::CHUNK_SIZE;

					//m_vpChunkRenderList.push_back(pChunk);
					int size = m_renderList.max_size();

					for(int x = 0; x < Chunk::CHUNK_SIZE; x++)
					{
						for(int y = 0; y < Chunk::CHUNK_SIZE; y++)
						{
							for(int z = 0; z < Chunk::CHUNK_SIZE; z++)
							{
								if(IsVoxelVisible(pChunk, iterator, x, y, z))
								{
									
									m_renderList.push_back(pChunk->m_pBlocks[x][y][z]);									
								}								
							}
						}
					}	// End of for loop
				}
			}			
		}
    }
}