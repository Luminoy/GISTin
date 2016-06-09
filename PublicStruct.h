#ifndef PUBLIC_STRUCT_H
#define PUBLIC_STRUCT_H
#include "point2d.h"

#include "delaunaytriangulation.h"

//�������ݽṹ

enum COLOR {
	BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, YELLO, WHITE
};

struct PNT
{
	double x;
	double y;
};

struct PointSet    //�㼯   
{
    double x;
	double y;   
    int ID; 
	int nC;  //�����к�
	int nL;  //�����к�

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


//դ����Ϣ
struct Raster_Infor 
{
   double dXmin, dYmin, dXmax, dYmax;   //դ�񳡵���С������
   int    nLineSize, nColSize;          //����������
   double dXdelt, dYdelt;               //x�����ȡ�y������
   double dDx, dDy;                     //ÿ��������Ԫ��x������y���� 
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

// ���ߵ����˹�ϵ��һ��������˼�����
struct TopoPoint {
	int nLineCount;
	int *pConnectLineIDs;
	TopoPoint() :nLineCount(0) { pConnectLineIDs = new int[4]; }
	// ̯�����ԣ���̬����洢�ռ�
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
//	int LID;                  //�߶ε�ID
//	int ID1, ID2;             //ID1��ID2Ϊ�߶ε����˵���ԭʼ�㼯�е����
//	int LeftTri, RightTri;    //�������������
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
//	int      ID1, ID2, ID3;  //��¼�������ԭ�㼯�е����
//	TRIANGLE *next, *back;   //next��backָ����ɨ�跨�д����ʼ��������
//	TRIANGLE *p1tin;         //��������ָ����ɨ�跨�б�ʾÿ�������Ͷ�Ӧ��3��������
//	TRIANGLE *p2tin;         //��Ӧ��ϵ�붥��y�����й�
//	TRIANGLE *p3tin;         //p1tin��Ӧy�������Ķ����Ӧ�ıߵ��ڽ������Σ�p2tin��֮��p3tin��С
//	int      g_SeqNum;       //�����ε����
//	int      visited;        //��ɨ�跨�ķǵݹ鷽ʽ��Ҫ��
//	double   weight;         //��������Ȩ��
//	double   accu;           //�ۼ�ֵ
//	TRIANGLE *parentTri;     //��������
//	TRIANGLE() : ID1(-1), ID2(-1), ID3(-1),
//		next(NULL), back(NULL), p1tin(NULL), p2tin(NULL), p3tin(NULL),
//		g_SeqNum(-1), visited(0), weight(1.), accu(0.), parentTri(NULL) {}
//	//TRIANGLE(int _ID1 = -1, int _ID2 = -1, int _ID3 = -1, )
//}TRIANGLENODE;

#endif //PUBLIC_STRUCT_H