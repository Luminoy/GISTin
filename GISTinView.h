// GISTinView.h : interface of the CGISTinView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GISTINVIEW_H__FA3CE189_6746_4A96_981A_7F3676F77102__INCLUDED_)
#define AFX_GISTINVIEW_H__FA3CE189_6746_4A96_981A_7F3676F77102__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//----------------宏定义
#include "math.h"
#define   ZOOMIN            1001
#define   ZOOMOUT           1002
#define   PAN               1003
#define   SELECT            1004
#define	  STARTPNT          1005
#define   ENDPNT            1006
#define   TESTCASE          1007
#define   ZoomFactor        2.0
#define   _MAX_PNUM_anArc   100000
#define   _MAX_ARCNUM_aMap  100000
#define   _PI               3.14159265358979323846
#define   ThreadPoolSIZE    8
#define   BlockGridSize     100 
#define   BlockGridOffset   0.1
#define   MAX_COLOR_NUM     136
//------------------


#include "delaunaytriangulation.h"
#include "PublicStruct.h"

#include <gdal.h>
#include <gdal_priv.h>
#include <gdal_frmts.h>
#include <ogr_api.h>
#include <ogr_core.h>
#include <ogrsf_frmts.h>

#include <xhash>
#include <utility>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <iostream>
using namespace std;

// 为Point2d 写的两个函数，以供unordered_map使用
struct hash_func {
	size_t operator() (const Point2d& PNT2d) const
	{
		return hash<double>()(PNT2d.x) + (hash<double>()(PNT2d.y) >> 4);
	}
};

struct hash_cmp {
	bool operator() (const Point2d& P1, const Point2d& P2) const
	{
		return P1.x == P2.x && P1.y == P2.y;
	}
};

struct MultisetLess {
	bool operator() (MyPoint *P1, MyPoint *P2) const
	{
		return  P1->accu < P2->accu;
	}
};
struct BlockTin
{
	vector<DCEL*> EdgeSet;
	COLORREF Color;
};

//--------------------------------------------------------------------
class CGISTinView : public CView
{
private:
//1.数据成员定义(窗口操作)
//-----------------------------------------------
   int      Type;
   int      OperateID;
   HCURSOR  m_hZoomIn;	//放大时的光标
   HCURSOR  m_hZoomOut;	//缩小时的光标
   HCURSOR  m_hPan;
   HCURSOR  m_hSelect;
   PNT  ptMouse,ptMouseTmp;
   BOOL   Captured;
//-----------------------------------------------
   double ClientWidth;    //客户区宽和高
   double ClientHeight;
   double zoomratio,xcenter,ycenter; //图形的中心、放大系数
   double xmin,ymin,xmax,ymax,dx,dy; //图形显示时的最小最大坐标(xmin,ymin)、(xmax,ymax)
   
   bool nFlagPoint,nFlagArc;
   double xminPoint,yminPoint,xmaxPoint,ymaxPoint;
   double xminArc,yminArc,xmaxArc,ymaxArc;
//-----------------------------------------------   
//2.数据成员定义
   long     pointNumber;   // 离散点的个数
   PointSet *Point;        // 离散点数组
   int      arcNumber;     // 弧段条数
   ArcSet   *Arc;
   MyPoint* & PointData = Point;

   //TRIANGLE *tinHead;
   PNT *pStartPoint, *pEndPoint;   //起始点的指针
   long nStartPointID, nEndPointID;//起始点的ID
   //LineSet m_LineSet;            
   TopoPoint *m_TopoPoint;       
   MyPoint *pPathPoints;           // 存放结果数据点
   long nPathPointNum;             // 路径点个数
   TopoPointCollection pTopoPointCollection;//点线的拓扑信息
   unordered_map<Point2d, int, hash_func, hash_cmp> mHashTable;
   vector<pair<vector<PNT>, double> > m_vecInputSHPGroups;
   MyDataPackage *pDataPackage;
   double bias_x, bias_y;

   bool m_DisplayResultPath;
   map<double, int> m_ColorRefTable;
public:
//1.函数成员定义(窗口操作)
   void LoadFile(int Type);
   void RefreshScreen();
   void ZoomFull();
   void GetScreenPoint(PNT *pt);
   void GetMapPoint(PNT * pt);
   void CalcBoundPoint();
   void CalcBoundArc();
   void CalcBoundGraph();
   void DrawGraph(CDC*pDC);
   void DrawRasterLayer(CDC* pDC, MyDataPackage * pDataPackage);
   void DrawPoint(CDC* pDC);
   void DrawPolygonFromPointGroups(CDC* pDC, vector<pair<vector<PNT>, double> >& pPointGroups);
   void DrawResultPath(CDC* pDC, MyPoint* pPathPoints, int count);
   void RefreshPoint(CDC *pDC,double x,double y, int radius = 2);
   void RefreshPoint(CDC *pDC, bool IsScreenPoint, double x, double y, COLOR PRGB, COLOR BRGB, int radius);
   void DrawArc(CDC* pDC);
   void RefreshARC(CDC *pDC,ArcSet arc);
   void RefreshLine(CDC *pDC,PNT pt1,PNT pt2);
   bool isIn_Qujian(double x, double x1,double x2);  

   void CalPointDistance(vector<PNT>& PNTSet);
   int GetPointIDByXY(double x, double y);
   void PointLineTopoConstruct();
   int  OnLeft(MyPoint P, MyPoint P1, MyPoint P2);
   //void LineTopologyConstruct();
   //void PointTopologyConstruct();
   //void CreateTriPath();
   int  ModifyPointData(int PID, PNT * pData);
   template<typename DT>
   void AssignEdgeAttribute(DCEL ** pEdges, int count, MyDataPackage *pPackage);
   void AssignEdgeAttribute(DCEL ** pEdges, const char * szFileName);
   void CreateLinePath();
   void AccuSort(vector<int>& vec, int left, int right);
   void OnPathConstruction();

   MyDataPackage * ReadRasterData(const char * filename);
   void ReadShapefile(const char * filename, char * fieldName = NULL);

   vector<PNT> SplitLongSegments(vector<pair<vector<PNT>, double> >& m_vecInputSHPGroups);

   void ElimiateDuplicatePoints(vector<PNT>& PNTSet);

   void SavePointsToTextFile(const char * filename, MyPoint * pData, int count);

   void SaveShapeFile(const char * filename, MyPoint * pData, int count);
   void SaveShapeFile(const char * filename, DCEL ** pData, int count);

   //template<typename DT>
   //bool LineOfSight(MyPoint& l1, MyPoint& l2, DT *pData, int nWidth, int nHeight);

   template<typename DT>
   double DDA_Line_2(int curr_x, int curr_y, int parent_x, int parent_y, DT * space, DT &NodataValue, int nWidth, int nHeight);

   template<typename DT>
   double Bresenham(int x1, int y1, int x2, int y2, DT * space, DT & NodataValue, int nWidth, int nHeight);

   template<typename DT>
   bool CGISTinView::LineOfSight(int x1, int y1, int x2, int y2, DT *pData, int nWidth, int nHeight);

protected:
	
	// create from serialization only
	CGISTinView();
	DECLARE_DYNCREATE(CGISTinView)

// Attributes
public:
	CGISTinDoc* GetDocument();
	// 为multiset重写的less比较函数

// Operations
private:   

public: 
	COLORREF colors[MAX_COLOR_NUM];
	CBrush   MyBrush[MAX_COLOR_NUM];
	CPen     MyPen[MAX_COLOR_NUM];
	//定义辅助格网
    Raster_Infor rasterobject;
    double m_dReadFileTime;	
	CString m_strDictionary;
    //图层显示控制
    bool m_displayGrid;
    bool m_displayMergeTin;
	bool m_displayTin;
    bool m_displayBinaryTree;
    void DrawGrid(CDC* pDC, Raster_Infor& Info);
    void DrawBinaryLeaf(CDC* pDC, Raster_Infor& Info, vector<BinaryTree*> Leaf);
    void DrawDelaunay(CDC *pDC, vector<DCEL*> &dcelCollection, COLORREF color, int nWidth = 1);  
	void DrawDelaunay(CDC *pDC, DCEL **pEdge, long nCount, COLORREF color, int nWidth = 1);  
    void DrawBlockTin(CDC *pDC, vector<BlockTin> &BlocksTin);
	void DrawBlockTin(vector<BlockTin> &BlocksTin);
	//存储二叉树叶子节点
    vector<BinaryTree*> TreeLeaf;  
	DCEL**   m_pMergeTin;
	long    m_nEdgeCount; 

	void PDTranslateToSite(PointSet *Data, int num, Point2d* &ps, int &nPoints);
	DCEL**   m_pDelaunayEdge;//存储直接运算的三角网
	long     m_nDeEdgeCount; 
   
	//记录初始行列数
	int m_nLines;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGISTinView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGISTinView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGISTinView)
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnPan();
	afx_msg void OnGlobe();
	afx_msg void OnSelect();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLoadPointFile();
	afx_msg void OnLoadLineFile();
	afx_msg void OnDisplayGrid();
	afx_msg void OnGenerateDelaunay();
	afx_msg void OnDisplayMergeDelaunay();
	afx_msg void OnUpdateDisplayMergeDelaunay(CCmdUI* pCmdUI);
	afx_msg void OnDisplayBinaryTree();
	afx_msg void OnUpdateDisplayBinaryTree(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayGrid(CCmdUI* pCmdUI);
	afx_msg void OnParallelGenerateTin();
	afx_msg void OnDisplayGenerateDelaunay();
	afx_msg void OnUpdateDisplayGenerateDelaunay(CCmdUI* pCmdUI);
	afx_msg void OnDivideGenerateTin();
	afx_msg void OnDivideGenerateTin2();
	afx_msg void OnParallelGenerateTin2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShapefileOpen();
	afx_msg void OnStartPNT();
	afx_msg void OnEndPNT();
	afx_msg void OnTestCase();
	afx_msg void OnTopoConstruct();
	afx_msg void OnCreatePath();
	afx_msg void OnRasterOpen();
	template<typename DT>
	void PathOptimize(MyPoint *pPath, int nPointCount, MyDataPackage *pDataPackage);
	void OnPathSmooth();
	void GenerateRandomPoint(MyPoint * P1, MyPoint * P2, int &x, int &y);
	void CGISTinView::GenerateRandomPoint(double x0, double y0, double x1, double y1, double &x, double &y);
	inline double DistanceOfTwoPoints(double x1, double y1, double x2, double y2);
	vector<MyPoint> AppendPointsInRectangleArea(double xmin, double xmax, double ymin, double ymax );
	void OnPointDensify();
	afx_msg void OnSavePoint();
	afx_msg void OnSaveLine();
	afx_msg void OnDisplayPath();
};

#ifndef _DEBUG  // debug version in GISTinView.cpp
inline CGISTinDoc* CGISTinView::GetDocument()
   { return (CGISTinDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GISTINVIEW_H__FA3CE189_6746_4A96_981A_7F3676F77102__INCLUDED_)
