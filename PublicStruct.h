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
class TopoPoint {
public:
	int nLineCount;
	int MaxElementCount;
	int *pConnectLineIDs;
	TopoPoint() {  }
	void CreateInstance() { nLineCount = 0; MaxElementCount = 4; pConnectLineIDs = new int[MaxElementCount]; }
	// 摊还策略，动态分配存储空间
	void AddLineID(int nLID) {
		if (nLineCount == MaxElementCount) {
			int *tmp = pConnectLineIDs;
			try
			{
				pConnectLineIDs = new int[MaxElementCount * 2];
			}
			catch (exception ex) {
				AfxMessageBox(_T("alloc new memory failed!"));
				return ;
			}
			MaxElementCount = MaxElementCount * 2;
			memcpy(pConnectLineIDs, tmp, nLineCount * sizeof(int));  //bug的根源是 sizeof !
			delete []tmp;
		}
		pConnectLineIDs[nLineCount++] = nLID;
	}

	~TopoPoint() {
		nLineCount = 0;
		delete[]pConnectLineIDs;
	}
};

class TopoPointCollection {
public:
	int nPointCount;
	int MaxElementCount;
	TopoPoint *pTopoPoints;

	TopoPointCollection(int _count = 0) :nPointCount(0), MaxElementCount(0), pTopoPoints(NULL) { }

	void Initialize(int _count = 100) {
		nPointCount = MaxElementCount = _count;
		Alloc(nPointCount);
	}

	//延迟分配策略
	bool Alloc(int _count) {
		if (_count <= 0)	return true;
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
			// 新建部分需要初始化
			for (int i = nPointCount; i < _count; i++) {
				pTopoPoints[i].CreateInstance();
			}
			delete[]tmp;
		}
		else
		{
			try
			{
				pTopoPoints = new TopoPoint[_count];
				for (int i = 0; i < _count; i++) {
					pTopoPoints[i].CreateInstance();
				}
			}
			catch (exception ex) {
				return false;
			}
			//memset(pTopoPoints, 0, _count);
		}
		return true;
	}

	TopoPoint& operator[](int pid) {
		return pTopoPoints[pid];
	}

	void Free() {
		for (int i = 0; i < nPointCount; i++) {
			delete &pTopoPoints[i];
		}
		delete[]pTopoPoints;
	}

	bool AddTopoPoint(TopoPoint *pNewTopoPoint) {
		if (MaxElementCount == nPointCount) {
			MaxElementCount = nPointCount + 100;
			if (!Alloc(MaxElementCount))	return false;
		}
		memcpy(&pTopoPoints[nPointCount++], pNewTopoPoint, sizeof(TopoPoint));
		return true;
	}

	TopoPoint* CreateTopoPoint(int pid) {
		if (MaxElementCount == pid) {
			MaxElementCount = MaxElementCount + 100;
			if (!Alloc(MaxElementCount)) {
				AfxMessageBox(_T("alloc memory failed!"));
				return NULL;
			}
		}
		return &pTopoPoints[pid];
	}

	~TopoPointCollection() { 
		Free();
	}
};

struct Line
{
	int LID;                  //线段的ID
	int ID1, ID2;             //ID1、ID2为线段的两端点在原始点集中的序号
	int LeftTri, RightTri;    //左右三角形序号
	Line* next;
	Line() : LID(-1), ID1(-1), ID2(-1), LeftTri(-1), RightTri(-1), next(NULL) {}
};

struct TopoLine {

};

struct LineSet {
	long nLineNum;
	Line* pLines;
};

struct Triangle {
	long TID;
	long PID1, PID2, PID3;
	long LID1, LID2, LID3;
	long TID1, TID2, TID3;
	Triangle *next;
};

typedef struct TRIANGLE
{
	int      ID1, ID2, ID3;  //记录坐标点在原点集中的序号
	TRIANGLE *next, *back;   //next和back指针在扫描法中窜起初始化的三角
	TRIANGLE *p1tin;         //以下三个指针在扫描法中标示每个三角型对应的3个三角形
	TRIANGLE *p2tin;         //对应关系与顶点y坐标有关
	TRIANGLE *p3tin;         //p1tin对应y坐标最大的顶点对应的边的邻接三角形，p2tin次之，p3tin最小
	int      g_SeqNum;       //三角形的序号
	int      visited;        //在扫描法的非递归方式中要用
	double   weight;         //三角网的权重
	double   accu;           //累计值
	TRIANGLE *parentTri;     //父三角形
	TRIANGLE() : ID1(-1), ID2(-1), ID3(-1),
		next(NULL), back(NULL), p1tin(NULL), p2tin(NULL), p3tin(NULL),
		g_SeqNum(-1), visited(0), weight(1.), accu(0.), parentTri(NULL) {}
	//TRIANGLE(int _ID1 = -1, int _ID2 = -1, int _ID3 = -1, )
}TRIANGLENODE;

#endif //PUBLIC_STRUCT_H