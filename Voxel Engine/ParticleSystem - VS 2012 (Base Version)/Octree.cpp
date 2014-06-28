#include "Octree.h"

Octree::Octree (XMFLOAT3 tmin, XMFLOAT3 tmax, int d) 
{
	min = tmin;
	max = tmax;
	center = XMFLOAT3(tmin.x/2 + tmax.x/2, tmin.y/2 + tmax.y/2, tmin.z/2 + tmax.z/2);
	depth = d;
	numVoxels = 0;
	hasChildren = false;
	CreateBuffersThisLeaf();
}

Octree::~Octree () 
{
	if(hasChildren) 
	{
		destroyChildren();
	}

	this->DeleteBuffersThisLeaf();
}

void Octree::fileVoxel(Vertex::Voxel* voxel, XMFLOAT3 pos, bool addVoxel)
{
	for(int x = 0; x < 2; x++)
	{
		if (x == 0) 
		{
			if (pos.x - 0.5 > center.x) 
			{
				continue;
			}
		}
		else if (pos.x + 0.5 < center.x) 
		{
			continue;
		}
			 
		for(int y = 0; y < 2; y++) 
		{
			if (y == 0) 
			{
				if (pos.y - 0.5 > center.y) 
				{
					continue;
				}
			}
			else if (pos.y + 0.5 < center.y) 
			{
				continue;
			}
                    
			for(int z = 0; z < 2; z++) 
			{
				if (z == 0) 
				{
					if (pos.z - 0.5 > center.z) 
					{
						continue;
					}
				}
				else if (pos.z + 0.5 < center.z) 
				{
					continue;
				}
                        
				//Add or remove the ball
				if (addVoxel) 
				{
					children[x][y][z]->add(voxel);
				}
				else 
				{
					children[x][y][z]->remove(voxel, pos);
				}
			}
		}
	}
}

void Octree::haveChildren()
	{
		for(int x = 0; x < 2; x++)
		{
			float minX;
			float maxX;
			if(x == 0)
			{
				minX = min.x;
				maxX = center.x;
			}else {
				minX = center.x;
				maxX = max.x;
			}

			for(int y = 0; y < 2; y++)
			{
				float minY;
				float maxY;

				if(y == 0) 
				{
					minY = min.y;
					maxY = center.y;
				}else {
					minY = center.y;
					maxY = max.y;
				}

				for(int z = 0; z < 2; z++)
				{
					float minZ;
					float maxZ;

					if(z == 0)
					{
						minZ = min.z;
						maxZ = center.z;
					}else {
						minZ = center.z;
						maxZ = max.z;
					}

					children[x][y][z] = new Octree (XMFLOAT3(minX, minY, minZ), 
													XMFLOAT3(maxX, maxY, maxZ),
													depth+1);

				}

			}

		}

		for(std::vector<Vertex::Voxel*>::iterator it = voxels.begin(); it != voxels.end(); it++)
		{
			Vertex::Voxel* p = *it;
			fileVoxel(p, p->InitialPos, true);
		}

		voxels.clear();
		hasChildren = true;

	}

void Octree::collectVoxels(std::vector<Vertex::Voxel*> &vs) {
		if(hasChildren) {
			for(int x =0; x<2; x++) {
				for(int y = 0; y < 2; y++) {
					for(int z = 0; z<2; z++) {
						children[x][y][z]->collectVoxels(vs);
					}
				}
			}
		}else {
			for(std::vector<Vertex::Voxel*>::iterator it = voxels.begin(); it !=voxels.end(); it++) {
				Vertex::Voxel* vo = *it;
				vs.push_back(vo);
			}
		}
	}

void Octree::destroyChildren() 
{
	collectVoxels(voxels);

	for(int x = 0; x<2; x++)
	{
		for(int y= 0; y<2; y++)
		{
			for(int z =0; z<2; z++)
			{
				children[x][y][z]->voxels.clear();
				children[x][y][z]->node_colors.clear();
				children[x][y][z]->node_indices.clear();
				children[x][y][z]->node_light.clear();
				children[x][y][z]->node_normal.clear();
				children[x][y][z]->node_shadow.clear();
				children[x][y][z]->node_uvs.clear();
				children[x][y][z]->node_vertices.clear();

				delete children[x][y][z];
			}
		}
	}

	hasChildren = false;

		
}

void Octree::remove(Vertex::Voxel* voxel, XMFLOAT3 pos)
{
	numVoxels --;
	if(hasChildren && numVoxels << MIN_VOXELS_PER_OCTREE) 
	{
		destroyChildren();
	}

	if(hasChildren) 
	{
		fileVoxel(voxel, pos, false);
	}
	else 
	{
		//TODO: hope it will work :S
		voxels.erase(std::find(voxels.begin(), voxels.end(), voxel));
	}
}


void Octree::add(Vertex::Voxel* voxel) 
{
	numVoxels++;

	if(!hasChildren && depth < MAX_OCTREE_DEPTH && numVoxels > MAX_VOXELS_PER_OCTREE) 
	{
		haveChildren();
	}

	if(hasChildren) 
	{
		fileVoxel(voxel, voxel->InitialPos, true);
	}
	else 
	{
		voxels.push_back(voxel);
	}

}

void Octree::remove(Vertex::Voxel* voxel)
{
	remove(voxel, voxel->InitialPos);
}

void Octree::voxelMove(Vertex::Voxel* voxel, XMFLOAT3 oldPos) 
{
	remove(voxel, oldPos);
	add(voxel);
}

void Octree::buffersPreparation(std::vector<XMFLOAT3>& tmpVert, std::vector<XMFLOAT2>& tmpUV, std::vector<XMFLOAT3>& tmpNorm) 
{
	if(this->hasChildren == false)
	{
		//howManyNeeded++;
		node_colors.clear();
		node_shadow.clear();
		node_light.clear();
		node_vertices.clear();
		node_uvs.clear();
		node_indices.clear();
		node_normal.clear();
			
		RecreateBuffer(tmpVert, tmpUV, tmpNorm);
	}
}

bool Octree::displayIt(Vertex::Voxel& v)
{
	if(v.Type == 0)
	{
		return false;
	}
		
	int tmpX = v.InitialPos.x;
	int tmpY = v.InitialPos.y;
	int tmpZ = v.InitialPos.z;

		
	int around = 0;
	int tp = v.InitialPos.x - 1; 
	if((tp) >= 0)
	{
		if(worldGrid[tp][(int)v.InitialPos.y][(int)v.InitialPos.z].Type != 0)
		{
			around++;
		}
	}

	tp = v.InitialPos.x + 1; 
	if(tp < CHUNK_SIZE)
	{
		if(worldGrid[tp][(int)v.InitialPos.y][(int)v.InitialPos.z].Type != 0 )
		{
			around++;
		}
	}

	tp = v.InitialPos.y - 1; 
	if(tp >= 0)
	{
		if(worldGrid[(int)v.InitialPos.x][tp][(int)v.InitialPos.z].Type != 0 )
		{
			around++;
		}
	}

	tp = v.InitialPos.y + 1; 
	if(tp < CHUNK_SIZE)
	{
		if(worldGrid[(int)v.InitialPos.x][tp][(int)v.InitialPos.z].Type != 0 )
		{
			around++;
		}
	}

	tp = v.InitialPos.z - 1; 
	if(tp >= 0)
	{
		if(worldGrid[(int)v.InitialPos.x][(int)v.InitialPos.y][tp].Type != 0 )
		{
			around++;
		}
	}

	tp = v.InitialPos.z + 1; 
	if(tp < CHUNK_SIZE)
	{
		if(worldGrid[(int)v.InitialPos.x][(int)v.InitialPos.y][tp].Type != 0 )
		{
			around++;
		}
	}


	if(around > 5)
	{
		v.Type = false;
		return false;
	}else {
		v.Type = true;

		return true;
		
	}
		
}

void Octree::RecreateBuffer(std::vector<XMFLOAT3>& tmpVert, std::vector<XMFLOAT2>& tmpUV, std::vector<XMFLOAT3>& tmpNorm)
{
	//printf("Buffer Creation for %f %f %f %d ..", this->min.x,  this->min.y , this->min.z, this->voxels.at(0)->Type);
		
	int block;
	XMFLOAT3 tmp;
	XMFLOAT2 p;
	std::vector<XMFLOAT3>	before_index_vert;
	std::vector<XMFLOAT2>	before_index_UV;
	std::vector<XMFLOAT3>	before_index_norm;
	std::vector<float>		before_index_shadow;
	std::vector<float>		before_index_light;
	std::vector<float>		before_index_types;

	for(int v = 0; v < this->voxels.size(); v++)
	{
		block = this->voxels[v]->Type;
						
		if(displayIt(*voxels[v]))
		{
			bool addToBuffer;
			for(int i = 0; i < tmpNorm.size(); i++)
			{		
				int testX = voxels.at(v)->InitialPos.x + tmpNorm.at(i).x;
				int testY = voxels.at(v)->InitialPos.y + tmpNorm.at(i).y;
				int testZ = voxels.at(v)->InitialPos.z + tmpNorm.at(i).z;

				bool test = false;
				int maxXX = (int)CHUNK_SIZE;
					
				if(testX >= 0 && testX < maxXX)
					test = true;
				else 
					test = false;

				if(test != false)
					if(testY >= 0 && testY < maxXX)
							test = true;
					else 
						test = false;

				if(test != false)
					if(testZ >= 0 && testZ < maxXX)
						test = true;
					else 
						test =false;

				addToBuffer = false;

				if(test == true)
				{
					if(::worldGrid[testX][testY][testZ].Type == 0 )
					{
						addToBuffer = true;
					}
				}

				if(testX > maxXX || testY > maxXX || testY > maxXX)
				{
					addToBuffer = true;
				}

				if(testX < 0 || testY <  0 || testY < 0)
				{
					addToBuffer = true;
				}


				if(addToBuffer)
				{
					tmp.x = tmpVert.at(i).x + voxels.at(v)->InitialPos.x;
					tmp.y = tmpVert.at(i).y + voxels.at(v)->InitialPos.y;
					tmp.z = tmpVert.at(i).z + voxels.at(v)->InitialPos.z;
							
					before_index_types.push_back(block);
					before_index_vert.push_back(tmp);
					before_index_norm.push_back(tmpNorm.at(i));
					before_index_shadow.push_back(0);
					before_index_light.push_back(0);

							
					if(block == 0)
					{
						p = tmpUV.at(i);
						p.x = p.x/sqrt((double)ATLAS_ELEMENTS);
						p.y = p.y/sqrt((double)ATLAS_ELEMENTS);
						before_index_UV.push_back(p);
					}
					else if(block == 1)
					{
						XMFLOAT2 p = tmpUV.at(i);
						p.x = p.x/sqrt((double)ATLAS_ELEMENTS) +sqrt((double)ATLAS_ELEMENTS)/ATLAS_ELEMENTS;
						p.y = p.y/sqrt((double)ATLAS_ELEMENTS);
						before_index_UV.push_back(p);


					}
					else if(block == 2)
					{
						XMFLOAT2 p = tmpUV.at(i);
						p.x = p.x/sqrt((double)ATLAS_ELEMENTS);
						p.y = p.y/sqrt((double)ATLAS_ELEMENTS)+ sqrt((double)ATLAS_ELEMENTS)/ATLAS_ELEMENTS;
						before_index_UV.push_back(p);

					}
					else if(block == 3)
					{
						XMFLOAT2 p = tmpUV.at(i);
						p.x = p.x/sqrt((double)ATLAS_ELEMENTS) + sqrt((double)ATLAS_ELEMENTS)/ATLAS_ELEMENTS;
						p.y = p.y/sqrt((double)ATLAS_ELEMENTS) + sqrt((double)ATLAS_ELEMENTS)/ATLAS_ELEMENTS;
						before_index_UV.push_back(p);
					}
					else if(block == 4)
					{
						XMFLOAT2 p = tmpUV.at(i);
						p.x = p.x/sqrt((double)ATLAS_ELEMENTS) + 2*sqrt((double)ATLAS_ELEMENTS)/ATLAS_ELEMENTS;
						p.y = p.y/sqrt((double)ATLAS_ELEMENTS);
						before_index_UV.push_back(p);	
					}				
				}		
			}
		}
	}

	//initShadowAndLight(before_index_shadow, before_index_light, this);

	//::indexVBO(before_index_vert, before_index_UV, before_index_norm, before_index_shadow, before_index_light, node_indices, node_vertices, node_uvs, node_normal, node_shadow, node_light);

	//Re create is causing problems..
	ReCreateBuffersThisLeaf();
	//printf("Done\n");

	before_index_vert.clear();
	before_index_UV.clear();
	before_index_norm.clear();
	before_index_shadow.clear();
	before_index_light.clear();
	before_index_types.clear();
}

float Octree::intersectionWithRay(Ray* ray)
{
	float out = -1;
	BoundingBox b = BoundingBox(this->min, this->max);
	ray->intersected(b,out);

	return out;		
}

void Octree::ReCreateBuffersThisLeaf()
{
	if(hasChildren == false)
	{
		/*
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, node_vertices.size()*sizeof(XMFLOAT3),&node_vertices[0][0], GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, node_uvs.size()*sizeof(XMFLOAT2),&node_uvs[0][0], GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBufferData(GL_ARRAY_BUFFER, node_normal.size()*sizeof(XMFLOAT3),&node_normal[0][0], GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, shadowbuffer);
		glBufferData(GL_ARRAY_BUFFER, node_shadow.size()*sizeof(float),&node_shadow[0], GL_STREAM_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, lightbuffer);
		glBufferData(GL_ARRAY_BUFFER, node_light.size()*sizeof(float),&node_light[0], GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, node_indices.size()*sizeof(unsigned int),&node_indices[0], GL_STREAM_DRAW);
		*/
	}	
}

void Octree::CreateBuffersThisLeaf()
{
	/*
		glGenBuffers(1, &vertexbuffer);
		glGenBuffers(1, &uvbuffer);
		glGenBuffers(1, &normalbuffer);
		glGenBuffers(1, &shadowbuffer);
		glGenBuffers(1, &lightbuffer);
		glGenBuffers(1, &elementbuffer);
	*/
}

void Octree::DeleteBuffersThisLeaf()
{
	node_colors.clear();
	node_shadow.clear();
	node_light.clear();
	node_vertices.clear();
	node_uvs.clear();
	node_indices.clear();
	node_normal.clear();
	/*
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &shadowbuffer);
	glDeleteBuffers(1, &lightbuffer);
	glDeleteBuffers(1, &elementbuffer);
	*/	
}

/*
void Octree::DrawThisLeaf(Shader& shader)
{
	if(this->hasChildren == false)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glEnableVertexAttribArray(shader["vVertex"]);
		glVertexAttribPointer(shader["vVertex"],  3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glEnableVertexAttribArray(shader["vUV"]);
		glVertexAttribPointer(shader["vUV"],  2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glEnableVertexAttribArray(shader["vNormal"]);
		glVertexAttribPointer(shader["vNormal"],  3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, shadowbuffer);
		glEnableVertexAttribArray(shader["vShadow"]);
		glVertexAttribPointer(shader["vShadow"],  1, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, lightbuffer);
		glEnableVertexAttribArray(shader["vLight"]);
		glVertexAttribPointer(shader["vLight"],  1, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(0);
			
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		//glDrawArrays(GL_TRIANGLES, 0, frame_vertices[i].size());
		glDrawElements(
			GL_TRIANGLES,      // mode
			node_indices.size(),    // countw
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
		);

	}
}
*/

void Octree::createListOfOctreesInPlayerArea(BoundingBox& bb, std::vector<Octree*>&out)
{
	if(hasChildren) 
	{
		for(int x = 0; x < 2; x++) 
		{
			for(int y = 0; y < 2; y++) 
			{
				for(int z = 0; z < 2; z++) 
				{
					children[x][y][z]->	createListOfOctreesInPlayerArea(bb, out);
				}
			}
		}
	}
	else 
	{
		//apparently quicker solution
		if(bb.intersect(BoundingBox(this->min.x, this->min.y, this->min.z, this->max.x, this->max.y, this->max.z)))
		{
			out.push_back(this);				
		}
	}
}

void Octree::createListOfOctreesInFrustum(Frustum* f, std::vector<Octree*>& out, int& oc) 
{
	if (hasChildren) 
	{
		for(int x = 0; x < 2; x++) 
		{
			for(int y = 0; y < 2; y++) 
			{
				for(int z = 0; z < 2; z++) 
				{
					children[x][y][z]->	createListOfOctreesInFrustum(f, out, oc);
				}
			}
		}
	}
	else 
	{
		//apparently quicker solution
		if(f->BoxInFrustum(this->min.x,this->min.y,this->min.z, this->max.x,this->max.y,this->max.z))
		{
			//f->octreInFrustum.push_back(this);
			out.push_back(this);
			oc++;
		}
	}
}

void Octree::getNumberOfAllOctrees(int& i)
{
	if (hasChildren) 
	{
		i++;
		for(int x = 0; x < 2; x++) 
		{
			for(int y = 0; y < 2; y++) 
			{
				for(int z = 0; z < 2; z++) 
				{
					children[x][y][z]->	getNumberOfAllOctrees(i);
				}
			}
		}
	}
	else 
	{
		i++;
	}
}

void Octree::getAllOctrees(std::vector<Octree*>& out) 
{
	if (hasChildren) 
	{
		out.push_back(this);
		for(int x = 0; x < 2; x++) 
		{
			for(int y = 0; y < 2; y++) 
			{
				for(int z = 0; z < 2; z++) 
				{
					children[x][y][z]->	getAllOctrees(out);
				}
			}
		}
	}
	else 
	{
		out.push_back(this);			
	}
}

void Octree::getHowManyVisibleBoxes(int& x, std::vector<Octree*>& in)
{
	x = 0;
	for(int i = 0; i < in.size(); i++)
	{
		for(int v = 0; v <in[i]->voxels.size(); v++)
		{
			if(in[i]->voxels[v]->Type != 0)
			{
				x++;
			}
		}
	}
}


void Octree::getClosestVoxelSelect(float x, Ray& r, XMFLOAT3& ret)
{
	int dst = 9999999;
	int idx;
	//printf("Octree: %x: float: %f \n", this, x);
	bool notFound = true;
	//for each voxel in this octree
	for(int i = 0; i < this->voxels.size(); i++)
	{
		float minX = -0.51 + this->voxels.at(i)->InitialPos.x;
		float maxX = 0.51 + this->voxels.at(i)->InitialPos.x;
		float minY = -0.51 + this->voxels.at(i)->InitialPos.y;
		float maxY = 0.51 + this->voxels.at(i)->InitialPos.y;
		float minZ = -0.51 + this->voxels.at(i)->InitialPos.z;
		float maxZ = 0.51 + this->voxels.at(i)->InitialPos.z;
		float distance = -9998;
		r.intersected(BoundingBox(minX, minY, minZ, maxX, maxY, maxZ), distance);			
			
		if(distance < x && distance > 1)
		{
					
			if(distance < dst && distance > 1) 
			{
				dst = distance;
				idx = i;
				notFound = false;
			}
		}			
	}

	if(notFound != true)
	{
		int i = idx;

		ret = this->voxels.at(i)->InitialPos;
	}
}
	
void Octree::getClosestVoxel(float x, Ray& r, int& to)
{
	int dst = 9999999;
	int idx;
	//printf("Octree: %x: float: %f \n", this, x);
	bool notFound = true;
	//for each voxel in this octree
	for(int i = 0; i < this->voxels.size(); i++)
	{
		float minX = -0.51 + this->voxels.at(i)->InitialPos.x;
		float maxX = 0.51 + this->voxels.at(i)->InitialPos.x;
		float minY = -0.51 + this->voxels.at(i)->InitialPos.y;
		float maxY = 0.51 + this->voxels.at(i)->InitialPos.y;
		float minZ = -0.51 + this->voxels.at(i)->InitialPos.z;
		float maxZ = 0.51 + this->voxels.at(i)->InitialPos.z;
		float distance = -9998;
		r.intersected(BoundingBox(minX, minY, minZ, maxX, maxY, maxZ), distance);			
			
		if(distance < x && distance > 1)
		{					
			if(distance < dst && distance > 1) 
			{
				dst = distance;
				idx = i;
				notFound = false;
			}
		}			
	}

	if(notFound != true)
	{
		int i = idx;
		this->voxels.at(i)->Type = to;
		::worldGrid[(int)this->voxels.at(i)->InitialPos.x][(int)this->voxels.at(i)->InitialPos.y][(int)this->voxels.at(i)->InitialPos.z].Type = to;
	}
}

bool Octree::inDistanceOf(float x, Ray& r)
{
	float d = -999;
	r.intersected(BoundingBox(this->min, this->max) ,d);
	if(d >= 0 && d <= x)
		return true;
	else 
		return false;
}

void Octree::colideWithRay(Ray* ray, std::hash_map<Octree*, float>& olist)
{
	if (hasChildren) 
	{
		for(int x = 0; x < 2; x++) 
		{
			for(int y = 0; y < 2; y++) 
			{
				for(int z = 0; z < 2; z++) 
				{
					children[x][y][z]->	colideWithRay(ray, olist);
				}
			}
		}
	}
	else 
	{
		//apparently quicker solution
		float x = -999;
		ray->intersected(BoundingBox(this->min, this->max), x); 
		if(x >= 0.5)
		{
			olist[this] = x;
		}
	}
}
/*
void Octree::initShadowAndLight(std::vector<float>& before_index_shadow, std::vector<float>& before_index_light, Octree* cO) 
{
	//for each element in each chunk check if is on border
	int internalSector = 0;
	int startPoint = 0;

	for(int v = 0; v < cO->voxels.size(); v++)
	{
		if(cO->voxels.at(v)->Type != -1)
		{
			// here
			for(int i = 0; i < tmpVert.size() ; i++)
			{
				int testX = cO->voxels.at(v)->InitialPos.x + tmpNorm.at(i).x;
				int testY = cO->voxels.at(v)->InitialPos.y + tmpNorm.at(i).y;
				int testZ = cO->voxels.at(v)->InitialPos.z + tmpNorm.at(i).z;	

				bool test = false;
				int maxXX = (int)CHUNK_SIZE;
					
				if(testX >= 0 && testX < maxXX)
					test = true;
				else 
					test = false;

				if(test != false)
					if(testY >= 0 && testY < maxXX)
							test = true;
					else 
						test = false;

				if(test != false)
				if(testZ >= 0 && testZ < maxXX)
					test = true;
				else 
					test =false;

				bool addToBuffer = false;

				if(test == true)
				{
					if(::worldGrid[testX][testY][testZ].Type == 0)
					{
						addToBuffer = true;
					}
				}

				if(testX > maxXX || testY > maxXX || testY > maxXX)
				{
					addToBuffer = true;
				}

				if(testX < 0 || testY <  0 || testY < 0)
				{
					addToBuffer = true;
				}

				if(addToBuffer)
				{
					isInShadow(startPoint, cO->voxels.at(v)->InitialPos.x,cO->voxels.at(v)->InitialPos.y,cO->voxels.at(v)->InitialPos.z, before_index_shadow);
					isInLight(startPoint, cO->voxels.at(v)->InitialPos.x,cO->voxels.at(v)->InitialPos.y,cO->voxels.at(v)->InitialPos.z, before_index_shadow, before_index_light);
					startPoint++;
				}
			}
		}
	}

	internalSector++;

	//std::cout << "initShadow complete \n";
}*/