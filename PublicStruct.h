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
class TopoPoint {
public:
	int nLineCount;
	int MaxElementCount;
	int *pConnectLineIDs;
	TopoPoint() {  }
	void CreateInstance() { nLineCount = 0; MaxElementCount = 4; pConnectLineIDs = new int[MaxElementCount]; }
	// ̯�����ԣ���̬����洢�ռ�
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
			memcpy(pConnectLineIDs, tmp, nLineCount * sizeof(int));  //bug�ĸ�Դ�� sizeof !
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

	//�ӳٷ������
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
			// �½�������Ҫ��ʼ��
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
	int LID;                  //�߶ε�ID
	int ID1, ID2;             //ID1��ID2Ϊ�߶ε����˵���ԭʼ�㼯�е����
	int LeftTri, RightTri;    //�������������
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
	int      ID1, ID2, ID3;  //��¼�������ԭ�㼯�е����
	TRIANGLE *next, *back;   //next��backָ����ɨ�跨�д����ʼ��������
	TRIANGLE *p1tin;         //��������ָ����ɨ�跨�б�ʾÿ�������Ͷ�Ӧ��3��������
	TRIANGLE *p2tin;         //��Ӧ��ϵ�붥��y�����й�
	TRIANGLE *p3tin;         //p1tin��Ӧy�������Ķ����Ӧ�ıߵ��ڽ������Σ�p2tin��֮��p3tin��С
	int      g_SeqNum;       //�����ε����
	int      visited;        //��ɨ�跨�ķǵݹ鷽ʽ��Ҫ��
	double   weight;         //��������Ȩ��
	double   accu;           //�ۼ�ֵ
	TRIANGLE *parentTri;     //��������
	TRIANGLE() : ID1(-1), ID2(-1), ID3(-1),
		next(NULL), back(NULL), p1tin(NULL), p2tin(NULL), p3tin(NULL),
		g_SeqNum(-1), visited(0), weight(1.), accu(0.), parentTri(NULL) {}
	//TRIANGLE(int _ID1 = -1, int _ID2 = -1, int _ID3 = -1, )
}TRIANGLENODE;

#endif //PUBLIC_STRUCT_H