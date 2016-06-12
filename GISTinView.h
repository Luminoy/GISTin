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
//------------------

#include "delaunaytriangulation.h"
#include "PublicStruct.h"

#include <xhash>
#include <utility>
#include <vector>
#include <queue>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <iostream>
using namespace std;

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

   TRIANGLE *tinHead;
   PNT *pStartPoint, *pEndPoint;   //起始点的指针
   long nStartPointID, nEndPointID;//起始点的ID
   LineSet m_LineSet;            
   TopoPoint *m_TopoPoint;       
   MyPoint *pPathPoints;           // 存放结果数据点
   long nPathPointNum;             // 路径点个数
   TopoPointCollection pTopoPointCollection;//点线的拓扑信息
   unordered_map<Point2d, int, hash_func, hash_cmp> mHashTable;
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
   void DrawPoint(CDC* pDC);
   void RefreshPoint(CDC *pDC,double x,double y);
   void DrawArc(CDC* pDC);
   void RefreshARC(CDC *pDC,ArcSet arc);
   void RefreshLine(CDC *pDC,PNT pt1,PNT pt2);
   bool isIn_Qujian(double x, double x1,double x2);  

   void CalPointDistance(vector<PNT>& PNTSet);
   bool IsLineExist(int PID1, int PID2);
   int GetPointIDByXY(double x, double y);
   void PointLineTopoConstruct();
   int  OnLeft(MyPoint P, MyPoint P1, MyPoint P2);
   void LineTopologyConstruct();
   void PointTopologyConstruct();
   void CreateTriPath();
   int  ModifyPointData(int PID, PNT * pData);
   void CreateLinePath();
   void AccuSort(vector<long>& vec, long left, long right);
   void OnPathConstruction();
   void OnTinDensify();
protected: // create from serialization only
	CGISTinView();
	DECLARE_DYNCREATE(CGISTinView)

// Attributes
public:
	CGISTinDoc* GetDocument();

// Operations
private:   

public: 
	COLORREF colors[14];
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
};

#ifndef _DEBUG  // debug version in GISTinView.cpp
inline CGISTinDoc* CGISTinView::GetDocument()
   { return (CGISTinDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GISTINVIEW_H__FA3CE189_6746_4A96_981A_7F3676F77102__INCLUDED_)
