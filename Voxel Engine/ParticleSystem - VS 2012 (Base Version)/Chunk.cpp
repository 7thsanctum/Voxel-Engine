#include "Chunk.h"
#include "Simplex.h"
#include <fstream>

Chunk::Chunk()
{
    // Create the blocks
    m_pBlocks = new Vertex::Voxel**[CHUNK_SIZE];
    for(int i = 0; i < CHUNK_SIZE; i++)
    {
        m_pBlocks[i] = new Vertex::Voxel*[CHUNK_SIZE];

        for(int j = 0; j < CHUNK_SIZE; j++)
        {
            m_pBlocks[i][j] = new Vertex::Voxel[CHUNK_SIZE];
        }
    }

	m_pData = new Vertex::Voxel[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];

	heightMapBuilder.SetSourceModule (myModule);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (CHUNK_SIZE, CHUNK_SIZE);

	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	long long time = 100000L;
	e = std::default_random_engine(time);
	
	shouldRender = false;

	distribution = std::uniform_int_distribution<int>(1, 2);
}

Chunk::~Chunk()
{
    // Delete the blocks
    for (int i = 0; i < CHUNK_SIZE; ++i)
    {
        for (int j = 0; j < CHUNK_SIZE; ++j)
        {
            delete [] m_pBlocks[i][j];
        }

        delete [] m_pBlocks[i];
    }
    delete [] m_pBlocks;
}

void Chunk::Load()
{
	loaded = true;	// TODO: Actually have this set somewhere or related to a variable
}

void Chunk::Unload()
{
	loaded = false;	// TODO: Actually have this set somewhere or related to a variable
}

bool Chunk::IsLoaded()
{	
	return true;	// TODO: Actually have this set somewhere or related to a variable
	//return loaded;
}

void Chunk::Setup()
{	
    for(int i = 0; i < CHUNK_SIZE; i++)
    {
        for(int j = 0; j < CHUNK_SIZE; j++)
        {
			for(int k = 0; k < CHUNK_SIZE; k++)
			{
				//m_pBlocks[i][j][k] = new Vertex::Voxel[CHUNK_SIZE];
				ZeroMemory(&m_pBlocks[i][j][k], sizeof(Vertex::Voxel));
				m_pBlocks[i][j][k].Age  = 0.0f;
				m_pBlocks[i][j][k].InitialPos = XMFLOAT3((i + mPosition.x), (j + mPosition.y), (k + mPosition.z));
				m_pBlocks[i][j][k].Type = distribution(e); 
				shouldRender = true;
			}
        }
	}
	
	setup = true;
}

void Chunk::Setup_Sphere(int numChunks)
{	
    for(int i = 0; i < CHUNK_SIZE; i++)
    {
        for(int j = 0; j < CHUNK_SIZE; j++)
        {
			for(int k = 0; k < CHUNK_SIZE; k++)
			{
				
				//m_pBlocks[i][j][k] = new Vertex::Voxel[CHUNK_SIZE];
				ZeroMemory(&m_pBlocks[i][j][k], sizeof(Vertex::Voxel));
				m_pBlocks[i][j][k].Age  = 0.0f;
				m_pBlocks[i][j][k].InitialPos = XMFLOAT3((i + mPosition.x), (j + mPosition.y), (k + mPosition.z));
				//m_pBlocks[i][j][k].Size = XMFLOAT2(1.0f,1.0f);
				if (sqrt((float)  (i + mPosition.x-CHUNK_SIZE*numChunks/2)*(i + mPosition.x-CHUNK_SIZE*numChunks/2) 
								+ (j + mPosition.y-CHUNK_SIZE*numChunks/2)*(j + mPosition.y-CHUNK_SIZE*numChunks/2) 
								+ (k + mPosition.z-CHUNK_SIZE*numChunks/2)*(k + mPosition.z-CHUNK_SIZE*numChunks/2)) <= CHUNK_SIZE*numChunks/2)
				{
					m_pBlocks[i][j][k].Type = distribution(e); 
					shouldRender = true;
				}
				else m_pBlocks[i][j][k].Type = 0;
			}
        }
	}
	
	setup = true;
}

void Chunk::Setup_Landscape(float offset)
{
	//heightMapBuilder.SetBounds (2.0 + mPosition.x, 6.0, 1.0+ mPosition.y, 5.0);
	heightMapBuilder.SetBounds (2.0 + offset, 6.0 + offset, 1.0, 5.0);
	heightMapBuilder.Build ();
	
	/* // Debugging, Renders texture to file
	utils::RendererImage renderer;
	utils::Image image;
	renderer.SetSourceNoiseMap (heightMap);
	renderer.SetDestImage (image);
	renderer.Render ();

	utils::WriterBMP writer;
	writer.SetSourceImage (image);
	writer.SetDestFilename ("tutorial.bmp");
	writer.WriteDestFile ();
	*/
   // Texture* heightTexture = m_pRenderer->GetTexture(m_pChunkManager->GetHeightMapTexture());

    for(int i = 0; i < CHUNK_SIZE; i++)
    {
        for(int k = 0; k < CHUNK_SIZE; k++)
        {
            // Use the noise library to get the height value of x, z
            //float height = m_pChunkManager->GetNoiseValue(x, z);

            // Use the height map texture to get the height value of x, z
			double value =  myModule.GetValue (i, k, 0.50) + 5 ;
			double height = (heightMap.GetValue(i, k)+1.2)*10;
			//double height = (heightMap.GetValue(i, k) * (CHUNK_SIZE-1) * 1.0f) / 2.0f;

            for (int j = 0; j < CHUNK_SIZE * 4; j++)
            {
				//m_pBlocks[i][j][k] = new Vertex::Voxel[CHUNK_SIZE];
				ZeroMemory(&m_pBlocks[i][j][k], sizeof(Vertex::Voxel));
				m_pBlocks[i][j][k].Age  = 0.0f;
				m_pBlocks[i][j][k].InitialPos = XMFLOAT3((i + mPosition.x), (j + mPosition.y), (k + mPosition.z));
				if (j < height)
				{
					m_pBlocks[i][j][k].Type = 1;
					shouldRender = true;
				}
				else 
					m_pBlocks[i][j][k].Type = 0;
            }
        }
    }
}

void Chunk::HeightMapLandscape(float offset, int numChunks, int x, int y, int z)
{
	//heightMapBuilder.SetBounds (2.0 + mPosition.x, 6.0, 1.0+ mPosition.y, 5.0);
	float min = 0.5f;
	float max = 0.6f;
	float spacing = max - min;
	heightMapBuilder.SetBounds (min + offset + x * spacing, max + offset  + x * spacing, min + z * spacing , max + z * spacing);
	heightMapBuilder.Build ();

	for(int i = 0; i < CHUNK_SIZE; i++)
	{
		for(int k = 0; k < CHUNK_SIZE; k++)
		{
			// Use the noise library to get the height value of x, z
			//float height = m_pChunkManager->GetNoiseValue(x, z);

			// Use the height map texture to get the height value of x, z
			double value =  myModule.GetValue (i, k, 0.50) + 5 ;
			double height = (heightMap.GetValue(i, k) + numChunks/4)*numChunks ;
			//double height = (heightMap.GetValue(i, k) * (CHUNK_SIZE-1) * 1.0f) / 2.0f;

			for (int j = 0; j < CHUNK_SIZE; j++)
			{
				m_pBlocks[i][j][k].Age  = 0.0f;
				m_pBlocks[i][j][k].InitialPos = XMFLOAT3((i + mPosition.x), (j + mPosition.y), (k + mPosition.z));
				if ( j + (y * Chunk::CHUNK_SIZE) < height)
				{
					m_pBlocks[i][j][k].Type = 1; 
					shouldRender = true;
				}
				else m_pBlocks[i][j][k].Type = 0;
			}
		}
	}	

	for(int i = 0; i < CHUNK_SIZE; i++)
	{
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			for(int k = 0; k < CHUNK_SIZE; k++)
			{
				/*if(j + 1 > numChunks*CHUNK_SIZE || (j + 1 < CHUNK_SIZE) && m_pBlocks[i][j+1][k].Type == 0)
				{
					m_pBlocks[i][j][k].Type = 1;
				}*/

				if(m_pBlocks[i][j][k].Type == 2)
				{
					m_pBlocks[i][j][k].Type = 1;
				}

				if( j + 1 > numChunks*CHUNK_SIZE || ((j + 1 < CHUNK_SIZE) && m_pBlocks[i][j+1][k].Type == 0) && m_pBlocks[i][j][k].Type == 1)
				{
					m_pBlocks[i][j][k].Type = 2;
				}
			}
		}
	}
}

void Chunk::SimplexLandscape(float offset, int numChunks, int x, int y, int z)
{
	//heightMapBuilder.SetBounds (2.0 + mPosition.x, 6.0, 1.0+ mPosition.y, 5.0);
	heightMapBuilder.SetBounds (0.5 + offset + x * 0.5, 1.0 + offset + x * 0.5, 0.5 + z * 0.5 , 1.0 + z * 0.5);
	heightMapBuilder.Build ();
	
	for(int i = 0; i < CHUNK_SIZE; i++)
	{
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			for(int k = 0; k < CHUNK_SIZE; k++)
			{
				float xf = float(i + (x*CHUNK_SIZE))/(CHUNK_SIZE*numChunks);
				float yf = float(j + (y*CHUNK_SIZE))/(CHUNK_SIZE*numChunks);
				float zf = float(k + (z*CHUNK_SIZE))/(CHUNK_SIZE*numChunks);
				float value = simplex_noise(1, xf*3, yf*3, zf*3);

				m_pBlocks[i][j][k].Age  = 0.0f;
				m_pBlocks[i][j][k].InitialPos = XMFLOAT3((i + mPosition.x), (j + mPosition.y), (k + mPosition.z));
				if ( value > 1.1f)
				{
					m_pBlocks[i][j][k].Type = distribution(e); 
					shouldRender = true;
				}
				else m_pBlocks[i][j][k].Type = 0;
			}
		}
	}

}

void Chunk::CustomLandscape(float offset, int numChunks, int x, int y, int z)
{
	int numVoxels = 0;
	float caves, center_falloff, plateau_falloff, density;

	//heightMapBuilder.SetBounds (2.0 + mPosition.x, 6.0, 1.0+ mPosition.y, 5.0);
	heightMapBuilder.SetBounds (0.5 + offset + x * 0.5, 1.0 + offset + x * 0.5, 0.5 + z * 0.5 , 1.0 + z * 0.5);
	heightMapBuilder.Build ();
	
	for(int i = 0; i < CHUNK_SIZE; i++)
	{
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			for(int k = 0; k < CHUNK_SIZE; k++)
			{
				float xf = float(i + (x*CHUNK_SIZE))/(CHUNK_SIZE*numChunks);
				float yf = float(j + (y*CHUNK_SIZE))/(CHUNK_SIZE*numChunks);
				float zf = float(k + (z*CHUNK_SIZE))/(CHUNK_SIZE*numChunks);
				float value = simplex_noise(1, xf*3, yf*3, zf*3);

				if(yf <= 0.8)
				{
					plateau_falloff = 1.0;
				}
				else if(0.8 < yf && yf < 0.9)
				{
					plateau_falloff = 1.0-(yf-0.8)*10.0;
				}
				else
				{
					plateau_falloff = 0.0;
				}

				center_falloff = 0.1/(
										pow((xf-0.5)*1.5, 2) +
										pow((yf-1.0)*0.8, 2) +
										pow((zf-0.5)*1.5, 2) );
				caves = pow(simplex_noise(1, xf*5, yf*5, zf*5), 3);
				density = (
					simplex_noise(5, xf, yf*0.5, zf) *
					center_falloff *
					plateau_falloff );
				density *= pow(
					pnoise((xf+1)*3.0, (yf+1)*3.0, (zf+1)*3.0)+0.4, 1.8
				);
				if(caves < 0.5)
				{
					density = 0;
				}
				
				if(density > 3.1)
				{
					m_pBlocks[i][j][k].Type = distribution(e);
					shouldRender = true;
				}
				else
					m_pBlocks[i][j][k].Type = 0; 
			}
		}
	}
	
	for(int i = 0; i < CHUNK_SIZE; i++)
	{
		for (int j = 0; j < CHUNK_SIZE; j++)
		{
			for(int k = 0; k < CHUNK_SIZE; k++)
			{
				/*if(j + 1 > numChunks*CHUNK_SIZE || (j + 1 < CHUNK_SIZE) && m_pBlocks[i][j+1][k].Type == 0)
				{
					m_pBlocks[i][j][k].Type = 1;
				}*/

				if(m_pBlocks[i][j][k].Type == 2)
				{
					m_pBlocks[i][j][k].Type = 1;
				}

				if( j + 1 > numChunks*CHUNK_SIZE || ((j + 1 < CHUNK_SIZE) && m_pBlocks[i][j+1][k].Type == 0) && m_pBlocks[i][j][k].Type == 1)
				{
					m_pBlocks[i][j][k].Type = 2;
				}
			}
		}
	}
}

void Chunk::Model(std::vector<std::vector<int>>& model, int offset, int numChunks, int x, int y, int z)
{
	for(int i = 0; i < Chunk::CHUNK_SIZE; i++)
	{
		for(int j = 0; j < Chunk::CHUNK_SIZE; j++)
		{
			for(int k = 0; k < Chunk::CHUNK_SIZE; k++)
			{
				ZeroMemory(&m_pBlocks[i][j][k], sizeof(Vertex::Voxel));
				m_pBlocks[i][j][k].Age  = 0.0f;
				m_pBlocks[i][j][k].InitialPos = XMFLOAT3((i + mPosition.x), (j + mPosition.y), (k + mPosition.z));
				int start = (x * 16);
				if((x * 16) + i >= model.size())
				{
					m_pBlocks[i][j][k].Type  = 0;
				}
				else 
				{
					int value = (model[(x * 16) + i][(y * 16) + (z * 16) + j * 16 + k]);

					if(value != 0)
					{
						value = value;
					}

					if(value < 100)
					{
						value = value;
					}

					if(value > 1000)
					{
						value = 0;
					}

					m_pBlocks[i][j][k].Type = value;
					
				}
			}
		}
	}
	
	loaded = true;
	setup = true;
	shouldRender = true;
}

void Chunk::Setup(int type)
{	
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	std::default_random_engine e(millis.count());
	std::uniform_int_distribution<int> distribution(1, 2);

    for(int i = 0; i < CHUNK_SIZE; i++)
    {
        for(int j = 0; j < CHUNK_SIZE; j++)
        {
			for(int k = 0; k < CHUNK_SIZE; k++)
			{
				//m_pBlocks[i][j][k] = new Vertex::Voxel[CHUNK_SIZE];
				ZeroMemory(&m_pBlocks[i][j][k], sizeof(Vertex::Voxel));
				m_pBlocks[i][j][k].Age  = 0.0f;
				m_pBlocks[i][j][k].InitialPos = XMFLOAT3((i + mPosition.x), (j + mPosition.y), (k + mPosition.z));
				//m_pBlocks[i][j][k].Size = XMFLOAT2(1.0f,1.0f);
				m_pBlocks[i][j][k].Type = distribution(e); 
			}
        }
	}
	shouldRender = true;
	setup = true;
}

void Chunk::Load_Model()
{
	// TODO: Load model
}

Vertex::Voxel*** Chunk::GetData()
{
	return m_pBlocks;
}

bool Chunk::IsSetup()
{
	return setup;
}

bool Chunk::ShouldRender()
{	
	return shouldRender;	// TODO: Actually have this set somewhere or related to a variable
}