#ifndef BINARY_TREE
#define BINARY_TREE
#include "PublicStruct.h"
#include <vector>
using namespace std;

struct AdaptiveDivisionInfor
{
   vector<BinaryTree*> ChildLeafs;
   GridField **GridArray;
   BinaryTree *pTreeNode;
   double dDx;
   double dDy;
   AdaptiveDivisionInfor(GridField **pGridArray, double dx, double dy)
   {
       GridArray = pGridArray;
	   dDx = dx;
	   dDy = dy;
   }
   AdaptiveDivisionInfor()
   {
   }
};

BinaryTree* GreateTreeNode(int startLine, int endLine, int startCol, int endCol, int pNum, int level);

void FindLeafNode(BinaryTree *pTreeNode, vector<BinaryTree*> &Leaf);

void GenerateBinaryTree(vector<BinaryTree*> &ParentLeafs, BinaryTree *pRootNode, int ndivision, GridField **GridArray, double dDx, double dDy);

void GenerateBinaryTree2(vector<BinaryTree*> &ParentLeafs, BinaryTree *pRootNode, int ndivision, GridField **GridArray, double dDx, double dDy);

void AdaptiveDivision(int ndivision, GridField **GridArray, BinaryTree *pTreeNode, double dDx, double dDy);

void AdaptiveDivision(vector<BinaryTree*> &LeafNode, GridField **GridArray, BinaryTree *pTreeNode, double dDx, double dDy);

void AllocationBlock(PointSet *Point, GridField **GridArray, vector<BinaryTree*> &Leaf);

void ClearGridFieldMemory(GridField **GridArray, int Line, int Col);

GridField **  AllocationPointSet(PointSet *Point, int pointNum, Raster_Infor& rasterInfo);

void GenerateDelaunay(vector<BinaryTree*> &Leaf, int nThreadNum = 1); 

void GenerateDelaunayBySerial(vector<BinaryTree*> &Leaf);

BinaryTree* GapCombination( BinaryTree *pNode);

void SetBlockTinBTExtremum(vector<BinaryTree*> &Leaf);

void ThreadFunc(BinaryTree *pNode);

void BinaryTreeThreadFunc(AdaptiveDivisionInfor *pInfor);

#endif //BINARY_TREE
