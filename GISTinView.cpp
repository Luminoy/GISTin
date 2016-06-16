// GISTinView.cpp : implementation of the CGISTinView class
//
#include "stdafx.h"
#include "GISTin.h"
#include "GISTinDoc.h"
#include "GISTinView.h"
#include "GridDlg.h"
#include "math.h"

#include "BinaryTree.h"

#define MIN_DIS_VALUE 1.0
#define MAX_DIS_VALUE 10.0

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGISTinView

IMPLEMENT_DYNCREATE(CGISTinView, CView)

BEGIN_MESSAGE_MAP(CGISTinView, CView)
	//{{AFX_MSG_MAP(CGISTinView)
	ON_COMMAND(ID_ZOOM_IN, OnZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
	ON_COMMAND(ID_PAN, OnPan)
	ON_COMMAND(ID_GLOBE, OnGlobe)
	ON_COMMAND(ID_SELECT, OnSelect)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_LOAD_POINT_FILE, OnLoadPointFile)
	ON_COMMAND(ID_LOAD_LINE_FILE, OnLoadLineFile)
	ON_COMMAND(ID_DISPLAY_GRID, OnDisplayGrid)
	ON_COMMAND(ID_GENERATE_DELAUNAY, OnGenerateDelaunay)
	ON_COMMAND(ID_DISPLAY_MERGE_DELAUNAY, OnDisplayMergeDelaunay)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_MERGE_DELAUNAY, OnUpdateDisplayMergeDelaunay)
	ON_COMMAND(ID_DISPLAY_BINARY_TREE, OnDisplayBinaryTree)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_BINARY_TREE, OnUpdateDisplayBinaryTree)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_GRID, OnUpdateDisplayGrid)
	ON_COMMAND(ID_PARALLEL_GENERATE_TIN, OnParallelGenerateTin)
	ON_COMMAND(ID_DISPLAY_GENERATE_DELAUNAY, OnDisplayGenerateDelaunay)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_GENERATE_DELAUNAY, OnUpdateDisplayGenerateDelaunay)
	ON_COMMAND(ID_DIVIDE_GENERATE_TIN, OnDivideGenerateTin)
	ON_COMMAND(ID_DIVIDE_GENERATE_TIN2, OnDivideGenerateTin2)
	ON_COMMAND(ID_PARALLEL_GENERATE_TIN2, OnParallelGenerateTin2)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_SHAPEFILE_OPEN, &CGISTinView::OnShapefileOpen)
	ON_COMMAND(ID_STARTPNT, &CGISTinView::OnStartPNT)
	ON_COMMAND(ID_ENDPNT, &CGISTinView::OnEndPNT)
	ON_COMMAND(ID_TOPOCONSTRUCT, &CGISTinView::OnTopoConstruct)
	ON_COMMAND(ID_TESTCASE, &CGISTinView::OnTestCase)
	ON_COMMAND(ID_CREATEPATH, &CGISTinView::OnCreatePath)
	ON_COMMAND(ID_RASTER_OPEN, &CGISTinView::OnRasterOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGISTinView construction/destruction
MyDataPackage* CGISTinView::ReadRasterData(const char *filename) {
	GDALAllRegister();
	GDALDataset *pData = (GDALDataset *)GDALOpen(filename, GA_ReadOnly);
	if (pData == NULL) {
		AfxMessageBox("ERRROR!");
		return NULL;
	}
	int nWidth = pData->GetRasterXSize();
	int nHeight = pData->GetRasterYSize();
	GDALRasterBand *pBand = pData->GetRasterBand(1);
	double fGeoTranform[6];       // 6个！
	pData->GetGeoTransform(fGeoTranform);
	GDALDataType type = pBand->GetRasterDataType();
	double NodataValue = pBand->GetNoDataValue();
	DT_8U *pReturnData = new DT_8U[nWidth*nHeight];
	pBand->RasterIO(GF_Read, 0, 0, nWidth, nHeight, pReturnData, nWidth, nHeight, type, 0, 0);
	
	// 设置1为可通行，0为不可通行
	//for (int i = 0; i < nHeight; i++) {
	//	for (int j = 0; j < nWidth; j++) {
	//		if (pReturnData[i*nWidth + j] == (DT_8U)NodataValue) {
	//			pReturnData[i*nWidth + j] = 1;
	//		}
	//		else
	//		{
	//			pReturnData[i*nWidth + j] = 0;
	//		}
	//	}
	//}
	switch (type)
	{
	case GDT_Byte:

		break;
	default:
		break;
	}
	CString str;
	str.AppendFormat("XSize: %d, YSize: %d\n", nWidth, nHeight);
	str.AppendFormat("DataType: %d\n", type);
	str.AppendFormat("Left: %.3lf, Upper: %.3lf\n", fGeoTranform[0], fGeoTranform[3]);
	str.AppendFormat("PixelWidth: %.3lf,PixelHeight: %.3lf\n", fGeoTranform[1], fGeoTranform[5]);

	AfxMessageBox(str);
	GDALClose(pData);

	MyDataPackage *pDataPackage = new MyDataPackage();
	pDataPackage->SetInfo(type, pReturnData, nWidth, nHeight, fGeoTranform[1], fGeoTranform[5], fGeoTranform[3], fGeoTranform[0], NodataValue);
	return pDataPackage;
}

vector<PNT> CGISTinView::ReadShapefile(const char *filename, char *format) {
	OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	OGRSFDriver* poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(format);
	OGRDataSource *poDS = poDriver->Open(filename);
	vector<PNT> PNTSet;
	if (poDS == NULL)
	{
		MessageBoxA("open failed!\n", "Error", 0);
	}
	else
	{
		OGRLayer *poLayer = poDS->GetLayer(0);
		OGRFeature *poFeat = NULL;
		int idx = 0;
		int pnt_count = 0;
		while ((poFeat = poLayer->GetNextFeature()) != NULL) {
			OGRGeometry *poGeometry = poFeat->GetGeometryRef();
			if (poGeometry != NULL) {
				if (wkbFlatten(poGeometry->getGeometryType()) == wkbPoint) {
					OGRPoint *poPoint = (OGRPoint *)poGeometry;
					pnt_count = 1;
					vector<PNT> group(pnt_count);

					PNT POINT;
					POINT.x = poPoint->getX();
					POINT.y = poPoint->getY();

					//PNTSet.push_back(POINT);
					group.push_back(POINT);

					m_vecInputSHPGroups.push_back(group);

					//char str[100];
					//sprintf_s(str, "%d : (%f, %f)\n", idx, poPoint->getX(), poPoint->getY());
					//MessageBoxA(NULL, str, "coordinate", 0);
				}
				else if (wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
				{
					OGRLineString *poLine = (OGRLineString *)poGeometry;
					pnt_count = poLine->getNumPoints();
					vector<PNT> group(pnt_count);
					for (int i = 0; i < pnt_count; i++) {
						OGRPoint *poPoint = new OGRPoint();
						poLine->getPoint(i, poPoint);

						PNT POINT;
						POINT.x = poPoint->getX();
						POINT.y = poPoint->getY();

						//PNTSet.push_back(POINT);
						group.push_back(POINT);

						//char str[100];
						//sprintf_s(str, "%d : (%f, %f)\n", idx, poPoint->getX(), poPoint->getY());
						//MessageBoxA(NULL, str, "coordinate", 0);
					}
					m_vecInputSHPGroups.push_back(group);

				}
				else if (wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon) {
					OGRPolygon *poPolygon = (OGRPolygon *)poGeometry;
					OGRLinearRing* poRing = poPolygon->getExteriorRing();
					pnt_count = poRing->getNumPoints();
					OGRPoint *poPoint = new OGRPoint();
					vector<PNT> group;
					for (int i = 0; i < pnt_count; i++) {
						poRing->getPoint(i, poPoint);

						PNT POINT;
						POINT.x = poPoint->getX();
						POINT.y = poPoint->getY();

						//PNTSet.push_back(POINT);
						group.push_back(POINT);

						//char str[100];
						//sprintf_s(str, "%d : (%f, %f)\n", idx, poPoint->getX(), poPoint->getY());
						//MessageBoxA(NULL, str, "coordinate", 0);
					}
					m_vecInputSHPGroups.push_back(group);

				}
				else if (poGeometry->getGeometryType() == wkbMultiPolygon25D) {  //加了wkbFlatten居然会导致判断为不等于！！！
					OGRMultiPolygon *poMultiPolygon = (OGRMultiPolygon *)poGeometry;

					for (int k = 0; k < poMultiPolygon->getNumGeometries(); k++) {
						OGRPolygon *poPolygon = (OGRPolygon *)poMultiPolygon->getGeometryRef(k);
						OGRLinearRing *poRing = (OGRLinearRing *)poPolygon->getExteriorRing();
						pnt_count = poRing->getNumPoints();
						OGRPoint *poPoint = new OGRPoint();
						vector<PNT> group;
						for (int i = 0; i < pnt_count; i++) {
							poRing->getPoint(i, poPoint);

							PNT POINT;
							POINT.x = poPoint->getX();
							POINT.y = poPoint->getY();

							//PNTSet.push_back(POINT);
							group.push_back(POINT);
						}
						m_vecInputSHPGroups.push_back(group);
					}
				}
				else
				{
					printf("%d : no geometry !\n", idx);
				}
				OGRFeature::DestroyFeature(poFeat);
				idx++;
			}
		}

		// 长线段的切割
		for (int v = 0; v < m_vecInputSHPGroups.size(); v++) {
			vector<PNT>& group = m_vecInputSHPGroups[v];
			if (group.size() >= 2) {
				for (int w = 0; w < group.size(); w++) {
					PNT pFirst = group[w % group.size()];
					PNT pSecond = group[(w + 1) % group.size()];
					double dis = sqrt(pow(pSecond.x - pFirst.x, 2) + pow(pSecond.y - pFirst.y, 2));
					if (dis > MAX_DIS_VALUE) {
						int parts = int(dis / MAX_DIS_VALUE) + (int(dis) % 10 == 0 ? 0 : 1); // 应该将原长线段划分为parts段
						double dx = (pSecond.x - pFirst.x) / parts;
						double dy = (pSecond.y - pFirst.y) / parts;
						for (int i = 1; i < parts; i++) {
							PNT NewPNT = { pFirst.x + i * dx, pFirst.y + i * dy };
							PNTSet.push_back(NewPNT);
						}
					}
					PNTSet.push_back(pSecond);
				}
			}
		}
		//if (pnt_count >= 2) {
		//	PNT pFirst, pSecond;
		//	//pFirst = pSecond = NULL;

		//	long idx_begin = PNTSet.size() - pnt_count;
		//	long idx_end = PNTSet.size() - 1;
		//	pFirst = PNTSet[idx_begin];
		//	for (int k = 1; k <= pnt_count; ++k) {
		//		pSecond = PNTSet[idx_begin + k % pnt_count];
		//		double dis = sqrt(pow(pSecond.x - pFirst.x, 2) + pow(pSecond.y - pFirst.y, 2));
		//		if (dis > MAX_DIS_VALUE) {
		//			int parts = int(dis / MAX_DIS_VALUE) + (int(dis) % 10 == 0 ? 0 : 1); // 应该将原长线段划分为parts段
		//			double dx = (pSecond.x - pFirst.x) / parts;
		//			double dy = (pSecond.y - pFirst.y) / parts;
		//			for (int i = 1; i < parts; i++) {
		//				PNT NewPNT = { pFirst.x + i * dx, pFirst.y + i * dy };
		//				PNTSet.push_back(NewPNT);
		//			}
		//		}
		//		pFirst = pSecond;
		//	}
		//}
		OGRDataSource::DestroyDataSource(poDS);
		OGRCleanupAll();
		return PNTSet;
	}
}

void CGISTinView::SaveShapeFile(const char *filename, const char *format, MyPoint* pData, int count) {
	::OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(format);
	OGRDataSource *poDS = poDriver->CreateDataSource(filename);
	OGRLayer *poLayer = poDS->CreateLayer("Points", NULL, wkbPoint);
	OGRFieldDefn ogrField("NO", OFTInteger);
	ogrField.SetWidth(10);
	poLayer->CreateField(&ogrField);

	for (int i = 0; i < count; ++i) {
		OGRFeature *poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
		poFeature->SetField("NO", i);
		OGRPoint point;
		point.setX(pData[i].x);
		point.setY(pData[i].y);
		poFeature->SetGeometry(&point);
		if (poLayer->CreateFeature(poFeature) != OGRERR_NONE) {
			AfxMessageBox("创建矢量数据出错！");
			return;
		}
		OGRFeature::DestroyFeature(poFeature);
	}

	OGRDataSource::DestroyDataSource(poDS);
	OGRCleanupAll();
}

CGISTinView::CGISTinView()
{
	OperateID=0;  zoomratio=1; 	Captured=FALSE; 	
	m_hZoomIn=AfxGetApp()->LoadCursor(IDC_ZOOMIN);
	m_hZoomOut=AfxGetApp()->LoadCursor(IDC_ZOOMOUT);
	m_hPan=AfxGetApp()->LoadCursor(IDC_PAN);
	m_hSelect=AfxGetApp()->LoadCursor(IDC_SELECT);    
	pointNumber=0;arcNumber=0;
    //Arc= new ArcSet[_MAX_ARCNUM_aMap];
	xmin=ymin=1.E+256; xmax=ymax=-1.E+256;
	nFlagPoint=FALSE; nFlagArc=FALSE;
	colors[0] = RGB(0, 0, 0);
	colors[1] = RGB(255, 0, 0);
	colors[2] = RGB(0, 255, 0);
	colors[3] = RGB(0, 0, 255);
	colors[4] = RGB(0, 255, 255);
	colors[5] = RGB(255, 0, 255);
	colors[6] = RGB(255, 255, 0);
	colors[7] = RGB(255, 255, 255);
	colors[8] = RGB(0, 127, 127);
	colors[9] = RGB(127, 0, 127);
	colors[10] = RGB(127, 127, 0);
	colors[11] = RGB(10, 200, 100);
	colors[12] = RGB(100, 200, 10);
	colors[13] = RGB(100, 100, 100);

	m_displayGrid = false;
	m_displayTin = true;
    m_displayMergeTin =true;
	m_displayBinaryTree = false;
	m_dReadFileTime = 0;
	m_strDictionary = _T("");
	m_nEdgeCount = 0;
	m_nDeEdgeCount = 0;

	//tinHead = NULL;
	pStartPoint = pEndPoint = NULL;
	m_TopoPoint = NULL;
	pPathPoints = NULL;
	nPathPointNum = 0;
	nStartPointID = nEndPointID = -1;
}
CGISTinView::~CGISTinView()
{
}

BOOL CGISTinView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGISTinView drawing
void CGISTinView::OnDraw(CDC* pDC)
{
	CGISTinDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	DrawGraph(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CGISTinView diagnostics

#ifdef _DEBUG
void CGISTinView::AssertValid() const
{
	CView::AssertValid();
}

void CGISTinView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGISTinDoc* CGISTinView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGISTinDoc)));
	return (CGISTinDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
//////////////////////窗口操作//////////////////////////////////////////////
void CGISTinView::OnZoomIn() 
{
	OperateID=ZOOMIN;
}
void CGISTinView::OnZoomOut() 
{
	OperateID=ZOOMOUT;
}
void CGISTinView::OnPan() 
{  
	OperateID=PAN; 
}
void CGISTinView::OnGlobe() 
{
	ZoomFull();
    RefreshScreen();
}
void CGISTinView::OnSelect() 
{
	OperateID=SELECT;
}

void CGISTinView::OnStartPNT()
{
	OperateID = STARTPNT;
}

void CGISTinView::OnEndPNT() {
	OperateID = ENDPNT;
}

void CGISTinView::OnTestCase() {
	OperateID = TESTCASE;
}

void CGISTinView::OnLButtonDown(UINT nFlags, CPoint point) 
{
    ptMouse.x=point.x;  ptMouse.y=point.y;  ptMouseTmp=ptMouse;
    PNT mpt=ptMouse;  GetMapPoint(&mpt);
	CRect rect;
	GetClientRect(&rect);
	//--------设置鼠标光标
    switch(OperateID)
	{
    case ZOOMOUT:  ::SetCursor(m_hZoomOut);
		            xcenter=mpt.x; ycenter=mpt.y; zoomratio*=ZoomFactor;
                   InvalidateRect(&rect);  break;
    case ZOOMIN:   ::SetCursor(m_hZoomIn);
		           Captured=true;  break;
	case SELECT:   ::SetCursor(m_hSelect);break; 
    
 	case PAN:      ::SetCursor(m_hPan);
    default: break;
	}
	CView::OnLButtonDown(nFlags, point);
}

void CGISTinView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	int pid = -1;
	Point2d p2d;
	int DX,DY;  PNT mpt1=ptMouse,mpt2={point.x,point.y};
    DX=(int)(mpt2.x-mpt1.x); DY=(int)(mpt2.y-mpt1.y);
    GetMapPoint(&mpt1); GetMapPoint(&mpt2);

	CRect   Rect;
	GetClientRect(&Rect);
	//-----设置鼠标光标
	switch(OperateID)
	{
    case ZOOMOUT:  ::SetCursor(m_hZoomOut);break;             
    case ZOOMIN:   ::SetCursor(m_hZoomIn);
		           Captured=false;
				   if(labs(DX)>=2&&labs(DY)>=2)
				   {
                       double xxmin,yymin,xxmax,yymax,dxx,dyy,fx,fy;
                       if(mpt1.x>mpt2.x){xxmin=mpt2.x; xxmax=mpt1.x;} 
					   else {xxmin=mpt1.x; xxmax=mpt2.x;}
                       if(mpt1.y>mpt2.y){yymin=mpt2.y; yymax=mpt1.y;} 
					   else {yymin=mpt1.y; yymax=mpt2.y;}
                       dxx=xxmax-xxmin;dyy=yymax-yymin;
					   xcenter=xxmin+dxx/2.;  ycenter=yymin+dyy/2.;
                        fx=dxx/ClientWidth;   fy=dyy/ClientHeight;
                       zoomratio=(fx>fy)?fx:fy;
				   }
                   else
				    {xcenter=mpt2.x; ycenter=mpt2.y; zoomratio/=ZoomFactor;}
				   InvalidateRect(&Rect); break;
	case PAN:      ::SetCursor(m_hPan);
		           xcenter-=mpt2.x-mpt1.x; ycenter-=mpt2.y-mpt1.y; 
		           InvalidateRect(&Rect); break;
	case SELECT:   ::SetCursor(m_hSelect);break;   
	case STARTPNT: 
		if (!pStartPoint) {
			pStartPoint = new PNT;
		}
		pStartPoint->x = point.x;
		pStartPoint->y = point.y;
		GetMapPoint(pStartPoint);
		nStartPointID = ModifyPointData(nStartPointID, pStartPoint);
		InvalidateRect(&Rect); break;
	case ENDPNT:
		if (!pEndPoint) {
			pEndPoint = new PNT;
		}
		pEndPoint->x = point.x;
		pEndPoint->y = point.y;
		GetMapPoint(pEndPoint);
		nEndPointID = ModifyPointData(nEndPointID, pEndPoint);
		InvalidateRect(&Rect); break;
	case TESTCASE:	
		for (int i = 0; i < pointNumber; i++) {
			if (abs(mpt2.x - PointData[i].x) < 0.05 && abs(mpt2.y - PointData[i].y) < 0.05) {
				pid = i;
				break;
			}
		}
		if (pid != -1) {
			CString cstr;
			cstr.AppendFormat("line count: %d\n", pTopoPointCollection[pid].nLineCount);
			for (int j = 0; j < pTopoPointCollection[pid].nLineCount; j++) {
				cstr.AppendFormat("%d: %d\n", j, pTopoPointCollection[pid].pConnectLineIDs[j]);
			}
			AfxMessageBox(cstr);
		}
		break;
    default: break;
	}
	CView::OnLButtonUp(nFlags, point);
}

void CGISTinView::OnMouseMove(UINT nFlags, CPoint point) 
{
	PNT mpt={point.x,point.y},pt12=ptMouseTmp;
   
 	GetMapPoint(&mpt);
	
	//------设置鼠标光标
    switch(OperateID)
	{
 	case PAN:                     ::SetCursor(m_hPan);break; 
	case ZOOMOUT:                 ::SetCursor(m_hZoomOut);break;
	case ZOOMIN:                  ::SetCursor(m_hZoomIn);break;
	case SELECT:                  ::SetCursor(m_hSelect);break;
	default:                      ::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW)); break;
	}

    if(Captured&&OperateID==ZOOMIN)
	{
        CClientDC dc(this);
		
        CPen  NewPen1(PS_DOT,1,RGB(255,255,255));
		CPen *OldPen=dc.SelectObject(&NewPen1);
		dc.SetROP2(R2_NOT);  //当前绘制的像素值设为屏幕像素值的反
		dc.SelectStockObject(NULL_BRUSH); //不使用画刷，即透明效果
		//-----消除原矩形
		dc.Rectangle((int)ptMouse.x, (int)ptMouse.y, (int)pt12.x, (int)pt12.y);

		CPen NewPen2(PS_DOT,1,RGB(0,0,0));
		dc.SelectObject(&NewPen2)->DeleteObject();
        //-----画新矩形
		dc.Rectangle((int)ptMouse.x, (int)ptMouse.y, (int)point.x, (int)point.y);
		dc.SelectObject(OldPen)->DeleteObject();

		ptMouseTmp.x=point.x;   ptMouseTmp.y=point.y;

	}
	CView::OnMouseMove(nFlags, point);
}
void CGISTinView::RefreshScreen()
{
	CRect rect;
	GetClientRect(&rect);
	InvalidateRect(&rect);
}
void CGISTinView::GetScreenPoint(PNT *pt)
{
	CRect  Rect;
	GetClientRect(&Rect);
	ClientHeight=Rect.Height();
	ClientWidth=Rect.Width();
	//计算屏幕坐标
    pt->x=(pt->x-xcenter)/zoomratio+ClientWidth/2;
    pt->y=(pt->y-ycenter)/zoomratio+ClientHeight/2;
    pt->y=ClientHeight-pt->y;
}
void CGISTinView::GetMapPoint(PNT * pt)
{
	//计算地图坐标
    pt->x=zoomratio*(pt->x-ClientWidth/2)+xcenter;
    pt->y=zoomratio*(ClientHeight/2-pt->y)+ycenter;
}
void CGISTinView::ZoomFull()
{
    //计算客户区的大小	
	CRect  Rect;
	GetClientRect(&Rect);
	ClientHeight=Rect.Height();
	ClientWidth=Rect.Width();
	//全局显示
    xcenter=xmin+dx/2.;  ycenter=ymin+dy/2.;
    double zx=dx/ClientWidth,
           zy=dy/ClientHeight;
    zoomratio=(zx>zy)?zx:zy;    
}
//--------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//////////////////////文件操作//////////////////////////////////////////////
void CGISTinView::LoadFile(int Type)
{
	CString  TheFileName;
    CFileDialog  FileDlg(true,NULL,NULL,OFN_HIDEREADONLY,"*.txt|*.txt|",AfxGetMainWnd());

    if(FileDlg.DoModal()==IDOK)
	{	
		TheFileName=FileDlg.GetPathName();	
		int n = TheFileName.ReverseFind('\\');
		m_strDictionary = TheFileName.Left(n);
	}
	else 
		return;

	FILE * fp=fopen(TheFileName,"r+");
    if(fp==NULL) return;
	if(Type==1)
	{
	  	
       clock_t start=clock();
	   
    
	   fscanf(fp,"%d",&pointNumber);
       Point=new PointSet[pointNumber];
       for(int i=0;i<pointNumber;i++)
	   {
		   fscanf(fp,"%d%lf%lf",&Point[i].ID,&Point[i].x,&Point[i].y);
	   }
	   clock_t stop = clock();
	   double dInterval = (double) (stop - start);
	   m_dReadFileTime = dInterval / 1000;

	   nFlagPoint=true;
	   CalcBoundPoint();
	   CalcBoundGraph();
	   ZoomFull();	   
 	   RefreshScreen();  

	}
	if(Type==2)
	{
      char s1[50],s2[50];
      int ArcID,pNo,pNum=0,aNum=0;
      PNT * pts = new PNT[_MAX_PNUM_anArc];
      fscanf(fp,"%d",&ArcID);
      for(;;)
	  {
        if(feof(fp))break;
        fscanf(fp,"%s%s",&s1,&s2);
        if(s1[0]=='E')
		{
            Arc[aNum].pNum=pNum;
            Arc[aNum].pts=new PNT[pNum];
            for(pNo=0;pNo<pNum;pNo++)
			{
                Arc[aNum].pts[pNo].x=pts[pNo].x;
                Arc[aNum].pts[pNo].y=pts[pNo].y;
			}
            aNum++;  ArcID=atoi(s2);  pNum=0;
            if(s2[0]=='E')break;
		}
        else{pts[pNum].x=atof(s1); pts[pNum++].y=atof(s2);}
	  }
      arcNumber = aNum;
	  nFlagArc=true;
	  int k=Arc[0].pNum;
	  CalcBoundArc();
	  CalcBoundGraph();
	  ZoomFull();
	  RefreshScreen();
    }
	fclose(fp);
}
void CGISTinView::CalcBoundArc()
{
	xminArc=yminArc= 1.E+256;  xmaxArc=ymaxArc=-1.E+256;
	for(int i=0;i<arcNumber;i++)
  	 for(int j=0;j<Arc[i].pNum;j++)
	 {
		if(Arc[i].pts[j].x>xmaxArc)
			xmaxArc=Arc[i].pts[j].x;  
		if(Arc[i].pts[j].x<xminArc)
			xminArc=Arc[i].pts[j].x;
		if(Arc[i].pts[j].y>ymaxArc)
			ymaxArc=Arc[i].pts[j].y;  
		if(Arc[i].pts[j].y<yminArc)
			yminArc=Arc[i].pts[j].y;  
	 }	 
}
void CGISTinView::CalcBoundPoint()
{
  xminPoint=yminPoint=1.E+256; xmaxPoint=ymaxPoint=-1.E+256;
  for(int i=0;i<pointNumber;i++)
  {
    if(Point[i].x<xminPoint) xminPoint=Point[i].x;
    if(Point[i].x>xmaxPoint) xmaxPoint=Point[i].x;
    if(Point[i].y<yminPoint) yminPoint=Point[i].y;
    if(Point[i].y>ymaxPoint) ymaxPoint=Point[i].y;
  }
}
void CGISTinView::CalcBoundGraph()
{
    
    if(nFlagPoint==TRUE)
	{
	  if(xminPoint<xmin) xmin=xminPoint;
	  if(xmaxPoint>xmax) xmax=xmaxPoint;
      if(yminPoint<ymin) ymin=yminPoint;
	  if(ymaxPoint>ymax) ymax=ymaxPoint;
	}
	if(nFlagArc==TRUE)
	{
	  if(xminArc<xmin) xmin=xminArc;
	  if(xmaxArc>xmax) xmax=xmaxArc;
	  if(yminArc<ymin) ymin=yminArc;
	  if(ymaxArc>ymax) ymax=ymaxArc;
	}   
    dx=xmax-xmin;  dy=ymax-ymin;
}
void CGISTinView::OnLoadPointFile() 
{
 LoadFile(1);	
}

void CGISTinView::OnLoadLineFile() 
{
  LoadFile(2);
}
//-------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
//////////////////////图形显示操作//////////////////////////////////////////
void CGISTinView::DrawGraph(CDC*pDC)
{
	DrawPoint(pDC);
    DrawArc(pDC);	

	if (!m_vecInputSHPGroups.empty()) {
		DrawPolygonFromPointGroups(pDC, m_vecInputSHPGroups);
	}
	if (m_displayMergeTin) 
	{
		DrawDelaunay(pDC, m_pMergeTin, m_nEdgeCount, RGB(255, 0, 0), 1);
	}
	if (m_displayGrid)
	{
		DrawGrid(pDC, rasterobject);	
	}
	
	if (m_displayTin)
	{
		DrawDelaunay(pDC, m_pDelaunayEdge, m_nDeEdgeCount, RGB(0, 0, 255));
	}
	
	if (m_displayBinaryTree)
	{
		DrawBinaryLeaf(pDC, rasterobject, TreeLeaf);
	}
	if (pPathPoints) 
	{
		DrawResultPath(pDC, pPathPoints, nPathPointNum);
	}

	
}
void CGISTinView::DrawDelaunay(CDC *pDC, vector<DCEL*> &dcelCollection, COLORREF color, int nWidth)
{
	if (dcelCollection.size() != 0)
	{
		CPen  NewPen;
	    NewPen.CreatePen(PS_SOLID, nWidth, color);
	    CPen *OldPen=pDC->SelectObject(&NewPen);
		vector<DCEL *>::iterator iter;
	    for(iter = dcelCollection.begin(); iter != dcelCollection.end(); iter++)
		{
			DCEL *pdecl = *iter;
			PNT P1 = {pdecl->e[0].oData->x, pdecl->e[0].oData->y};
		    PNT P2 = {pdecl->e[1].oData->x, pdecl->e[1].oData->y};
		    GetScreenPoint(&P1); GetScreenPoint(&P2); 
		    pDC->MoveTo(P1.x,P1.y);
		    pDC->LineTo(P2.x,P2.y);     
		}
		pDC->SelectObject(OldPen);
	}	
}
void CGISTinView::DrawDelaunay(CDC *pDC, DCEL **pEdge, long nCount, COLORREF color, int nWidth)
{
	CPen  BluePen;
	BluePen.CreatePen(PS_SOLID, nWidth, colors[BLUE]);

	CPen  GrayPen;
	GrayPen.CreatePen(PS_SOLID, nWidth, colors[PURPLE]);

	CPen *OldPen = pDC->SelectObject(&BluePen);
	for (int i = 0; i < nCount; i ++)
	{
		DCEL *pdecl = pEdge[i];
		PNT P1 = {pdecl->e[0].oData->x, pdecl->e[0].oData->y};
		PNT P2 = {pdecl->e[1].oData->x, pdecl->e[1].oData->y};
		GetScreenPoint(&P1); GetScreenPoint(&P2); 
		if (!pdecl->walkable) {
			pDC->SelectObject(&GrayPen);
			pDC->MoveTo(P1.x, P1.y);
			pDC->LineTo(P2.x, P2.y);
			pDC->SelectObject(&BluePen);
		}
		else
		{
			pDC->MoveTo(P1.x, P1.y);
			pDC->LineTo(P2.x, P2.y);
		}
		 
	}
    pDC->SelectObject(OldPen);
}
void CGISTinView::DrawBlockTin(vector<BlockTin> &BlocksTin)
{
	CDC *pDC=GetDC();
	for (int i = 0; i < BlocksTin.size(); i ++)
	{
		vector<DCEL*> edges = BlocksTin[i].EdgeSet;
		srand((unsigned)time( NULL));	
	    int c = rand()%14;  
		BlocksTin[i].Color = colors[c];
		DrawDelaunay(pDC, edges, BlocksTin[i].Color);		
		Sleep(1000);
	}
	ReleaseDC(pDC);
}
void CGISTinView::DrawBlockTin(CDC *pDC, vector<BlockTin> &BlocksTin)
{
	for (int i = 0; i < BlocksTin.size(); i ++)
	{
		vector<DCEL*> edges = BlocksTin[i].EdgeSet;
	    DrawDelaunay(pDC, edges, BlocksTin[i].Color);
	}
}
void CGISTinView::DrawBinaryLeaf(CDC* pDC, Raster_Infor& Info, vector<BinaryTree*> Leaf)
{
    if (Leaf.size() != 0)
    {
		CPen  NewPen;
	    NewPen.CreatePen(PS_SOLID,2,RGB(255,0,255));
	   CPen *OldPen=pDC->SelectObject(&NewPen);
        vector<BinaryTree*>::iterator iter;
	    for(iter = Leaf.begin(); iter != Leaf.end(); iter++)
		{
			BinaryTree *root = *iter;
			//起始点坐标
			double xStart = Info.dXmin + root->nStartCol * Info.dDx, yStart = Info.dYmin + root->nStartLine * Info.dDy;
			//终止点坐标
			double xEnd =  Info.dXmin + (root->nEndCol + 1) * Info.dDx, yEnd = Info.dYmin + (root->nEndLine + 1) * Info.dDy;
		   
			PNT P1 ={xStart, yStart};
			PNT P2 ={xEnd, yEnd};
			GetScreenPoint(&P1); GetScreenPoint(&P2); 

			pDC->MoveTo(P1.x,P1.y);	pDC->LineTo(P2.x,P1.y);
			pDC->MoveTo(P2.x,P1.y);	pDC->LineTo(P2.x,P2.y); 
			pDC->MoveTo(P2.x,P2.y);	pDC->LineTo(P1.x,P2.y); 
			pDC->MoveTo(P1.x,P2.y);	pDC->LineTo(P1.x,P1.y); 
		}
		pDC->SelectObject(OldPen);
    }
}
void CGISTinView::DrawGrid(CDC* pDC, Raster_Infor& Info)
{
	CPen  NewPen;
	NewPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	CPen *OldPen=pDC->SelectObject(&NewPen);
	//起始点坐标
	double xStart = Info.dXmin, yStart = Info.dYmin;
	//终止点坐标
    double xEnd = Info.dXmax, yEnd = Info.dYmax;
	//每一小格网的长、宽
	double xcol_wide = Info.dDx;
	double yrow_wide = Info.dDy;
	//先画行
	for(int row = 0; row <= Info.nLineSize; row ++)
	{
		PNT P1 ={xStart, yStart+yrow_wide*row};
		PNT P2 ={xEnd, yStart+yrow_wide*row };
		GetScreenPoint(&P1); GetScreenPoint(&P2); 
		pDC->MoveTo(P1.x,P1.y);
		pDC->LineTo(P2.x,P2.y);       
	}
	//画列
	for(int col = 0;col <= Info.nColSize; col ++)
	{
		PNT P1 ={xStart+xcol_wide*col, yStart};
		PNT P2 ={xStart+xcol_wide*col, yEnd};
		GetScreenPoint(&P1); GetScreenPoint(&P2); 
		pDC->MoveTo(P1.x,P1.y);
		pDC->LineTo(P2.x,P2.y);       
	}
	pDC->SelectObject(OldPen);
}
void CGISTinView::RefreshPoint(CDC *pDC,double x,double y, int radius)
{
    PNT P = {x,y};
    GetScreenPoint(&P);
	pDC->Ellipse(P.x - radius, P.y - radius, P.x + radius, P.y + radius);
	//pDC->SetPixel(P.x, P.y, colors[BLUE]);
}

void CGISTinView::RefreshPoint(CDC *pDC, bool IsScreenPoint, double x, double y, COLOR PRGB, COLOR BRGB, int radius)
{
	PNT P = { x,y };
	if (!IsScreenPoint) {
		GetScreenPoint(&P);
	}
	pDC->SetPixel(P.x, P.y, PRGB);
	pDC->Ellipse(P.x - radius, P.y - radius, P.x + radius, P.y + radius);
}


void CGISTinView::DrawPoint(CDC* pDC)
{
	CPen  NewPen;
    NewPen.CreatePen(PS_SOLID,1,colors[BLUE]);
    CPen *OldPen=pDC->SelectObject(&NewPen);

	int nSize = pointNumber;
	if (nSize > 500000)
	{
	   nSize = 500000;
	}
 	for (int i=0;i<nSize;i++)
    {
		RefreshPoint(pDC,Point[i].x,Point[i].y);
	}

	CBrush brush2(colors[RED]);
	CBrush *pOldBrush = pDC->SelectObject(&brush2);

	// 绘制起点
	if (pStartPoint) {
		RefreshPoint(pDC, 0, pStartPoint->x, pStartPoint->y, BLACK, RED, 4);
	}
	// 绘制终点
	CBrush brush3(colors[GREEN]);
	pDC->SelectObject(&brush3);
	if (pEndPoint) {
		RefreshPoint(pDC, 0, pEndPoint->x, pEndPoint->y, BLACK, GREEN, 4);
	}

	pDC->SelectObject(OldPen);
	pDC->SelectObject(pOldBrush);
}

void CGISTinView::DrawPolygonFromPointGroups(CDC * pDC, vector<vector<PNT> >& vecPointGroups)
{
	if (vecPointGroups.empty())	return;
	CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, colors[BLUE]);
	CBrush Brush;
	Brush.CreateSolidBrush(colors[PURPLE]);

	CPen *pOldPen = pDC->SelectObject(&Pen);
	CBrush *pOldBrush = pDC->SelectObject(&Brush);
	
	for (int i = 0; i < vecPointGroups.size(); i++) {
		vector<PNT> &group = vecPointGroups[i];
		pDC->BeginPath();
		if (group.size() > 2) {
			PNT P0(group[0]);
			GetScreenPoint(&P0);
			pDC->MoveTo(P0.x, P0.y);
			for (int j = 1; j < group.size(); j++) {
				PNT P1(group[j]);
				GetScreenPoint(&P1);
				pDC->LineTo(P1.x, P1.y);
			}
			pDC->LineTo(P0.x, P0.y);
		}
		pDC->EndPath();
		pDC->FillPath();
	}
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
}

void CGISTinView::DrawResultPath(CDC* pDC, MyPoint* pPathPoints, int count) {
	if (pPathPoints == NULL || count == 0) {
		return;
	}
	CPen Pen;
	Pen.CreatePen(PS_SOLID, 2, colors[RED]);
	CPen *pOldPen = pDC->SelectObject(&Pen);
	PNT P1, P2;
	P1.x = pPathPoints[0].x;
	P1.y = pPathPoints[0].y;
	GetScreenPoint(&P1);
	for (int i = 1; i < count; i++) {
		P2.x = pPathPoints[i].x;
		P2.y = pPathPoints[i].y;
		GetScreenPoint(&P2);
		pDC->MoveTo(P1.x, P1.y);
		pDC->LineTo(P2.x, P2.y);
		P1.x = P2.x;
		P1.y = P2.y;
	}
	pDC->SelectObject(pOldPen);
}

void CGISTinView::DrawArc(CDC* pDC)
{
	CPen  NewPen;
	NewPen.CreatePen(PS_SOLID,1,RGB(255,0,0));
	CPen *OldPen=pDC->SelectObject(&NewPen);
	for (int i=0;i<arcNumber;i++)
	   RefreshARC(pDC,Arc[i]);
	pDC->SelectObject(OldPen)->DeleteObject();
}
void CGISTinView::RefreshARC(CDC *pDC,ArcSet arc)
{
	for(int i=0;i<arc.pNum-1;i++)
		RefreshLine(pDC,arc.pts[i],arc.pts[i+1]);
}
void CGISTinView::RefreshLine(CDC *pDC,PNT pt1,PNT pt2)
{
	//利用原画笔绘制线段pt1->pt2(地图坐标)
    GetScreenPoint(&pt1);  GetScreenPoint(&pt2);
    double x1,x2,y1,y2,R=4000.,RxR=16.E+6,Dx,Dy,Dxy,Theta,kxy,x,y,Rxy;
    int IDA=0,IDB=0,ID_Draw=0;
    x1=pt1.x; y1=pt1.y;  x2=pt2.x; y2=pt2.y;
    if(x1*x1+y1*y1<=RxR)IDA=1;
    if(x2*x2+y2*y2<=RxR)IDB=1;
    if(IDA+IDB==2)ID_Draw=1;
    else if(IDA+IDB==1)
	{
		if(IDA==1)  {  Dx=x2-x1; Dy=y2-y1;  }
        if(IDB==1)  {  Dx=x1-x2; Dy=y1-y2;  }
        if(Dx==0.)    Theta=(Dy>=0.)? _PI/2.:-_PI/2.;
        else          Theta=atan(fabs(Dy/Dx));
        if(Dx>0&&Dy<0)Theta*=-1;
        if(Dx<0)      Theta=(Dy>=0.)?(_PI-Theta):(_PI+Theta);
        if(IDA==1)     {x2=x1+R*cos(Theta)*2;  y2=y1+R*sin(Theta)*2;}
        else if(IDB==1){x1=x2+R*cos(Theta)*2;  y1=y2+R*sin(Theta)*2;}
        ID_Draw=1;
	}
	else
	{ 
	  Dx=x2-x1;  Dy=y2-y1;  Dxy=x2*y1-x1*y2;
      if(Dxy*Dxy<R*R*(Dx*Dx+Dy*Dy))
	  {
		  Rxy=Dx*Dx+Dy*Dy;   x=-Dy*Dxy/Rxy;   y=Dx*Dxy/Rxy;
          if(isIn_Qujian(x,x1,x2)&&isIn_Qujian(y,y1,y2))
		  {
			  ID_Draw=1;
              if(fabs(Dy)>=fabs(Dx))
			  {
			  kxy=Dx/Dy; Dy=R*3.; Dx=kxy*Dy; x1=x+Dx; y1=y+Dy; x2=x-Dx; y2=y-Dy;
			  }
              else
			  {
				  kxy=Dy/Dx;  Dx=R*3.; Dy=kxy*Dx; x1=x+Dx; y1=y+Dy; x2=x-Dx; y2=y-Dy;
			  }
		  } 
		} 
	} 
	if(ID_Draw==1){pDC->MoveTo((int)x1,(int)y1); pDC->LineTo((int)x2,(int)y2);}
}
bool CGISTinView::isIn_Qujian(double x, double x1,double x2)
{
	if (x1>=x2)
	{
		if (x>=x2&&x<=x1)
		{
			return true;
		}
	}
	else
	{
		if (x>=x1&&x<=x2)
		{
			return true;
		}
	}
	return false;
}
void CGISTinView::OnDisplayGrid() 
{
	if (m_displayGrid == true)
	{
		m_displayGrid = false;
	}
	else if (m_displayGrid == false)
	{
		m_displayGrid = true;
	}
	RefreshScreen();
}

void CGISTinView::OnUpdateDisplayGrid(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_displayGrid);
}

void CGISTinView::OnDisplayMergeDelaunay() 
{
	if (m_displayMergeTin == true)
	{
		m_displayMergeTin = false;
	}
	else if (m_displayMergeTin == false)
	{
		m_displayMergeTin = true;
	}
	RefreshScreen();	
}

void CGISTinView::OnUpdateDisplayMergeDelaunay(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_displayMergeTin);
}
void CGISTinView::OnDisplayBinaryTree() 
{
	if (m_displayBinaryTree == true)
	{
		m_displayBinaryTree = false;
	}
	else if (m_displayBinaryTree == false)
	{
		m_displayBinaryTree = true;
	}
	RefreshScreen();	
}

void CGISTinView::OnUpdateDisplayBinaryTree(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_displayBinaryTree);	
}
void CGISTinView::OnDisplayGenerateDelaunay() 
{
	if (m_displayTin == true)
	{
		m_displayTin = false;
	}
	else if (m_displayTin == false)
	{
		m_displayTin = true;
	}
	RefreshScreen();	
}
void CGISTinView::OnUpdateDisplayGenerateDelaunay(CCmdUI* pCmdUI) 
{
    pCmdUI->SetCheck(m_displayTin);	
}
//////////////栅格场设置////////////////////////////////////////////////////////////////
bool SetRasterFieldInfor(int& ndivision, int&nThreadNum, Raster_Infor& rasterInfo, int pointNum, double xmin, double ymin, double xmax, double ymax, double dx, double dy, int &nLines, bool bflag = true)
{
	CGridDlg dlgGrid;
  	int nsqrt = sqrt(pointNum);
    dlgGrid.m_nLineSize = dlgGrid.m_nColumnSize = 1000;
	nLines = dlgGrid.m_nLineSize;
    if(dlgGrid.DoModal()==IDOK)
	{
	  rasterInfo.nLineSize = dlgGrid.m_nLineSize; 
	  rasterInfo.nColSize =dlgGrid.m_nColumnSize;
	  ndivision = dlgGrid.m_nDivision;
	  nThreadNum = dlgGrid.m_nThread;
	  if (bflag && (pow(2, ndivision) != nThreadNum))
	  {
		  AfxMessageBox("CPU个数与块数不相等，请重新输入！");
		  return false;
	  }
	}
	else
		return false;
	rasterInfo.dXdelt = dx, rasterInfo.dYdelt = dy;
    rasterInfo.dXmin = xmin - rasterInfo.dXdelt * 0.01;
    rasterInfo.dXmax = xmax + rasterInfo.dXdelt * 0.01;
    rasterInfo.dYmin = ymin - rasterInfo.dYdelt * 0.01;
    rasterInfo.dYmax = ymax + rasterInfo.dYdelt * 0.01;
    rasterInfo.dXdelt = rasterInfo.dXmax - rasterInfo.dXmin;
    rasterInfo.dYdelt = rasterInfo.dYmax - rasterInfo.dYmin;
	if (rasterInfo.dXdelt < rasterInfo.dYdelt)
	{
		rasterInfo.dDx = rasterInfo.dXdelt / rasterInfo.nColSize;
		rasterInfo.nLineSize = rasterInfo.dYdelt / rasterInfo.dDx;
		rasterInfo.dDy = rasterInfo.dYdelt / rasterInfo.nLineSize;
	}
	else if (rasterInfo.dXdelt > rasterInfo.dYdelt)
	{
		rasterInfo.dDy = rasterInfo.dYdelt / rasterInfo.nLineSize;
		rasterInfo.nColSize = rasterInfo.dXdelt / rasterInfo.dDy;
		rasterInfo.dDx = rasterInfo.dXdelt / rasterInfo.nColSize;
	}
	else
	{
		rasterInfo.dDx = rasterInfo.dXdelt / rasterInfo.nColSize, rasterInfo.dDy = rasterInfo.dYdelt / rasterInfo.nLineSize;
	}

	return true;
}
bool SetRasterFieldInfor2(int& ndivision, int&nThreadNum, Raster_Infor& rasterInfo, int pointNum, double xmin, double ymin, double xmax, double ymax, double dx, double dy, int &nLines, bool bflag = true)
{
	CGridDlg dlgGrid;
  	int nsqrt = sqrt(pointNum);
    dlgGrid.m_nLineSize = dlgGrid.m_nColumnSize = 500;
	nLines = dlgGrid.m_nLineSize;
    if(dlgGrid.DoModal()==IDOK)
	{
	  rasterInfo.nLineSize = dlgGrid.m_nLineSize; 
	  rasterInfo.nColSize =dlgGrid.m_nColumnSize;
	  ndivision = dlgGrid.m_nDivision;
	  nThreadNum = dlgGrid.m_nThread;
	  if (bflag && (pow(2, ndivision) != nThreadNum))
	  {
		  AfxMessageBox("CPU个数与块数不相等，请重新输入！");
		  return false;
	  }
	}
	else
		return false;
	rasterInfo.dXdelt = dx, rasterInfo.dYdelt = dy;
    rasterInfo.dXmin = xmin - rasterInfo.dXdelt * 0.01;
    rasterInfo.dXmax = xmax + rasterInfo.dXdelt * 0.01;
    rasterInfo.dYmin = ymin - rasterInfo.dYdelt * 0.01;
    rasterInfo.dYmax = ymax + rasterInfo.dYdelt * 0.01;
    rasterInfo.dXdelt = rasterInfo.dXmax - rasterInfo.dXmin;
    rasterInfo.dYdelt = rasterInfo.dYmax - rasterInfo.dYmin;
	if (rasterInfo.dXdelt < rasterInfo.dYdelt)
	{
		rasterInfo.dDx = rasterInfo.dXdelt / rasterInfo.nColSize;
		rasterInfo.nLineSize = rasterInfo.dYdelt / rasterInfo.dDx;
		rasterInfo.dDy = rasterInfo.dYdelt / rasterInfo.nLineSize;
	}
	else if (rasterInfo.dXdelt > rasterInfo.dYdelt)
	{
		rasterInfo.dDy = rasterInfo.dYdelt / rasterInfo.nLineSize;
		rasterInfo.nColSize = rasterInfo.dXdelt / rasterInfo.dDy;
		rasterInfo.dDx = rasterInfo.dXdelt / rasterInfo.nColSize;
	}
	else
	{
		rasterInfo.dDx = rasterInfo.dXdelt / rasterInfo.nColSize, rasterInfo.dDy = rasterInfo.dYdelt / rasterInfo.nLineSize;
	}

	return true;
}
//结构转换
void CGISTinView::PDTranslateToSite(PointSet *Data, int num, Point2d* &ps, int &nPoints)
{
	nPoints = num;
	ps = new Point2d[nPoints];
	for (int i = 0; i < num; i ++)
	{
		ps[i].x = Data[i].x;
		ps[i].y = Data[i].y;
	}
}
void TotalTime(clock_t start, double& duration, bool bDisplayMessage = true)
{
	clock_t stop = clock();
	double dInterval = ((double) (stop - start));
	duration += dInterval / 1000;
	if (bDisplayMessage)
	{
		CString str;
	    str.Format("时间开销:%lf",duration);
	    AfxMessageBox(str);
	}
}
///////////////////////////////////////////主函数调用过程////////////////////////////////////////////////////////
void CGISTinView::OnGenerateDelaunay() 
{
	int nPoints;
	Point2d *ps;
	PDTranslateToSite(Point, pointNumber, ps, nPoints);
	//1.直接生成三角网(测试时间)
	double duration = 0;
    clock_t start, stop;
	start=clock();
    MaxEdge maxEdge = doDelaunayTriangulation(ps, nPoints);
    stop = clock();    
    duration = (double) (stop - start);
	duration = duration / 1000;
	//2.文件存取
	FILE *fp = NULL;
	CString strFilePath = m_strDictionary + "\\直接生成时间.txt";
    CFileFind filefind;
    if(filefind.FindFile(strFilePath))
    {
	    fp = fopen(strFilePath, "at");		
	}
    else
	{
	   fp = fopen(strFilePath, "w");
	   fprintf(fp, "点数   读取文件时间   构建三角网时间\n");
	}
	fprintf(fp, "%d   %.4f   %.4f\n", pointNumber, m_dReadFileTime, duration);
    fclose(fp);

	//if (pointNumber > 50 * 10000)
	//{
	//	delete[]Point;
	//	Point = NULL;
	//	m_pDelaunayEdge = new DCEL*[pointNumber * 3];	     
	//	pointNumber = 0;
 //	   
	//}
	//else
	//{
	//	m_pDelaunayEdge = new DCEL*[pointNumber * 3];
	//}

	m_pDelaunayEdge = new DCEL*[pointNumber * 3];
	//3.收集边
	if (maxEdge.le != NULL)
	{
	    m_nDeEdgeCount = 0; 		
        collectDcel(maxEdge.le, m_pDelaunayEdge, m_nDeEdgeCount);
		collectDcel(maxEdge.re, m_pDelaunayEdge, m_nDeEdgeCount);
	}
	RefreshScreen();	
}

void CGISTinView::OnParallelGenerateTin() 
{
	//1.初始化栅格场
	int ndivision;
	int nThreadNum =1;
	bool bFlag = SetRasterFieldInfor(ndivision, nThreadNum, rasterobject, pointNumber, xmin, ymin, xmax, ymax, dx, dy, m_nLines);
    if (!bFlag)
    {
		return;
    }
	else if (pointNumber == 0)
	{
		return;
	}
    //测试时间
	double dInterval = 0;
    clock_t start, stop;
	double dDivisionTime, dParallelTime, dCombinationTime;	
	//2.将点集分配到栅格场格网中
	GridField **GridArray = AllocationPointSet(Point, pointNumber, rasterobject);	
	start = clock();
	//3.自适应分组
	BinaryTree *TreeRoot = GreateTreeNode(0, rasterobject.nLineSize - 1, 0, rasterobject.nColSize - 1, pointNumber, 0);   
    GenerateBinaryTree(TreeLeaf, TreeRoot, ndivision, GridArray, rasterobject.dDx, rasterobject.dDy);
	//4.分配每块的结点
	AllocationBlock(Point, GridArray, TreeLeaf);
	//5.释放栅格场内存
	ClearGridFieldMemory(GridArray, rasterobject.nLineSize, rasterobject.nColSize);
	stop = clock();
	dInterval = (double) (stop - start);
	dDivisionTime = dInterval / 1000;	
    start = clock();
	//6.并行生成Tin	
	GenerateDelaunay(TreeLeaf, nThreadNum);
    stop = clock();
	dInterval = (double) (stop - start);
	dParallelTime = dInterval / 1000;
	start = clock();
	//7.缝隙合并
	GapCombination(TreeRoot); 
    stop = clock();
	dInterval = (double) (stop - start);
	dCombinationTime = dInterval / 1000;
	//9.记录时间
	FILE *fp = NULL;
	CString strFilePath = m_strDictionary + "\\并行1000时间.txt";
    CFileFind filefind;
    if(filefind.FindFile(strFilePath))
    {
	    fp = fopen(strFilePath, "at");		
	}
    else
	{
	   fp = fopen(strFilePath, "w");
	   fprintf(fp, "点数   块数   线程数  初始栅格场行列数  读取文件时间 自适应分组时间 并行构建时间 缝隙合并时间 总时间(不含文件读取)\n");
	}
	double dTotalTime = dDivisionTime + dParallelTime + dCombinationTime;
	fprintf(fp, "%d   %d   %d     %d       %.4f       %.4f        %.4f      %.4f       %.4f\n", pointNumber, TreeLeaf.size(), nThreadNum, m_nLines, m_dReadFileTime, dDivisionTime, dParallelTime, dCombinationTime, dTotalTime);
    fclose(fp);
	//8.收集边
	if (pointNumber > 50 * 10000)
	{
		delete[]Point;
		Point = NULL; 
		m_pMergeTin = new DCEL*[pointNumber * 3];
		pointNumber = 0;	   
	}
	else
	{
		m_pMergeTin = new DCEL*[pointNumber * 3];
	}
	m_nEdgeCount = 0; 	
    collectDcel(TreeRoot->maxEdge.le, m_pMergeTin, m_nEdgeCount);
	collectDcel(TreeRoot->maxEdge.re, m_pMergeTin, m_nEdgeCount);   
	RefreshScreen();	
}

void CGISTinView::OnDivideGenerateTin() 
{
	//1.初始化栅格场
	int ndivision;
	int nThreadNum =1;
	bool bFlag = SetRasterFieldInfor(ndivision, nThreadNum, rasterobject, pointNumber, xmin, ymin, xmax, ymax, dx, dy, m_nLines, false);
    if (!bFlag)
    {
		return;
    }
	else if (pointNumber == 0)
	{
		return;
	}
    //测试时间
	double dInterval = 0;
    clock_t start, stop;
	double dTotalTime;
	
	//2.将点集分配到栅格场格网中
	GridField **GridArray = AllocationPointSet(Point, pointNumber, rasterobject);	
	start = clock();
	//3.自适应分组
	BinaryTree *TreeRoot = GreateTreeNode(0, rasterobject.nLineSize - 1, 0, rasterobject.nColSize - 1, pointNumber, 0);   
    GenerateBinaryTree2(TreeLeaf, TreeRoot, ndivision, GridArray, rasterobject.dDx, rasterobject.dDy);
	//4.分配每块的结点
	AllocationBlock(Point, GridArray, TreeLeaf);
	//5.释放栅格场内存
	ClearGridFieldMemory(GridArray, rasterobject.nLineSize, rasterobject.nColSize);
	//6.串行生成分块Tin	
	GenerateDelaunayBySerial(TreeLeaf);
	//7.缝隙合并
	GapCombination(TreeRoot); 
    stop = clock();
	dInterval = (double) (stop - start);
	dTotalTime = dInterval / 1000;
	//9.记录时间
	FILE *fp = NULL;
	CString strFilePath = m_strDictionary + "\\串行1000时间.txt";
    CFileFind filefind;
    if(filefind.FindFile(strFilePath))
    {
	    fp = fopen(strFilePath, "at");		
	}
    else
	{
	   fp = fopen(strFilePath, "w");
	   fprintf(fp, "点数   块数  初始栅格场行列数   读取文件时间  总时间(不含文件读取)\n");
	}
	fprintf(fp, "%d   %d   %d       %.4f       %.4f\n", pointNumber, TreeLeaf.size(), m_nLines, m_dReadFileTime, dTotalTime);
    fclose(fp);

	//8.收集边
	if (pointNumber > 50 * 10000)
	{
		delete[]Point;
		Point = NULL; 
		m_pMergeTin = new DCEL*[pointNumber * 3];
		pointNumber = 0;	   
	}
	else
	{
		m_pMergeTin = new DCEL*[pointNumber * 3];
	}

	m_nEdgeCount = 0; 	
    collectDcel(TreeRoot->maxEdge.le, m_pMergeTin, m_nEdgeCount);
	collectDcel(TreeRoot->maxEdge.re, m_pMergeTin, m_nEdgeCount);   
	RefreshScreen();		
}

void CGISTinView::OnDivideGenerateTin2() 
{
	//1.初始化栅格场
	int ndivision;
	int nThreadNum =1;
	bool bFlag = SetRasterFieldInfor2(ndivision, nThreadNum, rasterobject, pointNumber, xmin, ymin, xmax, ymax, dx, dy, m_nLines, false);
    if (!bFlag)
    {
		return;
    }
	else if (pointNumber == 0)
	{
		return;
	}
    //测试时间
	double dInterval = 0;
    clock_t start, stop;
	double dTotalTime;	
	//2.将点集分配到栅格场格网中
	GridField **GridArray = AllocationPointSet(Point, pointNumber, rasterobject);	
	start = clock();
	//3.自适应分组
	BinaryTree *TreeRoot = GreateTreeNode(0, rasterobject.nLineSize - 1, 0, rasterobject.nColSize - 1, pointNumber, 0);   
    GenerateBinaryTree2(TreeLeaf, TreeRoot, ndivision, GridArray, rasterobject.dDx, rasterobject.dDy);
	//4.分配每块的结点
	AllocationBlock(Point, GridArray, TreeLeaf);
	//5.释放栅格场内存
	ClearGridFieldMemory(GridArray, rasterobject.nLineSize, rasterobject.nColSize);
	//6.串行生成分块Tin	
	GenerateDelaunayBySerial(TreeLeaf);
	//7.缝隙合并
	GapCombination(TreeRoot); 
    stop = clock();
	dInterval = (double) (stop - start);
	dTotalTime = dInterval / 1000;
	//9.记录时间
	FILE *fp = NULL;
	CString strFilePath = m_strDictionary + "\\串行500时间.txt";
    CFileFind filefind;
    if(filefind.FindFile(strFilePath))
    {
	    fp = fopen(strFilePath, "at");		
	}
    else
	{
	   fp = fopen(strFilePath, "w");
	   fprintf(fp, "点数   块数  初始栅格场行列数   读取文件时间  总时间(不含文件读取)\n");
	}
	fprintf(fp, "%d   %d   %d       %.4f       %.4f\n", pointNumber, TreeLeaf.size(), m_nLines, m_dReadFileTime, dTotalTime);
    fclose(fp);
	//8.收集边
	if (pointNumber > 50 * 10000)
	{
		delete[]Point;
		Point = NULL; 
		m_pMergeTin = new DCEL*[pointNumber * 3];
		pointNumber = 0;	   
	}
	else
	{
		m_pMergeTin = new DCEL*[pointNumber * 3];
	}

	m_nEdgeCount = 0; 	
    collectDcel(TreeRoot->maxEdge.le, m_pMergeTin, m_nEdgeCount);
	collectDcel(TreeRoot->maxEdge.re, m_pMergeTin, m_nEdgeCount);   
	RefreshScreen();	
}

void CGISTinView::OnParallelGenerateTin2() 
{
	//1.初始化栅格场
	int ndivision;
	int nThreadNum =1;
	bool bFlag = SetRasterFieldInfor2(ndivision, nThreadNum, rasterobject, pointNumber, xmin, ymin, xmax, ymax, dx, dy, m_nLines);
    if (!bFlag)
    {
		return;
    }
	else if (pointNumber == 0)
	{
		return;
	}
    //测试时间
	double dInterval = 0;
    clock_t start, stop;
	double dDivisionTime, dParallelTime, dCombinationTime;	
	//2.将点集分配到栅格场格网中
	GridField **GridArray = AllocationPointSet(Point, pointNumber, rasterobject);	
	start = clock();
	//3.自适应分组
	BinaryTree *TreeRoot = GreateTreeNode(0, rasterobject.nLineSize - 1, 0, rasterobject.nColSize - 1, pointNumber, 0);   
    GenerateBinaryTree(TreeLeaf, TreeRoot, ndivision, GridArray, rasterobject.dDx, rasterobject.dDy);
	//4.分配每块的结点
	AllocationBlock(Point, GridArray, TreeLeaf);
	//5.释放栅格场内存
	ClearGridFieldMemory(GridArray, rasterobject.nLineSize, rasterobject.nColSize);
	stop = clock();
	dInterval = (double) (stop - start);
	dDivisionTime = dInterval / 1000;	
    start = clock();
	//6.并行生成Tin	
	GenerateDelaunay(TreeLeaf, nThreadNum);
    stop = clock();
	dInterval = (double) (stop - start);
	dParallelTime = dInterval / 1000;
	start = clock();
	//7.缝隙合并
	GapCombination(TreeRoot); 
    stop = clock();
	dInterval = (double) (stop - start);
	dCombinationTime = dInterval / 1000;
	//9.记录时间
	FILE *fp = NULL;
	CString strFilePath = m_strDictionary + "\\并行500时间.txt";
    CFileFind filefind;
    if(filefind.FindFile(strFilePath))
    {
	    fp = fopen(strFilePath, "at");		
	}
    else
	{
	   fp = fopen(strFilePath, "w");
	   fprintf(fp, "点数   块数   线程数  初始栅格场行列数  读取文件时间 自适应分组时间 并行构建时间 缝隙合并时间 总时间(不含文件读取)\n");
	}
	double dTotalTime = dDivisionTime + dParallelTime + dCombinationTime;
	fprintf(fp, "%d   %d   %d     %d       %.4f       %.4f        %.4f      %.4f       %.4f\n", pointNumber, TreeLeaf.size(), nThreadNum, m_nLines, m_dReadFileTime, dDivisionTime, dParallelTime, dCombinationTime, dTotalTime);
    fclose(fp);
	//8.收集边
	if (pointNumber > 50 * 10000)
	{
		delete[]Point;
		Point = NULL; 
		m_pMergeTin = new DCEL*[pointNumber * 3];
		pointNumber = 0;	   
	}
	else
	{
		m_pMergeTin = new DCEL*[pointNumber * 3];
	}
	m_nEdgeCount = 0; 	
    collectDcel(TreeRoot->maxEdge.le, m_pMergeTin, m_nEdgeCount);
	collectDcel(TreeRoot->maxEdge.re, m_pMergeTin, m_nEdgeCount);
	RefreshScreen();	
}

void CGISTinView::CalPointDistance(vector<PNT> &PNTSet) {
	int count = 0;
	queue<int> repeat_idx;
	for (int i = PNTSet.size() - 1; i > 0; --i) {
		for (int j = i - 1; j >= 0; --j) {
			double dis = sqrt(pow(PNTSet[i].x - PNTSet[j].x, 2) + pow(PNTSet[i].y - PNTSet[j].y, 2));
			if (dis < 1e-6) {
				//PNTSet.erase(PNTSet[i]);
				repeat_idx.push(j);
				count++;
				//CString cstr;
				//cstr.Format("%d: %lf, %lf\n%d: %lf, %lf", i, PNTSet[i].x, PNTSet[i].y, j, PNTSet[j].x, PNTSet[i].y);
				//AfxMessageBox(cstr);
			}
		}
	}

	CString cstr;
	cstr.Format("repeat point counts: %d\n", count);
	AfxMessageBox(cstr);

	// 去除重复点
	while (!repeat_idx.empty()) {
		vector<PNT>::iterator iter = PNTSet.begin();
		int idx = repeat_idx.front();
		repeat_idx.pop();
		PNTSet.erase(iter + idx);
	}
}

void CGISTinView::OnShapefileOpen()
{
	// TODO: 在此添加命令处理程序代码
	CString  TheFileName;
	CFileDialog  FileDlg(TRUE, NULL, "grass_land.shp", OFN_HIDEREADONLY | OFN_ENABLESIZING, "*.shp|*.shp|", AfxGetMainWnd());
	
	if (FileDlg.DoModal() == IDOK)
		TheFileName = FileDlg.GetPathName();
	else
		return;

	//char *filename = CString2LPSTR(TheFileName);
	vector<PNT> PNTSet = ReadShapefile(TheFileName, "ESRI Shapefile");
	CalPointDistance(PNTSet); //去重
	CalPointDistance(PNTSet);
	pointNumber = PNTSet.size();
	Point = new MyPoint[pointNumber + 4];
	for (int i = 0; i<pointNumber; i++)
	{
		Point[i].x = PNTSet[i].x;
		Point[i].y = PNTSet[i].y;
		Point[i].ID = i;
	}

	CString cstr;
	cstr.Format("total point counts: %d\n", pointNumber);
	AfxMessageBox(cstr);

	nFlagPoint = true;
	CalcBoundPoint();
	CalcBoundGraph();
	ZoomFull();
	RefreshScreen();
}

int CGISTinView::OnLeft(MyPoint P, MyPoint P1, MyPoint P2)
{
	if ((P2.x - P1.x)*(P.y - P1.y) - (P2.y - P1.y)*(P.x - P1.x) > 0)
		return -1;
	else if ((P2.x - P1.x)*(P.y - P1.y) - (P2.y - P1.y)*(P.x - P1.x) < 0)
		return 1;
	else
		return 0;
}


int CGISTinView::GetPointIDByXY(double x, double y) {
	for (int i = 0; i < pointNumber; i++) {
		if (x == PointData[i].x && y == PointData[i].y) { //TODO: hansh table may be a better choice.
			return i;
		}
	}
	return -1;
}

void CGISTinView::PointLineTopoConstruct() {
	// 建立查找表，unordered_map
	clock_t t1 = clock();
	for (int i = 0; i<pointNumber; i++)
	{
		Point2d p2d(Point[i].x, Point[i].y);
		mHashTable.insert(make_pair(p2d, i));
	}

	pTopoPointCollection.Initialize(pointNumber);
	for (int i = 0; i < m_nDeEdgeCount; i++)
	{
		DCEL *pdecl = m_pDelaunayEdge[i];
		int idx1 = mHashTable[*(pdecl->e[0].oData)]; //已解决//哈希函数有点问题
		int idx2 = mHashTable[*(pdecl->e[1].oData)];
		//int idx1 = (mHashTable.find(*(pdecl->e[0].oData)))->second; 
		//int idx2 = (mHashTable.find(*(pdecl->e[1].oData)))->second;
		//int idx1 = GetPointIDByXY(pdecl->e[0].oData->x, pdecl->e[0].oData->y); //逐点查找，很慢
		//int idx2 = GetPointIDByXY(pdecl->e[1].oData->x, pdecl->e[1].oData->y);
		pTopoPointCollection.pTopoPoints[idx1].AddLineID(i);
		pTopoPointCollection.pTopoPoints[idx2].AddLineID(i);
	}

	clock_t t2 = clock();
	double interval = (double)(t2 - t1) / 1000.;
	CString cstr;
	cstr.Format("topology construction: %.3lf s.\n", interval);
	AfxMessageBox(cstr);

}

//void CGISTinView::LineTopologyConstruct() {
//	//TriangleSet m_TriSet;
//	//Line* pLineColl = new Line[_MAX_ARCNUM_aMap];
//	Line *pNext, *pCurr;
//	pCurr = m_LineSet.pLines;
//	m_LineSet.pLines = NULL;
//	while (pCurr != NULL) {
//		pNext = pCurr->next;
//		delete pCurr;
//		pCurr = pNext;
//	}
//	long count = 0;
//	Line *LineHead, *LineRear;
//	LineHead = LineRear = NULL;
//	for (TRIANGLE *t = tinHead; t != NULL; t = t->next) {
//		// 先判断ID1、ID2组成的边是否已经存在(已存在的边则不用再次建立拓扑关系)
//		if (!IsLineExist(t->ID1, t->ID2)) {
//			Line* pLine = new Line;
//			pLine->LID = count++;
//			pLine->ID1 = t->ID1;
//			pLine->ID2 = t->ID2;
//			//判断顶点ID3是否在顶点ID1、ID2组成的直线的左边，-1为左，1为右
//			if (OnLeft(PointData[t->ID3], PointData[t->ID1], PointData[t->ID2]) == -1) {
//				pLine->LeftTri = t->g_SeqNum;
//				pLine->RightTri = t->p3tin ? t->p3tin->g_SeqNum : -1;
//			}
//			else
//			{
//				pLine->RightTri = t->g_SeqNum;
//				pLine->LeftTri = t->p3tin ? t->p3tin->g_SeqNum : -1;
//			}
//			if (count == 1) {
//				m_LineSet.pLines = pLine;
//				LineRear = pLine;
//			}
//			else
//			{
//				LineRear->next = pLine;
//				LineRear = LineRear->next;
//			}
//
//
//		}
//		//判断顶点ID2是否在顶点ID1、ID3组成的直线的左边，-1为左，1为右
//		if (!IsLineExist(t->ID1, t->ID3)) {
//			Line* pLine = new Line;
//			pLine->LID = count++;
//			pLine->ID1 = t->ID1;
//			pLine->ID2 = t->ID3;
//			if (OnLeft(PointData[t->ID2], PointData[t->ID1], PointData[t->ID3]) == -1) {
//				pLine->LeftTri = t->g_SeqNum;
//				pLine->RightTri = t->p2tin ? t->p2tin->g_SeqNum : -1;
//			}
//			else
//			{
//				pLine->RightTri = t->g_SeqNum;
//				pLine->LeftTri = t->p2tin ? t->p2tin->g_SeqNum : -1;
//			}
//			LineRear->next = pLine;
//			LineRear = LineRear->next;
//		}
//		//判断顶点ID1是否在顶点ID2、ID3组成的直线的左边，-1为左，1为右
//		if (!IsLineExist(t->ID2, t->ID3)) {
//			Line* pLine = new Line;
//			pLine->LID = count++;
//			pLine->ID1 = t->ID2;
//			pLine->ID2 = t->ID3;
//			if (OnLeft(PointData[t->ID1], PointData[t->ID2], PointData[t->ID3]) == -1) {
//				pLine->LeftTri = t->g_SeqNum;
//				pLine->RightTri = t->p1tin ? t->p1tin->g_SeqNum : -1;
//			}
//			else
//			{
//				pLine->RightTri = t->g_SeqNum;
//				pLine->LeftTri = t->p1tin ? t->p1tin->g_SeqNum : -1;
//			}
//			LineRear->next = pLine;
//			LineRear = LineRear->next;
//		}
//		//memcpy(pLineColl + count - 3, pLine, 3 * sizeof(Line));
//	}
//	m_LineSet.nLineNum = count;
//}
//
//void CGISTinView::PointTopologyConstruct() {
//	if (m_TopoPoint) {
//		delete[]m_TopoPoint;
//	}
//	m_TopoPoint = new TopoPoint[pointNumber];
//	Line* pLineHead = m_LineSet.pLines;
//	while (pLineHead != NULL) {
//		//CString cstr;
//		//cstr.Format("%d\n", pLineHead->LID);
//		//AfxMessageBox(cstr);
//		m_TopoPoint[pLineHead->ID1].pConnectLineIDs[m_TopoPoint[pLineHead->ID1].nLineCount++] = pLineHead->LID;
//		m_TopoPoint[pLineHead->ID2].pConnectLineIDs[m_TopoPoint[pLineHead->ID2].nLineCount++] = pLineHead->LID;
//		pLineHead = pLineHead->next;
//	}
//}
//
//void CGISTinView::CreateTriPath()
//{
//	if (nStartTri == -1 || nEndTri == -1) {
//		AfxMessageBox("未设置起点或终点！");
//		return;
//	}
//
//	std::queue<TRIANGLE*> queTri;
//
//	for (TRIANGLE *tri = tinHead; tri != NULL; tri = tri->next) {
//		tri->visited = 0;
//		if (tri->g_SeqNum == nStartTri) {
//			pStartTri = tri;
//		}
//		if (tri->g_SeqNum == nEndTri) {
//			pEndTri = tri;
//		}
//	}
//	// 最短路径计算
//	queTri.push(pStartTri);
//	while (queTri.size() != 0) {
//		TRIANGLE *T = queTri.front();
//		T->visited = 1;
//		if (T->p1tin && !T->p1tin->visited) {
//			double accu = T->accu + (T->weight + T->p1tin->weight) / 2;
//			if (!T->p1tin->parentTri || accu < T->p1tin->accu) {
//				T->p1tin->accu = accu;
//				T->p1tin->parentTri = T;
//			}
//			queTri.push(T->p1tin);
//		}
//		if (T->p2tin && !T->p2tin->visited) {
//			double accu = T->accu + (T->weight + T->p2tin->weight) / 2;
//			if (!T->p2tin->parentTri || accu < T->p2tin->accu) {
//				T->p2tin->accu = accu;
//				T->p2tin->parentTri = T;
//			}
//			queTri.push(T->p2tin);
//		}
//		if (T->p3tin && !T->p3tin->visited) {
//			double accu = T->accu + (T->weight + T->p3tin->weight) / 2;
//			if (!T->p3tin->parentTri || accu < T->p3tin->accu) {
//				T->p3tin->accu = accu;
//				T->p3tin->parentTri = T;
//			}
//			queTri.push(T->p3tin);
//		}
//		queTri.pop();
//	}
//
//	CRect Rect;
//	GetClientRect(&Rect);
//	InvalidateRect(&Rect);
//}

int CGISTinView::ModifyPointData(int PID, PNT *pData) {
	if (PID == -1) {
		MyPoint* PointData2;
		try {
			PointData2 = new MyPoint[pointNumber + 1];
		}
		catch (exception ex) {
			AfxMessageBox("alloc error!");
			return -1;
		}
		memcpy(PointData2, PointData, pointNumber * sizeof(MyPoint));
		PID = pointNumber++;

		delete[]PointData;
		PointData = PointData2;
		PointData2 = NULL;
	}

	if (pData != NULL) {
		PointData[PID].x = pData->x;
		PointData[PID].y = pData->y;
		PointData[PID].ID = PID;
	}

	return PID;
}

template<typename DT>
void CGISTinView::AssignEdgeAttribute(DCEL **pEdges, int count, MyDataPackage *pPackage) {
	int nWidth = pPackage->nWidth;
	int nHeight = pPackage->nHeight;
	float UpperBound = pPackage->fUpperBound;
	float LeftBound = pPackage->fLeftBound;
	float PixelWidth = pPackage->fPixelWidth;
	float PixelHeight = pPackage->fPixelHeight;
	
	DT *pData = static_cast<DT *>(pPackage->pData);
	DT NoDataValue = static_cast<DT>(pPackage->dNoDataValue);
	for (int i = 0; i < count; i++) {
		int r, c;
		Point2d P((pEdges[i]->e[0].oData->x + pEdges[i]->e[1].oData->x) / 2, (pEdges[i]->e[0].oData->y + pEdges[i]->e[1].oData->y) / 2);
		c = (P.x - LeftBound) / PixelWidth;
		r = (P.y - UpperBound) / PixelHeight;
		pEdges[i]->walkable = (pData[r * nWidth + c] == NoDataValue ? true : false);  //定义的是障碍栅格，因此值为Nodata的像元是可通行的！！
	}
}

void CGISTinView::AssignEdgeAttribute(DCEL **pEdges, const char* szFileName) {
	OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	OGRSFDriver* poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
	OGRDataSource *poDS = poDriver->Open(szFileName);
	OGRLayer *poLayer = NULL;

	OGRDataSource *poNewDS = poDriver->CreateDataSource("E:\\DynamicLayer.shp");
	OGRLayer *poNewLayer = poNewDS->CreateLayer("Lines", NULL, wkbLineString);
	OGRFieldDefn ogrField("NO", OFTInteger);
	ogrField.SetWidth(10);
	poNewLayer->CreateField(&ogrField);

	OGRDataSource *poResDS = poDriver->CreateDataSource("E:\\ResultLayer.shp");
	OGRLayer *poResLayer = poResDS->CreateLayer("Lines", NULL, wkbLineString);
	OGRFieldDefn ogrField_2("NO", OFTInteger);
	ogrField_2.SetWidth(10);
	poResLayer->CreateField(&ogrField_2);

	if (poDS == NULL)
	{
		MessageBox("open failed!\n", "Error", 0);
		return;
	}
	else
	{
		poLayer = poDS->GetLayer(0);
	}

	if (poNewDS == NULL)
	{
		MessageBox("open failed!\n", "Error", 0);
		return;
	}
	else
	{
		for (int i = 0; i < m_nDeEdgeCount; i++) {
			OGRFeature *poFeat = OGRFeature::CreateFeature(poNewLayer->GetLayerDefn());
			poFeat->SetField("NO", i);
			OGRLineString poLine;
			poLine.setNumPoints(2);
			OGRPoint P0(pEdges[i]->e[0].oData->x, pEdges[i]->e[0].oData->y);
			OGRPoint P1(pEdges[i]->e[1].oData->x, pEdges[i]->e[1].oData->y);
			poLine.setPoint(0,&P0);
			poLine.setPoint(1,&P1);
			poFeat->SetGeometry(&poLine);
			if (poNewLayer->CreateFeature(poFeat) != OGRERR_NONE) {
				AfxMessageBox("创建矢量线数据出错！");
				return;
			}
			OGRFeature::DestroyFeature(poFeat);
			OGRErr poErr = poLayer->Intersection(poNewLayer, poResLayer);
			if (poErr == OGRERR_NONE) {
				if (poResLayer->GetFeatureCount()) {
					pEdges[i]->walkable = false;
					poResLayer->DeleteFeature(0);
				}
			}
			poNewLayer->DeleteFeature(0);
		}
	}

	OGRDataSource::DestroyDataSource(poDS);
	OGRDataSource::DestroyDataSource(poNewDS);
	OGRDataSource::DestroyDataSource(poResDS);

	poDriver->DeleteDataSource("E:\\DynamicLayer.shp");
	poDriver->DeleteDataSource("E:\\ResultLayer.shp");

	
	OGRCleanupAll();
}

void CGISTinView::CreateLinePath() {
	//AssignEdgeAttribute(m_pDelaunayEdge, "E:\\快盘\\开阔空间的通行路径分析\\测试点\\shps\\grass_land.shp");
	clock_t t1 = clock();
	std::multiset<MyPoint*, MultisetLess> quePointID;
	quePointID.insert(&PointData[nStartPointID]);
	PointData[nStartPointID].visited = true;
	while (!quePointID.empty()) {
		Point2d pt((*quePointID.begin())->x, (*quePointID.begin())->y);
		int PID = mHashTable[pt]; //TODO:应该选取最小累积量的节点
		TopoPoint& CurrPoint = pTopoPointCollection[PID];
		for (int i = 0; i < CurrPoint.nLineCount; i++) {
			long LID = CurrPoint.pConnectLineIDs[i];
			DCEL *pLine = m_pDelaunayEdge[LID];
			if (pLine != NULL && pLine->walkable) {
				Point2d P0(pLine->e[0].oData->x, pLine->e[0].oData->y);
				Point2d P1(pLine->e[1].oData->x, pLine->e[1].oData->y);
				int idx1 = mHashTable[P0];
				int idx2 = mHashTable[P1];
				if (!PointData[idx1].visited) {
					double dis = sqrt(pow(PointData[idx1].x - PointData[PID].x, 2) + pow(PointData[idx1].y - PointData[PID].y, 2));
					if (PointData[idx1].parent == -1) {
						PointData[idx1].parent = PID;
						PointData[idx1].accu = dis + PointData[PID].accu;
						quePointID.insert(&PointData[idx1]);
					}
					else if (PointData[idx1].accu > dis + PointData[PID].accu) {
						PointData[idx1].accu = dis + PointData[PID].accu;
						PointData[idx1].parent = PID;
					}
				}
				if (!PointData[idx2].visited) {
					double dis = sqrt(pow(PointData[idx2].x - PointData[PID].x, 2) + pow(PointData[idx2].y - PointData[PID].y, 2));
					if (PointData[idx2].parent == -1) {
						PointData[idx2].parent = PID;
						PointData[idx2].accu = dis + PointData[PID].accu;
						quePointID.insert(&PointData[idx2]);
					}
					else if (PointData[idx2].accu > dis + PointData[PID].accu) {
						PointData[idx2].accu = dis + PointData[PID].accu;
						PointData[idx2].parent = PID;
					}
				}

			}
		}

		PointData[PID].visited = true;
		quePointID.erase(quePointID.begin());
		//if (quePointID.size() >= 2) {
		//	AccuSort(quePointID, 0, quePointID.size() - 1);
		//}

		//sort(quePointID.begin(), quePointID.end(), AccuCompare);
		//sort(quePointID.begin(), quePointID.end(), [=](long& ID1, long &ID2) {return PointData[ID1].accu <= PointData[ID2].accu; });
		//quePointID.pop();
	}
	CString cstr;

	clock_t t2 = clock();
	cstr.Format("path calcalation: %.3lf\n", double(t2 - t1) / 1000.);
	AfxMessageBox(cstr);

	cstr.Format(" ");
	int id = nEndPointID;
	int count = 1;
	while (id != nStartPointID) {
		count++;
		cstr.AppendFormat("%d\t", id);
		id = PointData[id].parent;
	}
	cstr.AppendFormat("%d\t", nStartPointID);
	AfxMessageBox(cstr);

	// 保存路径点
	if (pPathPoints) {
		delete[] pPathPoints;
	}
	pPathPoints = new MyPoint[count];
	nPathPointNum = count;
	count = 0;
	id = nEndPointID;
	while (id != nStartPointID) {
		memcpy(pPathPoints + count++, PointData + id, sizeof(MyPoint));
		id = PointData[id].parent;
	}
	memcpy(pPathPoints + count, PointData + nStartPointID, sizeof(MyPoint));
	//窗口重绘
	CRect Rect;
	GetClientRect(&Rect);
	InvalidateRect(&Rect);
}

// 按照MyPoint的accu字段进行升序排序
void CGISTinView::AccuSort(vector<int> &vec, int left, int right)
{
	int i, j;
	i = left; j = right;
	int mid = vec[(left + right) / 2];
	double m_accu = Point[mid].accu;
	do {
		while ((PointData[vec[i]].accu < m_accu) && (i < right)) i++;
		while ((PointData[vec[j]].accu > m_accu) && (j > left))  j--;
		if (i <= j)
		{
			long t = vec[i];
			vec[i] = vec[j];
			vec[j] = t;
			i++; j--;
		}
	} while (i <= j);
	if (left < j)
		AccuSort(vec, left, j);
	if (i < right)
		AccuSort(vec, i, right);
}

void CGISTinView::OnPathConstruction()
{
	//CreateTriPath();
	for (int i = 0; i < pointNumber; i++) {
		PointData[i].accu = 0;
		PointData[i].parent = -1;
		PointData[i].visited = false;
	}
	CreateLinePath();
}

// 三角网加密
//void CGISTinView::OnTinDensify()
//{
//	MyPoint *pNewPointData = NULL;
//	TRIANGLE *pNewTinHead = NULL;
//	long nNewStartPointID, nNewEndPointID;
//	nNewStartPointID = nNewEndPointID = -1;
//	vector<long> vecSave;
//	for (int i = 0; i < nPathPointNum; i++) {
//		long PID = pPathPoints[i].ID;
//		for (TRIANGLE* pTri = tinHead; pTri != NULL; pTri = pTri->next) {
//			if (pTri->ID1 == PID || pTri->ID2 == PID || pTri->ID3 == PID) {
//				if (PointData[pTri->ID1].visited) {
//
//					vecSave.push_back(pTri->ID1);
//					if (pTri->ID1 == nStartPointID) {
//						nNewStartPointID = vecSave.size() - 1;
//					}
//					if (pTri->ID1 == nEndPointID) {
//						nNewEndPointID = vecSave.size() - 1;
//					}
//					PointData[pTri->ID1].visited = false;
//				}
//				if (PointData[pTri->ID2].visited) {
//					vecSave.push_back(pTri->ID2);
//					if (pTri->ID2 == nStartPointID) {
//						nNewStartPointID = vecSave.size() - 1;
//					}
//					if (pTri->ID2 == nEndPointID) {
//						nNewEndPointID = vecSave.size() - 1;
//					}
//					PointData[pTri->ID2].visited = false;
//				}
//				if (PointData[pTri->ID3].visited) {
//					vecSave.push_back(pTri->ID3);
//					if (pTri->ID3 == nStartPointID) {
//						nNewStartPointID = vecSave.size() - 1;
//					}
//					if (pTri->ID3 == nEndPointID) {
//						nNewEndPointID = vecSave.size() - 1;
//					}
//					PointData[pTri->ID3].visited = false;
//				}
//			}
//		}
//	}
//
//
//	CString cstr;
//	cstr.Format("新点数：%d\n", vecSave.size());
//	AfxMessageBox(cstr);
//
//	cstr.Format("%d, %d\n", nStartPointID, PointData[vecSave[nNewStartPointID]].ID); //验证通过
//	AfxMessageBox(cstr);
//
//	cstr.Format("%d, %d\n", nEndPointID, PointData[vecSave[nNewEndPointID]].ID); //验证通过
//	AfxMessageBox(cstr);
//
//	nStartPointID = nNewStartPointID;
//	nEndPointID = nNewEndPointID;
//
//	pNewPointData = new MyPoint[vecSave.size()];
//	for (int i = 0; i < vecSave.size(); i++) {
//		memcpy(pNewPointData + i, PointData + vecSave[i], sizeof(MyPoint));
//		pNewPointData[i].ID = i;
//	}
//
//	delete[]PointData;
//	PointData = pNewPointData;
//	pointNumber = vecSave.size();
//
//
//	//OnTinGenerate();
//
//	CRect Rect;
//	GetClientRect(&Rect);
//	InvalidateRect(&Rect);
//
//
//}

void CGISTinView::OnTopoConstruct()
{
	PointLineTopoConstruct();
}


void CGISTinView::OnCreatePath()
{
	for (int i = 0; i < pointNumber; i++) {
		PointData[i].accu = 0;
		PointData[i].parent = -1;
		PointData[i].visited = false;
	}
	CreateLinePath();
}

void CGISTinView::OnRasterOpen()
{
	CString TheFilePath;
	CFileDialog fd(TRUE, NULL, "w001001x.adf", OFN_HIDEREADONLY | OFN_ENABLESIZING, "*.adf|*.adf|", AfxGetMainWnd());
	if (fd.DoModal() == IDOK) {
		TheFilePath = fd.GetFolderPath();
	}
	else
		return;

    MyDataPackage *pDataPackage = ReadRasterData(TheFilePath);
	
	switch (pDataPackage->nDataType)
	{
	case 1:
		AssignEdgeAttribute<DT_8U>(m_pDelaunayEdge, m_nDeEdgeCount, pDataPackage);
		break;
	case 2:
		AssignEdgeAttribute<DT_16U>(m_pDelaunayEdge, m_nDeEdgeCount, pDataPackage);
		break;
	case 3:
		AssignEdgeAttribute<DT_16S>(m_pDelaunayEdge, m_nDeEdgeCount, pDataPackage);
		break;
	case 4:
		AssignEdgeAttribute<DT_32U>(m_pDelaunayEdge, m_nDeEdgeCount, pDataPackage);
		break;
	case 5:
		AssignEdgeAttribute<DT_32S>(m_pDelaunayEdge, m_nDeEdgeCount, pDataPackage);
		break;
	case 6:
		AssignEdgeAttribute<DT_32F>(m_pDelaunayEdge, m_nDeEdgeCount, pDataPackage);
		break;
	default:
		AssignEdgeAttribute<DT_64F>(m_pDelaunayEdge, m_nDeEdgeCount, pDataPackage);
		break;
	}

	CRect Rect;
	GetClientRect(&Rect);
	InvalidateRect(&Rect);
}
