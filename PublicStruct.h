#ifndef PUBLIC_STRUCT_H
#define PUBLIC_STRUCT_H
#include "point2d.h"

#include "delaunaytriangulation.h"

//定义数据结构

enum COLOR {
	BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, YELLO, WHITE
};

struct PNT
{
	double x;
	double y;
};

struct PointSet    //点集   
{
    double x;
	double y;   
    int ID; 
	int nC;  //所在列号
	int nL;  //所在行号

	double accu;
	bool visited;
	int parent;
	PointSet() : x(-1.), y(-1.), ID(-1), accu(0.), visited(false), parent(-1) {}
};

typedef struct PointSet MyPoint;

struct ArcSet   
{
  int pNum;            
  PNT *pts;           
};

struct GridField
{
	int pNum;            
    PNT *pts;
};


//栅格场信息
struct Raster_Infor 
{
   double dXmin, dYmin, dXmax, dYmax;   //栅格场的最小、最大点
   int    nLineSize, nColSize;          //行数、列数
   double dXdelt, dYdelt;               //x方向跨度、y方向跨度
   double dDx, dDy;                     //每个格网单元的x长度与y长度 
};

struct BinaryTree
{
	int        nStartLine, nStartCol;
	int        nEndLine, nEndCol;
	int        nLevel; 
	int        nPoints; 
	Point2d    *point;
	BinaryTree *LeftChild;
	BinaryTree *RightChild;
	BinaryTree *Parent;
	MaxEdge    maxEdge;
	BinaryTree() :LeftChild(NULL), RightChild(NULL), point(NULL), Parent(NULL) {}
};

// 点线的拓扑关系，一个点关联了几条边
struct TopoPoint {
	int nLineCount;
	int *pConnectLineIDs;
	TopoPoint() :nLineCount(0) { pConnectLineIDs = new int[4]; }
	// 摊还策略，动态分配存储空间
	void AddLineID(int nLID) {
		int maxsize = sizeof(pConnectLineIDs) / sizeof(int);
		if (nLineCount == maxsize) {
			int *tmp = pConnectLineIDs;
			pConnectLineIDs = new int[maxsize * 2];
			memcpy(pConnectLineIDs, tmp, maxsize);
			delete []tmp;
		}
		pConnectLineIDs[nLineCount++] = nLID;
	}

	~TopoPoint() {
		delete[]pConnectLineIDs;
	}
};

struct TopoPointCollection {
	int nPointCount;
	TopoPoint *pTopoPoints;

	TopoPointCollection(int _count = 0) :nPointCount(_count) { 
		pTopoPoints = new TopoPoint[_count < 100 ? 100 : _count]; 
	}

	bool Alloc(int _count) {
		if (_count <= 0 || _count <= nPointCount)	return true;
		if (pTopoPoints) {
			TopoPoint *tmp = pTopoPoints;
			try 
			{
				pTopoPoints = new TopoPoint[_count];
			}
			catch (exception ex) {
				return false;
			}
			
			memcpy(pTopoPoints, tmp, nPointCount);
			delete[]tmp;
		}
		return true;
	}
	void AddTopoPoint(TopoPoint *pNewTopoPoint) {
		
	}

	void AddTopoPoint(int pid) {
		if (pid == nPointCount) {
			if (Alloc(nPointCount * 2)) {

			}
			else
			{
				AfxMessageBox(_T("alloc error!"));
			}
		}
	}
	~TopoPointCollection() { 
		for (int i = 0; i < nPointCount; i++) {
			delete &pTopoPoints[i];
		}
		delete[]pTopoPoints;
	}
};
//struct Line
//{
//	int LID;                  //线段的ID
//	int ID1, ID2;             //ID1、ID2为线段的两端点在原始点集中的序号
//	int LeftTri, RightTri;    //左右三角形序号
//	Line* next;
//	Line() : LID(-1), ID1(-1), ID2(-1), LeftTri(-1), RightTri(-1), next(NULL) {}
//};
//
//struct TopoLine {
//
//};
//
//struct LineSet {
//	long nLineNum;
//	Line* pLines;
//};
//
//
//struct Triangle {
//	long TID;
//	long PID1, PID2, PID3;
//	long LID1, LID2, LID3;
//	long TID1, TID2, TID3;
//	Triangle *next;
//};
//
//
//typedef struct TRIANGLE
//{
//	int      ID1, ID2, ID3;  //记录坐标点在原点集中的序号
//	TRIANGLE *next, *back;   //next和back指针在扫描法中窜起初始化的三角
//	TRIANGLE *p1tin;         //以下三个指针在扫描法中标示每个三角型对应的3个三角形
//	TRIANGLE *p2tin;         //对应关系与顶点y坐标有关
//	TRIANGLE *p3tin;         //p1tin对应y坐标最大的顶点对应的边的邻接三角形，p2tin次之，p3tin最小
//	int      g_SeqNum;       //三角形的序号
//	int      visited;        //在扫描法的非递归方式中要用
//	double   weight;         //三角网的权重
//	double   accu;           //累计值
//	TRIANGLE *parentTri;     //父三角形
//	TRIANGLE() : ID1(-1), ID2(-1), ID3(-1),
//		next(NULL), back(NULL), p1tin(NULL), p2tin(NULL), p3tin(NULL),
//		g_SeqNum(-1), visited(0), weight(1.), accu(0.), parentTri(NULL) {}
//	//TRIANGLE(int _ID1 = -1, int _ID2 = -1, int _ID3 = -1, )
//}TRIANGLENODE;

#endif //PUBLIC_STRUCT_H