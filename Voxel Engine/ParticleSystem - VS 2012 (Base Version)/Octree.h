#pragma once

#include "d3dUtil.h"
#include <hash_map>
#include "Vertex.h"
#include "Frustum.h"
#include "Ray.h"
#include "BoundingBox.h"

#define CHUNK_SIZE 256
Vertex::Voxel worldGrid[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];
#define ATLAS_ELEMENTS 16

const int MAX_OCTREE_DEPTH = 16;
const int MIN_VOXELS_PER_OCTREE = 256;
const int MAX_VOXELS_PER_OCTREE = 512;

class Octree 
{
public:

	XMFLOAT3 min;
	XMFLOAT3 max;
	XMFLOAT3 center;

	Octree  *children[2][2][2];

	std::vector<Vertex::Voxel*> voxels;
	long unsigned numVoxels;

	bool hasChildren;
	int depth;

	void fileVoxel(Vertex::Voxel* voxel, XMFLOAT3 pos, bool addVoxel);	
	void haveChildren();
	void collectVoxels(std::vector<Vertex::Voxel*> &vs);
	void destroyChildren();
	void remove(Vertex::Voxel* voxel, XMFLOAT3 pos);

public:

	unsigned int vertexbuffer;
	unsigned int uvbuffer;
	unsigned int normalbuffer;
	unsigned int shadowbuffer;
	unsigned int lightbuffer;
	unsigned int elementbuffer;
	//unsigned int typesbuffer;

	std::vector<XMFLOAT3> node_vertices; 
	std::vector<XMFLOAT3> node_colors;
	std::vector<XMFLOAT3> node_normal;
//	std::vector<float>		node_types;
	std::vector<unsigned long> node_indices;
	std::vector<float>		node_shadow;
	std::vector<float>		node_light;
	std::vector<XMFLOAT2> node_uvs;

	Octree (XMFLOAT3 tmin, XMFLOAT3 tmax, int d);
	~Octree ();

	void add(Vertex::Voxel* voxel);
	void remove(Vertex::Voxel* voxel);
	void voxelMove(Vertex::Voxel* voxel, XMFLOAT3 oldPos);
	
	void buffersPreparation(std::vector<XMFLOAT3>& tmpVert, std::vector<XMFLOAT2>& tmpUV, std::vector<XMFLOAT3>& tmpNorm);

	bool displayIt(Vertex::Voxel& v);

	void RecreateBuffer(std::vector<XMFLOAT3>& tmpVert, std::vector<XMFLOAT2>& tmpUV, std::vector<XMFLOAT3>& tmpNorm);
	float intersectionWithRay(Ray* ray);

	void ReCreateBuffersThisLeaf();
	void CreateBuffersThisLeaf();
	void DeleteBuffersThisLeaf();
	
	//void DrawThisLeaf(Shader& shader);
	void createListOfOctreesInPlayerArea(BoundingBox& bb, std::vector<Octree*>&out);
	void createListOfOctreesInFrustum(Frustum* f, std::vector<Octree*>& out, int& oc) ;
	void getNumberOfAllOctrees(int& i);
	void getAllOctrees(std::vector<Octree*>& out);
	void getHowManyVisibleBoxes(int& x, std::vector<Octree*>& in);
	void getClosestVoxelSelect(float x, Ray& r, XMFLOAT3& ret);	
	void getClosestVoxel(float x, Ray& r, int& to);
	bool inDistanceOf(float x, Ray& r);
	void colideWithRay(Ray* ray, std::hash_map<Octree*, float>& olist);
	
	//void initShadowAndLight(std::vector<float>& before_index_shadow, std::vector<float>& before_index_light, Octree* cO) ; 
};