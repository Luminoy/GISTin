#ifndef PUBLIC_STRUCT_H
#define PUBLIC_STRUCT_H
#include "point2d.h"

#include "delaunaytriangulation.h"

//定义数据结构

enum COLOR {
	LIGHTPINK, PINK, CRIMSON, LAVENDERBLUSH, PALEVIOLETRED, HOTPINK, DEEPPINK, MEDIUMVIOLETRED, ORCHID, 
	THISTLE, PLUM, VIOLET, MAGENTA, FUCHSIA, DARKMAGENTA, PURPLE, MEDIUMORCHID, DARKVOILET, DARKORCHID, 
	INDIGO, BLUEVIOLET, MEDIUMPURPLE, MEDIUMSLATEBLUE, SLATEBLUE, DARKSLATEBLUE, LAVENDER, GHOSTWHITE, 
	BLUE, MEDIUMBLUE, MIDNIGHTBLUE, DARKBLUE, NAVY, ROYALBLUE, CORNFLOWERBLUE, LIGHTSTEELBLUE, LIGHTSLATEGRAY, 
	SLATEGRAY, DODERBLUE, ALICEBLUE, STEELBLUE, LIGHTSKYBLUE, SKYBLUE, DEEPSKYBLUE, LIGHTBLUE, POWDERBLUE, 
	CADETBLUE, AZURE, LIGHTCYAN, PALETURQUOISE, CYAN, AQUA, DARKTURQUOISE, DARKSLATEGRAY, DARKCYAN, TEAL, 
	MEDIUMTURQUOISE, LIGHTSEAGREEN, TURQUOISE, AUQAMARIN, MEDIUMAQUAMARINE, MEDIUMSPRINGGREEN, MINTCREAM, 
	SPRINGGREEN, SEAGREEN, HONEYDEW, LIGHTGREEN, PALEGREEN, DARKSEAGREEN, LIMEGREEN, LIME, FORESTGREEN, 
	GREEN, DARKGREEN, CHARTREUSE, LAWNGREEN, GREENYELLOW, OLIVEDRAB, BEIGE, LIGHTGOLDENRODYELLOW, IVORY, 
	LIGHTYELLOW, YELLOW, OLIVE, DARKKHAKI, LEMONCHIFFON, PALEGODENROD, KHAKI, GOLD, CORNISLK, GOLDENROD, 
	FLORALWHITE, OLDLACE, WHEAT, MOCCASIN, ORANGE, PAPAYAWHIP, BLANCHEDALMOND, NAVAJOWHITE, ANTIQUEWHITE, 
	TAN, BRULYWOOD, BISQUE, DARKORANGE, PERU, LINEN, PEACHPUFF, SANDYBROWN, CHOCOLATE, SADDLEBROWN, SEASHELL, 
	SIENNA, LIGHTSALMON, CORAL, ORANGERED, DARKSALMON, TOMATO, MISTYROSE, SALMON, SNOW, LIGHTCORAL, ROSYBROWN,
	INDIANRED, RED, BROWN, FIREBRICK, DARKRED, MAROON, WHITE, WHITESMOKE, GAINSBORO, LIGHTGREY, SILVER, DARKGRAY, GRAY, DIMGRAY, BLACK
};

enum TARGETTYPE {
	TIME, SPEED, VITALITY
};
/*! 8U */
typedef unsigned char   DT_8U;
/*! 16U */
typedef unsigned short  DT_16U;
/*! 16S */
typedef short           DT_16S;
/*! 32U */
typedef unsigned int    DT_32U;
/*! 32S */
typedef int             DT_32S;
/*! 32F */
typedef float           DT_32F;
/*! 64F */
typedef double          DT_64F;


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
	PointSet(double _x, double _y) : x(_x), y(_y), ID(-1), accu(0.), visited(false), parent(-1) {}
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
	vector<int> pConnectLineIDs;
	vector<int> pConnectPointIDs;
	TopoPoint(): nLineCount(0) {  }
	// 摊还策略，动态分配存储空间
	void AddLineID(int nLID, int nPID = -1) {
		nLineCount++;
		pConnectLineIDs.push_back(nLID);
		if (nPID != -1)	pConnectPointIDs.push_back(nPID);
	}

	void DestroyInstance() {
		nLineCount = 0;
		pConnectLineIDs.clear();
		pConnectPointIDs.clear();
	}

	~TopoPoint() {
		DestroyInstance();
	}
};

//class TopoPoint {
//public:
//	int nLineCount;
//	int MaxElementCount;
//	int *pConnectLineIDs;
//	TopoPoint() {  }
//	void CreateInstance() { nLineCount = 0; MaxElementCount = 10; pConnectLineIDs = new int[MaxElementCount]; }
//	// 摊还策略，动态分配存储空间
//	void AddLineID(int nLID) {
//		if (nLineCount == MaxElementCount) {
//			int *tmp = pConnectLineIDs;
//			try
//			{
//				pConnectLineIDs = new int[MaxElementCount * 2];
//			}
//			catch (exception ex) {
//				AfxMessageBox(_T("alloc new memory failed!"));
//				pConnectLineIDs = tmp;
//				return ;
//			}
//			MaxElementCount = MaxElementCount * 2;
//			memcpy(pConnectLineIDs, tmp, nLineCount * sizeof(int));  //bug的根源是 sizeof !
//			delete []tmp;
//		}
//		pConnectLineIDs[nLineCount++] = nLID;
//	}
//
//	void DestroyInstance() {
//		nLineCount = 0;
//		delete[]pConnectLineIDs; 
//		pConnectLineIDs = NULL;
//	}
//
//	~TopoPoint() {
//		DestroyInstance();
//	}
//};

class TopoPointCollection {
public:
	int nPointCount;
	vector<TopoPoint> pTopoPoints;
	TopoPointCollection() :nPointCount(0) { }

	void Initialize(int _count = 100) {
		pTopoPoints.resize(_count);
	}

	TopoPoint& operator[](int pid) {
		return pTopoPoints[pid];
	}

	void Destroy() {
		nPointCount = 0;
		pTopoPoints.clear();
	}
	~TopoPointCollection()
	{
		Destroy();
	}
};

class Triangle
{
public:
	Triangle() {}
	Triangle(int _P1, int _P2, int _P3, double _area) :P1(_P1), P2(_P2), P3(_P3), area(_area) {}

	int P1, P2, P3;
	double area;
	static double totalArea;
	
	int hashCode;
	int calHashCode(int a, int b, int c)
	{
		a -= b; a -= c; a ^= (c >> 13);
		b -= c; b -= a; b ^= (a << 8);
		c -= a; c -= b; c ^= (b >> 13);
		a -= b; a -= c; a ^= (c >> 12); 
		b -= c; b -= a; b ^= (a << 16);
		c -= a; c -= b; c ^= (b >> 5);
		a -= b; a -= c; a ^= (c >> 3); 
		b -= c; b -= a; b ^= (a << 10); 
		c -= a; c -= b; c ^= (b >> 15); 
		return c;
	}
};

//class TopoPointCollection {
//public:
//	int nPointCount;
//	int MaxElementCount;
//	TopoPoint *pTopoPoints;
//
//	TopoPointCollection(int _count = 0) :nPointCount(0), MaxElementCount(0), pTopoPoints(NULL) { }
//
//	void Initialize(int _count = 100) {
//		nPointCount = MaxElementCount = _count;
//		Alloc(nPointCount);
//	}
//
//	//延迟分配策略
//	bool Alloc(int _count) {
//		if (_count <= 0)	return true;
//		if (pTopoPoints) {
//			TopoPoint *tmp = pTopoPoints;
//			try 
//			{
//				pTopoPoints = new TopoPoint[_count];
//			}
//			catch (exception ex) {
//				return false;
//			}
//			memcpy(pTopoPoints, tmp, nPointCount);
//			// 新建部分需要初始化
//			for (int i = nPointCount; i < _count; i++) {
//				pTopoPoints[i].CreateInstance();
//			}
//			delete[]tmp;
//		}
//		else
//		{
//			try
//			{
//				pTopoPoints = new TopoPoint[_count];
//				for (int i = 0; i < _count; i++) {
//					pTopoPoints[i].CreateInstance();
//				}
//			}
//			catch (exception ex) {
//				return false;
//			}
//			//memset(pTopoPoints, 0, _count);
//		}
//		return true;
//	}
//
//	TopoPoint& operator[](int pid) {
//		return pTopoPoints[pid];
//	}
//
//	void Free() {
//		if (pTopoPoints) {
//			for (int i = 0; i < nPointCount; i++) {
//				delete &pTopoPoints[i];
//			}
//			delete[]pTopoPoints;
//		}
//	}
//
//	bool AddTopoPoint(TopoPoint *pNewTopoPoint) {
//		if (MaxElementCount == nPointCount) {
//			MaxElementCount = nPointCount + 100;
//			if (!Alloc(MaxElementCount))	return false;
//		}
//		memcpy(&pTopoPoints[nPointCount++], pNewTopoPoint, sizeof(TopoPoint));
//		return true;
//	}
//
//	TopoPoint* CreateTopoPoint(int pid) {
//		if (MaxElementCount == pid) {
//			MaxElementCount = MaxElementCount + 100;
//			if (!Alloc(MaxElementCount)) {
//				AfxMessageBox(_T("alloc memory failed!"));
//				return NULL;
//			}
//		}
//		return &pTopoPoints[pid];
//	}
//
//	~TopoPointCollection() { 
//		Free();
//	}
//};

struct MyDataPackage {
	void *pData;
	int nDataType;
	int nWidth, nHeight;              //图像尺寸
	float fPixelWidth, fPixelHeight;  //像元尺寸
	float fUpperBound, fLeftBound;    //左上角 
	double dNoDataValue;

	MyDataPackage(): pData(NULL), nDataType(0), nWidth(0), nHeight(0) ,
		fPixelWidth(0.), fPixelHeight(0.), fUpperBound(0.), fLeftBound(0.),  dNoDataValue(0.)  {}

	void Reset() {
		if (pData) {
			delete []pData;
			pData = NULL;
			nDataType = 0, nWidth = 0, nHeight = 0;
			fPixelWidth = 0., fPixelHeight = 0., fUpperBound = 0., fLeftBound = 0., dNoDataValue = 0.;
		}

	}

	bool SetInfo( int type, int width, int height, 
		float pixel_width, float pixel_height, float upper_bound, float left_bound, 
		double nodata_value ) {
		Reset();
		switch (type)
		{
		case 1:
			pData = new DT_8U[width * height];
			break;
		case 2:
			pData = new DT_16U[width * height];
			break;
		case 3:
			pData = new DT_16S[width * height];
			break;
		case 4:
			pData = new DT_32U[width * height];
			break;
		case 5:
			pData = new DT_32S[width * height];
			break;
		case 6:
			pData = new DT_32F[width * height];
			break;
		default:
			return false;
			break;
		}
		nWidth = width, nHeight = height, nDataType = type;
		fPixelWidth = pixel_width, fPixelHeight = pixel_height, fUpperBound = upper_bound, fLeftBound = left_bound;
		dNoDataValue = nodata_value;

		return true;
	}

	bool SetInfo(int type, void *pOuterData, int width, int height, 
		float pixel_width, float pixel_height, float upper_bound, float left_bound, 
		double nodata_value) {
		Reset();
		pData = pOuterData;
		nWidth = width, nHeight = height, nDataType = type;
		fPixelWidth = pixel_width, fPixelHeight = pixel_height, fUpperBound = upper_bound, fLeftBound = left_bound;
		dNoDataValue = nodata_value;

		return true;
	}

	~MyDataPackage()
	{
		Reset();
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
//struct Triangle {
//	long TID;
//	long PID1, PID2, PID3;
//	long LID1, LID2, LID3;
//	long TID1, TID2, TID3;
//	Triangle *next;
//};
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