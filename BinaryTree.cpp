#include "stdafx.h"
#include "BinaryTree.h"
#include "math.h"

volatile int FinGroup = 0;
volatile int nAdaptiveGroup = 0;

BinaryTree* GreateTreeNode(int startLine, int endLine, int startCol, int endCol, int pNum, int level)
{
	BinaryTree *pCreateNode = new BinaryTree;
	pCreateNode->nStartLine = startLine;  pCreateNode->nStartCol = startCol;
	pCreateNode->nEndLine = endLine;      pCreateNode->nEndCol = endCol;
	pCreateNode->nPoints = pNum;            pCreateNode->nLevel = level;
	return pCreateNode;
}

void FindLeafNode(BinaryTree *pTreeNode, vector<BinaryTree*> &Leaf)
{
	if (pTreeNode != NULL)
	{
		if (pTreeNode->LeftChild == NULL || pTreeNode->RightChild == NULL)
		{
			Leaf.push_back(pTreeNode);
		}
		else
		{	
			FindLeafNode(pTreeNode->LeftChild, Leaf);
	  	    FindLeafNode(pTreeNode->RightChild, Leaf);
		}
	}
}

void AdaptiveDivision(int ndivision, GridField **GridArray, BinaryTree *pTreeNode, double dDx, double dDy)
{
	if (pTreeNode->nLevel == ndivision)
	{
		return;
	}	
	int pNumTotal = pTreeNode->nPoints; 
	if (pNumTotal < 4)
	{
	   return;
	}
	int nCurrentTotal = 0;
	double xdist = (pTreeNode->nEndCol - pTreeNode->nStartCol + 1) * dDx;
	double ydist = (pTreeNode->nEndLine - pTreeNode->nStartLine + 1) * dDy;
	if (xdist > ydist && pTreeNode->nStartCol != pTreeNode->nEndCol)
	{
		bool NotInColGroup = true;
		int nCol = pTreeNode->nStartCol;	
		for (int i = pTreeNode->nStartLine; i <= pTreeNode->nEndLine; i ++)
		{
		    nCurrentTotal += GridArray[i][nCol].pNum;
		}
		if (nCurrentTotal == pNumTotal)
		{
			NotInColGroup = false;
		}
		while (NotInColGroup)
		{
			int nextColTotal = 0;
			nCol ++;
		    for (int i = pTreeNode->nStartLine; i <= pTreeNode->nEndLine; i ++)
			    nextColTotal += GridArray[i][nCol].pNum;
			//点集全部集中于某一列	
			if (nCurrentTotal == pNumTotal)
			{
				NotInColGroup = false;
				break;
			}
			if (fabs(pNumTotal - 2 * nCurrentTotal ) < fabs(pNumTotal - 2 * (nCurrentTotal + nextColTotal)))
			{
				break;
			}
			nCurrentTotal += nextColTotal;				
		}
		//需要二叉划分
		if (NotInColGroup)
		{
			BinaryTree *LeftLeaf = GreateTreeNode(pTreeNode->nStartLine, pTreeNode->nEndLine, pTreeNode->nStartCol, nCol -1, nCurrentTotal, pTreeNode->nLevel + 1);
		    BinaryTree *RightLeaf = GreateTreeNode(pTreeNode->nStartLine, pTreeNode->nEndLine, nCol, pTreeNode->nEndCol, pNumTotal - nCurrentTotal, pTreeNode->nLevel + 1);
			pTreeNode->LeftChild = LeftLeaf;  pTreeNode->RightChild = RightLeaf;
			LeftLeaf->Parent = pTreeNode; RightLeaf->Parent = pTreeNode;
			AdaptiveDivision(ndivision, GridArray, LeftLeaf, dDx, dDy);
	        AdaptiveDivision(ndivision, GridArray, RightLeaf, dDx, dDy);
		}
		//否则，不需要二叉划分
		else
		{
			return;
		}		   
	}
	//这种情况也不二叉划分
    else if (xdist > ydist && pTreeNode->nStartCol == pTreeNode->nEndCol)
	{ 
		return;
	}
    else if (xdist <= ydist && pTreeNode->nStartLine != pTreeNode->nEndLine)
	{
		bool NotInLineGroup = true;
		int nLin = pTreeNode->nStartLine;
		for (int i = pTreeNode->nStartCol; i <= pTreeNode->nEndCol; i ++)
		{
			nCurrentTotal += GridArray[nLin][i].pNum;
		}
        if (nCurrentTotal == pNumTotal)
		{
			NotInLineGroup = false;
		}
		while (NotInLineGroup)
		{			
			int nextLinTotal = 0;
			nLin ++;
			for (int i = pTreeNode->nStartCol; i <= pTreeNode->nEndCol; i ++)
			    nextLinTotal += GridArray[nLin][i].pNum;
			if (nCurrentTotal == pNumTotal)
			{
			    NotInLineGroup = false;
				break;
			}
			if (fabs(pNumTotal - 2 * nCurrentTotal ) < fabs(pNumTotal - 2 * (nCurrentTotal + nextLinTotal)))
			{
				break;
			}
			nCurrentTotal += nextLinTotal;				
		}
		//需要二叉划分
		if (NotInLineGroup)
		{
			BinaryTree *LeftLeaf = GreateTreeNode(pTreeNode->nStartLine, nLin - 1, pTreeNode->nStartCol, pTreeNode->nEndCol, nCurrentTotal, pTreeNode->nLevel + 1);
		    BinaryTree *RightLeaf = GreateTreeNode(nLin, pTreeNode->nEndLine, pTreeNode->nStartCol, pTreeNode->nEndCol, pNumTotal - nCurrentTotal, pTreeNode->nLevel + 1);
			pTreeNode->LeftChild = LeftLeaf;  pTreeNode->RightChild = RightLeaf;
	        LeftLeaf->Parent = pTreeNode; RightLeaf->Parent = pTreeNode;
			AdaptiveDivision(ndivision, GridArray, LeftLeaf, dDx, dDy);
	        AdaptiveDivision(ndivision, GridArray, RightLeaf, dDx, dDy);
		}
		//否则，不需要二叉划分
		else
		{
			return;
		}			
	}
	//这种情况也不二叉划分
	else if (xdist <= ydist && pTreeNode->nStartLine == pTreeNode->nEndLine)
	{
		return;
	}
}

void AdaptiveDivision(vector<BinaryTree*> &LeafNode, GridField **GridArray, BinaryTree *pTreeNode, double dDx, double dDy)
{
	int pNumTotal = pTreeNode->nPoints; 
	if (pNumTotal < 4)
	{
	   return;
	}
	int nCurrentTotal = 0;
	double xdist = (pTreeNode->nEndCol - pTreeNode->nStartCol + 1) * dDx;
	double ydist = (pTreeNode->nEndLine - pTreeNode->nStartLine + 1) * dDy;
	if (xdist > ydist && pTreeNode->nStartCol != pTreeNode->nEndCol)
	{
		bool NotInColGroup = true;
		int nCol = pTreeNode->nStartCol;	
		for (int i = pTreeNode->nStartLine; i <= pTreeNode->nEndLine; i ++)
		{
		    nCurrentTotal += GridArray[i][nCol].pNum;
		}
		if (nCurrentTotal == pNumTotal)
		{
			NotInColGroup = false;
		}
		while (NotInColGroup)
		{
			int nextColTotal = 0;
			nCol ++;
		    for (int i = pTreeNode->nStartLine; i <= pTreeNode->nEndLine; i ++)
			    nextColTotal += GridArray[i][nCol].pNum;
			if (nCurrentTotal == pNumTotal)
			{
				NotInColGroup = false;
				break;
			}
			if (fabs(pNumTotal - 2 * nCurrentTotal ) < fabs(pNumTotal - 2 * (nCurrentTotal + nextColTotal)))
			{
				break;
			}
			nCurrentTotal += nextColTotal;				
		}
		//需要二叉划分
		if (NotInColGroup)
		{
			BinaryTree *LeftLeaf = GreateTreeNode(pTreeNode->nStartLine, pTreeNode->nEndLine, pTreeNode->nStartCol, nCol -1, nCurrentTotal, pTreeNode->nLevel + 1);
		    BinaryTree *RightLeaf = GreateTreeNode(pTreeNode->nStartLine, pTreeNode->nEndLine, nCol, pTreeNode->nEndCol, pNumTotal - nCurrentTotal, pTreeNode->nLevel + 1);
			pTreeNode->LeftChild = LeftLeaf;  pTreeNode->RightChild = RightLeaf;
			LeftLeaf->Parent = pTreeNode; RightLeaf->Parent = pTreeNode;
			LeafNode.push_back(LeftLeaf);
			LeafNode.push_back(RightLeaf);
		}
		//否则，不需要二叉划分
		else
		{
			return;
		}		   
	}
	//这种情况也不二叉划分
    else if (xdist > ydist && pTreeNode->nStartCol == pTreeNode->nEndCol)
	{ 
		return;
	}
    else if (xdist <= ydist && pTreeNode->nStartLine != pTreeNode->nEndLine)
	{
		bool NotInLineGroup = true;
		int nLin = pTreeNode->nStartLine;
		for (int i = pTreeNode->nStartCol; i <= pTreeNode->nEndCol; i ++)
		{
			nCurrentTotal += GridArray[nLin][i].pNum;
		}
        if (nCurrentTotal == pNumTotal)
		{
			NotInLineGroup = false;
		}
		while (NotInLineGroup)
		{			
			int nextLinTotal = 0;
			nLin ++;

			for (int i = pTreeNode->nStartCol; i <= pTreeNode->nEndCol; i ++)
			    nextLinTotal += GridArray[nLin][i].pNum;

			if (nCurrentTotal == pNumTotal)
			{
			    NotInLineGroup = false;
				break;
			}
			if (fabs(pNumTotal - 2 * nCurrentTotal ) < fabs(pNumTotal - 2 * (nCurrentTotal + nextLinTotal)))
			{
				break;
			}
			nCurrentTotal += nextLinTotal;				
		}
		//需要二叉划分
		if (NotInLineGroup)
		{
			BinaryTree *LeftLeaf = GreateTreeNode(pTreeNode->nStartLine, nLin - 1, pTreeNode->nStartCol, pTreeNode->nEndCol, nCurrentTotal, pTreeNode->nLevel + 1);
		    BinaryTree *RightLeaf = GreateTreeNode(nLin, pTreeNode->nEndLine, pTreeNode->nStartCol, pTreeNode->nEndCol, pNumTotal - nCurrentTotal, pTreeNode->nLevel + 1);
			pTreeNode->LeftChild = LeftLeaf;  pTreeNode->RightChild = RightLeaf;
	        LeftLeaf->Parent = pTreeNode; RightLeaf->Parent = pTreeNode;
			LeafNode.push_back(LeftLeaf);
			LeafNode.push_back(RightLeaf);
		}
		//否则，不需要二叉划分
		else
		{
			return;
		}			
	}
	//这种情况也不二叉划分
	else if (xdist <= ydist && pTreeNode->nStartLine == pTreeNode->nEndLine)
	{
		return;
	}
}
void BinaryTreeThreadFunc(AdaptiveDivisionInfor *pInfor)
{
	AdaptiveDivision(pInfor->ChildLeafs, pInfor->GridArray, pInfor->pTreeNode, pInfor->dDx, pInfor->dDy);
	nAdaptiveGroup ++;
}

void GenerateBinaryTree(vector<BinaryTree*> &ParentLeafs, BinaryTree *pRootNode, int ndivision, GridField **GridArray, double dDx, double dDy)
{
	if (ndivision < 1)
	{
		return;
	}

	AdaptiveDivisionInfor InforObject(GridArray, dDx, dDy);
	//1.第一次划分，得到左右两个孩子节点
    InforObject.pTreeNode = pRootNode;
    AdaptiveDivision(InforObject.ChildLeafs, InforObject.GridArray, InforObject.pTreeNode, InforObject.dDx, InforObject.dDy);
    ParentLeafs = InforObject.ChildLeafs;
    
	
	//2.以左右孩子为任务，开始并行，最多64线程
	AdaptiveDivisionInfor pInfor[64];
	vector<BinaryTree*> resultLeafs;
	for (int i = 2; i <= ndivision; i ++)
	{
		nAdaptiveGroup = 0;
		resultLeafs.clear();
		int nSize = ParentLeafs.size();		
		for (int j = 0; j < nSize; j ++)
		{
			if (j < 64)
			{
				pInfor[j].dDx = dDx, pInfor[j].dDy = dDy, pInfor[j].GridArray = GridArray;
			    pInfor[j].pTreeNode = ParentLeafs[j];
				pInfor[j].ChildLeafs.clear();
 				DWORD ThreadID;
 				CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)BinaryTreeThreadFunc, (VOID*)(&pInfor[j]), 0, &ThreadID);	
			}
			else
			{
			    AdaptiveDivision(resultLeafs, GridArray, ParentLeafs[j], dDx, dDy);
				nAdaptiveGroup ++;
			}			
		}
	    for(;;)if(nAdaptiveGroup == nSize)break;

		ParentLeafs.clear();
		if (nSize > 64)
		{
			nSize = 64;	
			for (int m = 0; m < resultLeafs.size(); m ++)
			{
			    ParentLeafs.push_back(resultLeafs[m]);
			}
		}
		for (int k = 0; k < nSize; k ++)
		{
			ParentLeafs.push_back(pInfor[k].ChildLeafs[0]);
			ParentLeafs.push_back(pInfor[k].ChildLeafs[1]);
		}
	}
}

void GenerateBinaryTree2(vector<BinaryTree*> &ParentLeafs, BinaryTree *pRootNode, int ndivision, GridField **GridArray, double dDx, double dDy)
{
	if (ndivision < 1)
	{
		return;
	}
	//1.第一次划分，得到左右两个孩子节点
    AdaptiveDivision(ParentLeafs, GridArray, pRootNode, dDx, dDy);
	//2.以左右孩子为任务，开始划分
	vector<BinaryTree*> resultLeafs;
	for (int i = 2; i <= ndivision; i ++)
	{
		resultLeafs.clear();
		int nSize = ParentLeafs.size();		
		for (int j = 0; j < nSize; j ++)
		{
		    AdaptiveDivision(resultLeafs, GridArray, ParentLeafs[j], dDx, dDy);		
		}
		ParentLeafs.clear();
		for (int m = 0; m < resultLeafs.size(); m ++)
		{
		    ParentLeafs.push_back(resultLeafs[m]);
		}
	}
}

void AllocationBlock(PointSet *Point, GridField **GridArray, vector<BinaryTree*> &Leaf)
{
    if (Leaf.size() == 0)
    {
		return;
	}
	vector<BinaryTree*>::iterator iter;
	for(iter = Leaf.begin(); iter != Leaf.end(); iter++)
	{
		BinaryTree *pNode = *iter;
		int nCount = 0;
		pNode->point = new Point2d[pNode->nPoints];
		for (int i = pNode->nStartLine; i <= pNode->nEndLine; i ++)
		for(int j = pNode->nStartCol; j <= pNode->nEndCol; j ++)
		{
			for (int k = 0; k < GridArray[i][j].pNum; k ++)
			{
				pNode->point[nCount].x = GridArray[i][j].pts[k].x;
				pNode->point[nCount].y = GridArray[i][j].pts[k].y;
				nCount ++;
			}
		}
	}
}

void ClearGridFieldMemory(GridField **GridArray, int Line, int Col)
{
	for (int i = 0; i < Line; i ++)
	for (int j = 0; j < Col; j++)
	if (GridArray[i][j].pNum != 0)
	{
		delete [](GridArray[i][j].pts);
	}
	for (int i = 0; i < Line; i ++)
	{
		delete[]GridArray[i];
	}
	delete GridArray;
}

GridField ** AllocationPointSet(PointSet *Point, int pointNum, Raster_Infor& rasterInfo)
{
	int i = 0, j = 0;
	int C = 0, L = 0;
	double x = 0, y =0;
	
	//1.初始化二维数组
	GridField **GridArray = new GridField*[rasterInfo.nLineSize];
	for (i = 0; i < rasterInfo.nLineSize; i ++)
	{
		GridArray[i] = new GridField[rasterInfo.nColSize];
	}
	for (i = 0; i < rasterInfo.nLineSize; i ++)
	  for (j = 0; j < rasterInfo.nColSize; j++)
	  {
		  GridArray[i][j].pNum = 0;
		  GridArray[i][j].pts = NULL;
	  }

	//2.计算每个点所在的行列号
    for(i = 0; i < pointNum; i ++)
	{
      x=Point[i].x;    y = Point[i].y;
      C= (int)((x-rasterInfo.dXmin) / rasterInfo.dDx);
	  L= (int)((y-rasterInfo.dYmin) / rasterInfo.dDy);
      Point[i].nC = C;
	  Point[i].nL = L;
      GridArray[L][C].pNum++;
	}

	//3.重新开辟数组大小
    for (i = 0; i < rasterInfo.nLineSize; i ++)
	 for (j = 0; j < rasterInfo.nColSize; j++)
      if (GridArray[i][j].pNum != 0)
      {
		  GridArray[i][j].pts = new PNT[GridArray[i][j].pNum];
		  GridArray[i][j].pNum = 0;
      }

	//4.将原始点集分配到对应的格网中
    for(i = 0; i < pointNum; i ++)
	{
      C = Point[i].nC;
	  L = Point[i].nL;
	  GridArray[L][C].pts[GridArray[L][C].pNum].x = Point[i].x;
      GridArray[L][C].pts[GridArray[L][C].pNum].y = Point[i].y;
      GridArray[L][C].pNum++;
	} 
	return GridArray;
}

void ThreadFunc(BinaryTree *pNode)
{
	MaxEdge maxEdge = doDelaunayTriangulation(pNode->point, pNode->nPoints);
 	pNode->maxEdge = maxEdge;
	SetBottomTopExtremum(pNode->maxEdge, pNode->maxEdge.re);
	FinGroup ++;
}

void GenerateDelaunay(vector<BinaryTree*> &Leaf, int nThreadNum)
{
	int nCount = 0;
	vector<BinaryTree*>::iterator iter;
	for(iter = Leaf.begin(); iter != Leaf.end(); iter++)
	{
		BinaryTree *pNode = *iter;
		nCount ++;
		if (nCount < nThreadNum)
		{
			DWORD ThreadID;
		    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, (VOID*)pNode, 0, &ThreadID);	
		}
		else
		{
			MaxEdge maxEdge = doDelaunayTriangulation(pNode->point, pNode->nPoints);
 	        pNode->maxEdge = maxEdge;
			SetBottomTopExtremum(pNode->maxEdge, pNode->maxEdge.re);
	        FinGroup ++;
		}
	}
	for(;;)if(FinGroup == Leaf.size())break;
}

void GenerateDelaunayBySerial(vector<BinaryTree*> &Leaf)
{
    vector<BinaryTree*>::iterator iter;
	for(iter = Leaf.begin(); iter != Leaf.end(); iter++)
	{
		BinaryTree *pNode = *iter;
		MaxEdge maxEdge = doDelaunayTriangulation(pNode->point, pNode->nPoints);
 		pNode->maxEdge = maxEdge;
		SetBottomTopExtremum(pNode->maxEdge, pNode->maxEdge.re);
	}
}

void SetBlockTinBTExtremum(vector<BinaryTree*> &Leaf)
{
	vector<BinaryTree*>::iterator iter;
	for(iter = Leaf.begin(); iter != Leaf.end(); iter++)
	{
		BinaryTree *pNode = *iter;
		Edge *e = pNode->maxEdge.re;
		if (e != NULL)
		{
			SetBottomTopExtremum(pNode->maxEdge, e);
		}		
	}
}

BinaryTree* GapCombination(BinaryTree *pNode)
{
	if (pNode != NULL && pNode->LeftChild == NULL)
	{
		return pNode;
	}
	BinaryTree *pLeftNode = GapCombination(pNode->LeftChild);
	BinaryTree *pRightNode = GapCombination(pNode->RightChild);
	BinaryTree *pParent = pLeftNode->Parent;
	//如果是左右合并
	if (pLeftNode->nEndCol < pRightNode->nStartCol)
	{
		pParent->maxEdge = MergeDelaunay(pLeftNode->maxEdge, pRightNode->maxEdge, true);
	}
	else//如果是上下合并
	{
		pParent->maxEdge = MergeDelaunay(pLeftNode->maxEdge, pRightNode->maxEdge, false);
	}
	return pParent;
}