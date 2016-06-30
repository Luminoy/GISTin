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
	ON_COMMAND(ID_PATH_SMOOTH, &CGISTinView::OnPathSmooth)
	ON_COMMAND(ID_POINT_DENSIFY, &CGISTinView::OnPointDensify)
	ON_COMMAND(ID_SAVE_POINT, &CGISTinView::OnSavePoint)
	ON_COMMAND(ID_SAVE_LINE, &CGISTinView::OnSaveLine)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGISTinView construction/destruction


MyDataPackage* CGISTinView::ReadRasterData(const char *filename) {
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
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

void CGISTinView::ReadShapefile(const char *fileName, char *fieldName) {
	OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	OGRSFDriver* poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
	OGRDataSource *poDS = poDriver->Open(fileName);
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

					OGRFeature *pFeat = (OGRFeature *)poPoint;
					double attr = 1;
					if (fieldName) {
						attr = pFeat->GetFieldAsDouble(fieldName);
					}

					PNT POINT;
					POINT.x = poPoint->getX();
					POINT.y = poPoint->getY();

					//PNTSet.push_back(POINT);
					group.push_back(POINT);

					m_vecInputSHPGroups.push_back(make_pair(group, attr));

					//char str[100];
					//sprintf_s(str, "%d : (%f, %f)\n", idx, poPoint->getX(), poPoint->getY());
					//MessageBoxA(NULL, str, "coordinate", 0);
				}
				else if (wkbFlatten(poGeometry->getGeometryType()) == wkbLineString)
				{
					OGRLineString *poLine = (OGRLineString *)poGeometry;
					pnt_count = poLine->getNumPoints();
					vector<PNT> group(pnt_count);
					OGRFeature *pFeat = (OGRFeature *)poLine;
					double attr = 1;
					if (fieldName) {
						attr = pFeat->GetFieldAsDouble(fieldName);
					}

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
					m_vecInputSHPGroups.push_back(make_pair(group, attr));

				}
				else if (wkbFlatten(poGeometry->getGeometryType()) == wkbPolygon) {
					OGRPolygon *poPolygon = (OGRPolygon *)poGeometry;
					OGRLinearRing* poRing = poPolygon->getExteriorRing();

					pnt_count = poRing->getNumPoints();
					OGRPoint *poPoint = new OGRPoint();
					vector<PNT> group;

					OGRFeature *pFeat = (OGRFeature *)poPolygon;
					OGRFeature *poFeature = poFeat;
					OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();

					double attr = 1;
					if (fieldName) {
						OGRFeatureDefn *poFeatDefn = pFeat->GetDefnRef();
						int iField = poFDefn->GetFieldIndex(fieldName);
						OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(iField);
						if (poFieldDefn->GetType() == OFTInteger)
							attr = poFeature->GetFieldAsInteger(iField);
						else if (poFieldDefn->GetType() == OFTReal)
							attr = poFeature->GetFieldAsDouble(iField);
					}

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
					m_vecInputSHPGroups.push_back(make_pair(group,attr));

					int nRings = poPolygon->getNumInteriorRings();
					for (int k = 0; k < nRings; k++) {
						poRing = poPolygon->getInteriorRing(k);
						pnt_count = poRing->getNumPoints();

						OGRPoint *poPoint = new OGRPoint();
						vector<PNT> group;

						OGRFeature *pFeat = (OGRFeature *)poPolygon;
						OGRFeature *poFeature = poFeat;
						OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();

						double attr = 1;
						if (fieldName) {
							OGRFeatureDefn *poFeatDefn = pFeat->GetDefnRef();
							int iField = poFDefn->GetFieldIndex(fieldName);
							OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(iField);
							if (poFieldDefn->GetType() == OFTInteger)
								attr = poFeature->GetFieldAsInteger(iField);
							else if (poFieldDefn->GetType() == OFTReal)
								attr = poFeature->GetFieldAsDouble(iField);
						}

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
						m_vecInputSHPGroups.push_back(make_pair(group, attr));
					}
					
				}
				else if (poGeometry->getGeometryType() == wkbMultiPolygon25D) {  //加了wkbFlatten居然会导致判断为不等于！！！
					OGRMultiPolygon *poMultiPolygon = (OGRMultiPolygon *)poGeometry;

					for (int k = 0; k < poMultiPolygon->getNumGeometries(); k++) {
						OGRPolygon *poPolygon = (OGRPolygon *)poMultiPolygon->getGeometryRef(k);
						OGRLinearRing *poRing = (OGRLinearRing *)poPolygon->getExteriorRing();
						pnt_count = poRing->getNumPoints();

						OGRFeature *pFeat = (OGRFeature *)poPolygon;
						OGRFeature *poFeature = pFeat;
						OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
						double attr = 1;
						if (fieldName) {
							OGRFeatureDefn *poFeatDefn = pFeat->GetDefnRef();
							int iField = poFDefn->GetFieldIndex(fieldName);
							OGRFieldDefn *poFieldDefn = poFDefn->GetFieldDefn(iField);
							if (poFieldDefn->GetType() == OFTInteger)
								attr = poFeature->GetFieldAsInteger(iField);
							else if (poFieldDefn->GetType() == OFTReal)
								attr = poFeature->GetFieldAsDouble(iField);
						}

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
						m_vecInputSHPGroups.push_back(make_pair(group,attr));
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

		OGRDataSource::DestroyDataSource(poDS);
		OGRCleanupAll();
	}
}

vector<PNT> CGISTinView::SplitLongSegments(vector<pair<vector<PNT>, double> >& m_vecInputSHPGroups)
{
	vector<PNT> PNTSet;
	// 长线段的切割
	for (int v = 0; v < m_vecInputSHPGroups.size(); v++) {
		vector<PNT>& group = m_vecInputSHPGroups[v].first;
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
	return PNTSet;
}

bool pntset_cmp(const PNT &P1, const PNT &P2) {
	return (P1.x < P2.x) || ((P1.x == P2.x) && (P1.y < P2.y));
}

bool unique_cmp(const PNT &P1, const PNT &P2) {
	return (P1.x == P2.x) && (P1.y == P2.y);
}

bool unique_shedhold(const PNT &P1, const PNT &P2) {
	return ((abs(P1.x - P2.x) <= 0.3) || (abs(P1.y - P2.y) <= 0.3));
}

void CGISTinView::ElimiateDuplicatePoints(vector<PNT> &PNTSet) {
	int prev_count = PNTSet.size();
	sort(PNTSet.begin(), PNTSet.end(), pntset_cmp);

	vector<PNT>::iterator iter = unique(PNTSet.begin(), PNTSet.end(), unique_shedhold);
	PNTSet.erase(iter, PNTSet.end());

	CString cstr;
	cstr.AppendFormat("delta: %d", prev_count - PNTSet.size());
	AfxMessageBox(cstr);
}

void CGISTinView::SavePointsToTextFile(const char *filename, MyPoint* pData, int count) {
	double xmin, xmax, ymin, ymax;
	xmax = xmin = pData[0].x;
	ymax = ymin = pData[0].y;
	for (int i = 1; i < count; i++) {
		if (xmax < pData[i].x) {
			xmax = pData[i].x;
		}
		if (xmin > pData[i].x) {
			xmin = pData[i].x;
		}
		if (ymax < pData[i].y) {
			ymax = pData[i].y;
		}
		if (ymin > pData[i].y) {
			ymin = pData[i].y;
		}
	}
	FILE *fp = NULL;
	fopen_s(&fp, filename, "wb+");
	if (fp == NULL) {
		AfxMessageBox("Write File Error！");
		return;
	}
	fprintf_s(fp, "%d\r\n", count);
	for (int i = 1; i <= count; i++) {
		fprintf_s(fp, "%d %.3lf %.3lf\r\n", i, pData[i - 1].x - xmin, pData[i - 1].y - ymin);
	}
	fclose(fp);
}


void CGISTinView::SaveShapeFile(const char *filename, MyPoint* pData, int count) {
	::OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
	OGRDataSource *poDS = poDriver->CreateDataSource(filename);
	OGRLayer *poLayer = poDS->CreateLayer(filename, NULL, wkbPoint);
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

void CGISTinView::SaveShapeFile(const char *filename, DCEL** pData, int count) {
	::OGRRegisterAll();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
	OGRDataSource *poDS = poDriver->CreateDataSource(filename);
	OGRLayer *poLayer = poDS->CreateLayer(filename, NULL, wkbLineString);
	OGRFieldDefn ogrField("NO", OFTInteger);
	ogrField.SetWidth(10);
	poLayer->CreateField(&ogrField);

	for (int i = 0; i < count; ++i) {
		OGRFeature *poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
		poFeature->SetField("NO", i);
		OGRLineString pLine;
		OGRPoint P0, P1;

		P0.setX(pData[i]->e[0].oData->x);
		P0.setY(pData[i]->e[0].oData->y);

		P1.setX(pData[i]->e[1].oData->x);
		P1.setY(pData[i]->e[1].oData->y);

		pLine.setNumPoints(2);
		pLine.setPoint(0, &P0);
		pLine.setPoint(1, &P1);

		poFeature->SetGeometry(&pLine);

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
	pDataPackage = NULL;
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
		pid = -1;
		for (int i = 0; i < pointNumber; i++) {
			if ((abs(mpt2.x - PointData[i].x) < 0.1) && (abs(mpt2.y - PointData[i].y) < 0.1)) {
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

void CGISTinView::DrawRasterLayer(MyDataPackage *pDataPackage) {
	
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
		if (pdecl->resistance) {
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

void CGISTinView::DrawPolygonFromPointGroups(CDC * pDC, vector<pair<vector<PNT>, double> >& vecPointGroups)
{
	if (vecPointGroups.empty())	return;
	CPen Pen;
	Pen.CreatePen(PS_SOLID, 1, colors[PURPLE]);
	

	CPen *pOldPen = pDC->SelectObject(&Pen);
	
	
	for (int i = 0; i < vecPointGroups.size(); i++) {
		continue;
		vector<PNT> &group = vecPointGroups[i].first;
		if (vecPointGroups[i].second != 3.0) {
			continue;
		}
		CBrush Brush;
		Brush.CreateSolidBrush(colors[(int)vecPointGroups[i].second]);
		CBrush *pOldBrush = pDC->SelectObject(&Brush);
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
		else
		{

		}
		pDC->EndPath();
		pDC->FillPath();
		pDC->SelectObject(pOldBrush);
	}
	pDC->SelectObject(pOldPen);
	
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

	m_pDelaunayEdge = new DCEL*[pointNumber * 3];
	//3.收集边
	if (maxEdge.le != NULL)
	{
	    m_nDeEdgeCount = 0; 		
        collectDcel(maxEdge.le, m_pDelaunayEdge, m_nDeEdgeCount);
		collectDcel(maxEdge.re, m_pDelaunayEdge, m_nDeEdgeCount);
	}

	CString str;
	str.AppendFormat("点数: %d .\n读取文件时间: %.4f s.\n构建三角网时间: %.4f s.\n", pointNumber, m_dReadFileTime, duration);
	AfxMessageBox(str);

	PointLineTopoConstruct();
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
	ReadShapefile(TheFileName, "Id");
	vector<PNT> PNTSet = SplitLongSegments(m_vecInputSHPGroups);
	ElimiateDuplicatePoints(PNTSet);
	ElimiateDuplicatePoints(PNTSet);

	//random_shuffle(PNTSet.begin(), PNTSet.end());
	//CalPointDistance(PNTSet); //去重
	//CalPointDistance(PNTSet);
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

void CGISTinView::OnSavePoint()
{
	CString  TheFileName;
	CFileDialog  FileDlg(TRUE, NULL, "dendify.shp", OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_OVERWRITEPROMPT, "*.shp|*.shp|", AfxGetMainWnd());

	if (FileDlg.DoModal() == IDOK)
		TheFileName = FileDlg.GetPathName();
	else
		return;

	SavePointsToTextFile(TheFileName, PointData, pointNumber);
	//SaveShapeFile(TheFileName, PointData, pointNumber);
}


void CGISTinView::OnSaveLine()
{
	CString  TheFileName;
	CFileDialog  FileDlg(TRUE, NULL, "edges.shp", OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_OVERWRITEPROMPT, "*.shp|*.shp|", AfxGetMainWnd());

	if (FileDlg.DoModal() == IDOK)
		TheFileName = FileDlg.GetPathName();
	else
		return;

	SaveShapeFile(TheFileName, m_pDelaunayEdge, m_nDeEdgeCount);
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
	mHashTable.clear();
	pTopoPointCollection.Destroy();
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
	int r0, c0, r1, c1;
	for (int i = 0; i < count; i++) {
		//c0 = (pEdges[i]->e[0].oData->x - LeftBound) / PixelWidth;
		//r0 = (pEdges[i]->e[0].oData->y - UpperBound) / PixelHeight;
		//c1 = (pEdges[i]->e[1].oData->x - LeftBound) / PixelWidth;
		//r1 = (pEdges[i]->e[1].oData->y - UpperBound) / PixelHeight;
		//c0 = (684324.035 - LeftBound) / PixelWidth;
		//r0 = (3554648.483 - UpperBound) / PixelHeight;
		//c1 = (684331.134 - LeftBound) / PixelWidth;
		//r1 = (3554650.067 - UpperBound) / PixelHeight;
		//c0 = (684377.903 - LeftBound) / PixelWidth;
		//r0 = (3554832.251 - UpperBound) / PixelHeight;
		//c1 = (684387.140 - LeftBound) / PixelWidth;
		//r1 = (3554833.909 - UpperBound) / PixelHeight;

		double resistance = 0;
		const int delta = 2;

		//vector<int> resist;
		//resist.resize(2 * delta + 1);
		int *resist = new int[(2 * delta + 1) * (2 * delta + 1)];
		int mr = ((pEdges[i]->e[0].oData->y + pEdges[i]->e[1].oData->y) / 2 - UpperBound) / PixelHeight;
		int mc = ((pEdges[i]->e[0].oData->x + pEdges[i]->e[1].oData->x) / 2 - LeftBound) / PixelWidth;
		
		for (int k = -delta; k <= delta; k++) {
			for (int m = -delta; m <= delta; m++) {
				if (mr + k < 0 || mr + k > nHeight - 1 || mc + m < 0 || mc + m > nWidth - 1)
					resist[(k + delta) * (2 * delta + 1) + m + delta] = 1;
				else
					resist[(k + delta) * (2 * delta + 1) + m + delta] = (pData[(mr + k) * nWidth + (mc + m)] != NoDataValue) ? 1 : 0;
			}
		}

		// 用map作为统计容器
		map<double, int> stat;
		for (int i = 0; i < (2 * delta + 1) * (2 * delta + 1); i++) {
			stat[resist[i]]++;
		}
		if (stat.size() == 1) {
			resistance = stat.begin()->first;
		}
		else if(stat.size() > 1)
		{
			resistance = INT_MAX;
			for (map<double, int>::iterator iter = stat.begin(); iter != stat.end(); iter++) {
				if (resistance > iter->first) {
					resistance = iter->first;
				}
			}
		}
		
		//int count_0 = count_if(resist.begin(), resist.end(), [=]->(int a) { return a == 0; });
		//int resist_0 = 0, resist_1 = 0;
		//for (int i = 0; i < (2 * delta + 1)*(2 * delta + 1); i++) {
		//	
		//	if (resist[i] == 0) {
		//		resist_0++;
		//	}
		//	if (resist[i] == 1) {
		//		resist_1++;
		//	}
		//}
		//
		//if (resist_0 == 0) {
		//	resistance = 1;
		//}
		//else
		//{
		//	resistance = 0;
		//}

		pEdges[i]->resistance = resistance;  //定义的是障碍栅格，因此值为Nodata的像元是可通行的！！
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
					pEdges[i]->resistance = 1;
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
			if (pLine != NULL && !pLine->resistance) {
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
	cstr.Format("path calcalation: %.3lf s.\n", double(t2 - t1) / 1000.);
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

    pDataPackage = ReadRasterData(TheFilePath);
	
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


void CGISTinView::OnPathSmooth()
{
	switch (pDataPackage->nDataType)
	{
	case 1:
		PathOptimize<DT_8U>(pPathPoints, nPathPointNum, pDataPackage);
		break;
	case 2:
		PathOptimize<DT_16U>(pPathPoints, nPathPointNum, pDataPackage);
		break;
	case 3:
		PathOptimize<DT_16S>(pPathPoints, nPathPointNum, pDataPackage);
		break;
	case 4:
		PathOptimize<DT_32U>(pPathPoints, nPathPointNum, pDataPackage);
		break;
	case 5:
		PathOptimize<DT_32S>(pPathPoints, nPathPointNum, pDataPackage);
		break;
	case 6:
		PathOptimize<DT_32F>(pPathPoints, nPathPointNum, pDataPackage);
		break;
	default:
		PathOptimize<DT_64F>(pPathPoints, nPathPointNum, pDataPackage);
		break;
	}


	CRect Rect;
	GetClientRect(&Rect);
	InvalidateRect(&Rect);
}


template<typename DT>
void CGISTinView::PathOptimize(MyPoint *pPath, int nPointCount, MyDataPackage *pPackage) {
	if (nPointCount <= 2)	return;

	int nWidth = pPackage->nWidth;
	int nHeight = pPackage->nHeight;
	float UpperBound = pPackage->fUpperBound;
	float LeftBound = pPackage->fLeftBound;
	float PixelWidth = pPackage->fPixelWidth;
	float PixelHeight = pPackage->fPixelHeight;

	MyPoint *pNewPoints = new MyPoint[nPathPointNum];
	int nNewPointNum = 0;
	DT *pData = static_cast<DT *>(pPackage->pData);
	DT NodataValue = static_cast<DT>(pPackage->dNoDataValue);
	memcpy(pNewPoints + nNewPointNum++, pPath + nPointCount - 1 , sizeof(MyPoint));
	int r0, c0, r1, c1;
	for (int i = nPointCount - 2; i >= 0; i--) {
		c0 = (pNewPoints[nNewPointNum - 1].x - LeftBound) / PixelWidth;
		r0 = (pNewPoints[nNewPointNum - 1].y - UpperBound) / PixelHeight;

		c1 = (pPath[i].x - LeftBound) / PixelWidth;
		r1 = (pPath[i].y - UpperBound) / PixelHeight;

		if (DDA_Line_2<DT>(r0, c0, r1, c1, pData, NodataValue, nWidth, nHeight)) {
			continue;
		}
		memcpy(pNewPoints + nNewPointNum++, pPath + i + 1, sizeof(MyPoint));
	}
	memcpy(pNewPoints + nNewPointNum++, pPath, sizeof(MyPoint));

	delete[] pPathPoints;
	pPathPoints = new MyPoint[nNewPointNum];
	memcpy(pPathPoints, pNewPoints, nNewPointNum * sizeof(MyPoint));
	nPathPointNum = nNewPointNum;;
	delete[] pNewPoints;
}
//
//template<typename DT>
//bool CGISTinView::LineOfSight(MyPoint& l1, MyPoint& l2, DT *pData, int nWidth, int nHeight)
//{
//	// This line of sight check uses only integer values. First it checks whether the movement along the x or the y axis is longer and moves along the longer
//	// one cell by cell. dx and dy specify how many cells to move in each direction. Suppose dx is longer and we are moving along the x axis. For each
//	// cell we pass in the x direction, we increase variable f by dy, which is initially 0. When f >= dx, we move along the y axis and set f -= dx. This way,
//	// after dx movements along the x axis, we also move dy moves along the y axis.
//
//	// x and y values correspond to corners, not cells.
//	int x1 = l1.x; // Originate from this cell.
//	int y1 = l1.y;
//
//	int x2 = l2.x; // Move to this cell.
//	int y2 = l2.y;
//
//	int dy = l2.y - l1.y;
//	int dx = l2.x - l1.x;
//
//	int f = 0;
//	int sy, sx; // Direction of movement. Value can be either 1 or -1.
//
//				// The x and y locations correspond to corners, not cells. We might need to check different surrounding cells depending on the direction we do the
//				// line of sight check. The following values are usedto determine which cell to check to see if it is unblocked.
//	int x_offset, y_offset;
//
//	if (dy < 0) {
//		dy = -dy;
//		sy = -1;
//		y_offset = 0; // Cell is to the North
//	}
//	else {
//		sy = 1;
//		y_offset = 1; // Cell is to the South
//	}
//
//	if (dx < 0) {
//		dx = -dx;
//		sx = -1;
//		x_offset = 0; // Cell is to the West
//	}
//	else {
//		sx = 1;
//		x_offset = 1; // Cell is to the East
//	}
//
//	if (dx >= dy) { // Move along the x axis and increment/decrement y when f >= dx.
//		while (x1 != x2) {
//			f = f + dy;
//			if (f >= dx) {  // We are changing rows, we might need to check two cells this iteration.
//				if (!pData[(x1 + x_offset) * nWidth + (y1 + y_offset)])
//					return false;
//
//				y1 = y1 + sy;
//				f = f - dx;
//			}
//
//			if (f != 0) {   // If f == 0, then we are crossing the row at a corner point and we don't need to check both cells.
//				if (!pData[(x1 + x_offset) * nWidth + (y1 + y_offset)])
//					return false;
//			}
//
//			if (dy == 0) {  // If we are moving along a horizontal line, either the north or the south cell should be unblocked.
//				if (!pData[(x1 + x_offset) * nWidth + y1] && !pData[(x1 + x_offset) * nWidth + (y1 + 1)])
//					return false;
//			}
//
//			x1 += sx;
//		}
//	}
//
//	else {  //if (dx < dy). Move along the y axis and increment/decrement x when f >= dy.
//		while (y1 != y2) {
//			f = f + dx;
//			if (f >= dy) {
//				if (!pData[(x1 + x_offset) * nWidth + (y1 + y_offset)])
//					return false;
//
//				x1 = x1 + sx;
//				f = f - dy;
//			}
//
//			if (f != 0) {
//				if (!pData[(x1 + x_offset) * nWidth + (y1 + y_offset)])
//					return false;
//			}
//
//			if (dx == 0) {
//				if (!pData[x1 * nWidth + (y1 + y_offset)] && !pData[(x1 + 1) * nWidth + (y1 + x_offset)])
//					return false;
//			}
//
//			y1 += sy;
//		}
//	}
//	return true;
//}

template<typename DT>
double CGISTinView::DDA_Line_2(int curr_x, int curr_y, int parent_x, int parent_y, DT* space, DT &NodataValue, int nWidth, int nHeight)
{
	int x1 = curr_x, y1 = curr_y, x2 = parent_x, y2 = parent_y;
	double k, dx, dy, x, y, xend, yend;
	double resist_0 = 0, resist_1 = 0;

	dx = x2 - x1;
	dy = y2 - y1;
	//if (curr_x == parent_x && curr_y == parent_y) return false;
	if (abs(dx) >= abs(dy))
	{
		k = dy / dx;
		if (dx > 0)
		{
			x = x1;
			y = y1;
			xend = x2;
		}
		else
		{
			x = x2;
			y = y2;
			xend = x1;
		}
		while (x <= xend)
		{
			if (y < 0 || y > nWidth - 1)	break;
			if (space[(int)x * nWidth + (int)floor(y)] != NodataValue) {
				resist_1++;
			}
			else
			{
				resist_0++;
			}

			y = y + k;
			x = x + 1;
			
		}
	}
	else
	{
		k = dx / dy;
		if (dy > 0)
		{
			x = x1;
			y = y1;
			yend = y2;
		}
		else
		{
			x = x2;
			y = y2;
			yend = y1;
		}
		while (y <= yend)
		{
			if (x < 0 || x > nHeight - 1)	break;
			if (space[(int)floor(x) * nWidth + (int)y] != NodataValue) {
				resist_1++;
			}
			else
			{
				resist_0++;
			}
			x = x + k;
			y = y + 1;
		}
	}
	return resist_0 > resist_1 ? 0 : 1;
}

template<typename DT>
double CGISTinView::Bresenham(int x1, int y1, int x2, int y2, DT* space, DT &NodataValue, int nWidth, int nHeight)
{
	if ((x1 < 0 || x1 > nWidth - 1) ||
		(x2 < 0 || x2 > nWidth - 1) ||
		(y1 < 0 || y1 > nHeight - 1) ||
		(y2 < 0 || y2 > nHeight - 1)
		)
	{
		return 1;
	}

	int startX, startY, endX, endY;
	double k, dx, dy, x, y, xend, yend;
	double resist_0 = 0, resist_1 = 0;

	int sign_x = 1, sign_y = 1;

	
	dx = x2 - x1;
	dy = y2 - y1;
	
	//if (curr_x == parent_x && curr_y == parent_y) return false;
	if (abs(dx) >= abs(dy))
	{
		k = dy / dx;
		if (dx > 0)
		{
			startX = x1;
			startY = y1;
			endX = x2;
			endY = y2;
		}
		else
		{
			startX = x2;
			startY = y2;
			endX = x1;
			endY = y1;
		}
		sign_y = endY > startY ? 1 : -1;
		for (x = startX, y = startY; x <= endX; ++x)
		{
			if (x - startX != 0)
			{
				// 计算当前斜率
				double currentK = abs((y - startY) / (x - startX));

				// 如果当前斜率 < k, 则增加y坐标
				if (currentK < abs(k))
				{
					y = y + sign_y;
				}
			}
			if (space[(int)x * nWidth + (int)y] != NodataValue) {
				resist_1++;
			}
			else
			{
				resist_0++;
			}
			//drawPixel(x, y);
		}
	}
	else
	{
		k = dx / dy;
		if (dy > 0)
		{
			startX = x1;
			startY = y1;
			endX = x2;
			endY = y2;
		}
		else
		{
			startX = x2;
			startY = y2;
			endX = x1;
			endY = y1;
		}
		sign_x = endX > startX ? 1 : -1;
		for (y = startY, x = startX; y <= endY; ++y)
		{
			if (y - startY != 0)
			{
				// 计算当前斜率
				double currentK = abs((x - startX) / (y - startY));

				// 如果当前斜率 < k, 则增加y坐标
				if (currentK < abs(k))
				{
					x = x + sign_x;
				}
			}
			if (space[(int)x * nWidth + (int)y] != NodataValue) {
				resist_1++;
			}
			else
			{
				resist_0++;
			}
			//drawPixel(x, y);
		}
	}
	return resist_0 >= resist_1 ? 0 : 1;
/*	if ((x1 < 0 || x1 > nWidth - 1) ||
		(x2 < 0 || x2 > nWidth - 1) ||
		(y1 < 0 || y1 > nHeight - 1)||
		(y2 < 0 || y2 > nHeight - 1)
		)
	{
		return 1;
	}
	int startX, startY, endX, endY;

	int deltaX = x2 - x1;
	int deltaY = y2 - y1;

	
	double resist_0 = 0, resist_1 = 0;

	if (abs(deltaX) > abs(deltaY)) {
		if (x1 > x2) 
		{
			startX = x2;
			startY = y2;
			endX = x1;
			endY = y1;
		}
		else
		{
			startX = x1;
			startY = y1;
			endX = x2;
			endY = y2;
		}

		double k = abs(deltaY / deltaX);

		for (double x = startX, y = startY; x <= endX; ++x)
		{
			if (x - startX != 0)
			{
				// 计算当前斜率
				double currentK = abs((y - startY) / (x - startX));

				// 如果当前斜率 < k, 则增加y坐标
				if (currentK < k)
				{
					y = y + sign_y;
				}
			}
			if (space[(int)x * nWidth + (int)y] != NodataValue) {
				resist_1++;
			}
			else
			{
				resist_0++;
			}
			//drawPixel(x, y);
		}
	}
	else
	{
		if (y1 > y2) {
			startX = x2;
			startY = y2;
			endX = x1;
			endY = y1;
		}
		else
		{
			startX = x1;
			startY = y1;
			endX = x2;
			endY = y2;
		}

		double k = deltaX / deltaY;
		double resist_0 = 0, resist_1 = 0;

		for (double x = startX, y = startY; y <= endY; ++y)
		{
			if (y - startY != 0)
			{
				// 计算当前斜率
				double currentK = abs((x - startX) / (y - startY));

				// 如果当前斜率 < k, 则增加y坐标
				if (currentK < k)
				{
					x = x + sign_x;
				}
			}
			if (space[(int)x * nWidth + (int)y] != NodataValue) {
				resist_1++;
			}
			else
			{
				resist_0++;
			}
			//drawPixel(x, y);
		}
	}

	return resist_0 >= resist_1 ? 0 : 1;*/
}

// 貌似该函数有点问题！
//template<typename DT>
//bool CGISTinView::LineOfSight(int x1, int y1, int x2, int y2, DT *pData, int nWidth, int nHeight)
//{
//	// This line of sight check uses only integer values. First it checks whether the movement along the x or the y axis is longer and moves along the longer
//	// one cell by cell. dx and dy specify how many cells to move in each direction. Suppose dx is longer and we are moving along the x axis. For each
//	// cell we pass in the x direction, we increase variable f by dy, which is initially 0. When f >= dx, we move along the y axis and set f -= dx. This way,
//	// after dx movements along the x axis, we also move dy moves along the y axis.
//
//	// x and y values correspond to corners, not cells.
//
//	int dy = y2 - y1;
//	int dx = x2 - x1;
//
//	int f = 0;
//	int sy, sx; // Direction of movement. Value can be either 1 or -1.
//
//				// The x and y locations correspond to corners, not cells. We might need to check different surrounding cells depending on the direction we do the
//				// line of sight check. The following values are usedto determine which cell to check to see if it is unblocked.
//	int x_offset, y_offset;
//
//	if (dy < 0) {
//		dy = -dy;
//		sy = -1;
//		y_offset = 0; // Cell is to the North
//	}
//	else {
//		sy = 1;
//		y_offset = 1; // Cell is to the South
//	}
//
//	if (dx < 0) {
//		dx = -dx;
//		sx = -1;
//		x_offset = 0; // Cell is to the West
//	}
//	else {
//		sx = 1;
//		x_offset = 1; // Cell is to the East
//	}
//
//	if (dx >= dy) { // Move along the x axis and increment/decrement y when f >= dx.
//		while (x1 != x2) {
//			f = f + dy;
//			if (f >= dx) {  // We are changing rows, we might need to check two cells this iteration.
//				if (pData[(x1 + x_offset) * nWidth + (y1 + y_offset)])
//					return false;
//
//				y1 = y1 + sy;
//				f = f - dx;
//			}
//
//			if (f != 0) {   // If f == 0, then we are crossing the row at a corner point and we don't need to check both cells.
//				if (pData[(x1 + x_offset) * nWidth + (y1 + y_offset)])
//					return false;
//			}
//
//			if (dy == 0) {  // If we are moving along a horizontal line, either the north or the south cell should be unblocked.
//				if (pData[(x1 + x_offset) * nWidth + y1] || pData[(x1 + x_offset) * nWidth + (y1 + 1)])
//					return false;
//			}
//
//			x1 += sx;
//		}
//	}
//
//	else {  //if (dx < dy). Move along the y axis and increment/decrement x when f >= dy.
//		while (y1 != y2) {
//			f = f + dx;
//			if (f >= dy) {
//				if (pData[(x1 + x_offset) * nWidth + (y1 + y_offset)])
//					return false;
//
//				x1 = x1 + sx;
//				f = f - dy;
//			}
//
//			if (f != 0) {
//				if (pData[(x1 + x_offset) * nWidth + (y1 + y_offset)])
//					return false;
//			}
//
//			if (dx == 0) {
//				if (pData[x1 * nWidth + (y1 + y_offset)] || pData[(x1 + 1) * nWidth + (y1 + x_offset)])
//					return false;
//			}
//
//			y1 += sy;
//		}
//	}
//	return true;
//}

void CGISTinView::GenerateRandomPoint(MyPoint *P1, MyPoint *P2, int &x, int &y) {
	int dx = abs(P1->x - P2->x);
	int dy = abs(P1->y - P2->y);

	x = P1->x + rand() % dx;
	y = P1->y + rand() % dy;
}

void CGISTinView::GenerateRandomPoint(double x0, double y0, double x1, double y1, double &x, double &y) {
	double dx = x1 - x0;
	double dy = y1 - y0;

	x = x0 + dx / 2;
	y = y0 + dy / 2;

	//if (dx > 1e-3)
	//	x = x0 + modf((double)rand(), &dx);
	//else
	//	x = x0;
	//if (dy > 1e-3)
	//	y = y0 + modf((double)rand(), &dy);
	//else
	//	y = y0;
}

inline double CGISTinView::DistanceOfTwoPoints(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void CGISTinView::OnPointDensify()
{
	// TODO: 在此添加命令处理程序代码
	bool *visited = new bool[pointNumber];
	memset(visited, 0, pointNumber * sizeof(bool));
	vector<Point2d *> vecPoints;
	for (int i = 0; i < nPathPointNum; i++) {
		MyPoint& pPoint = pPathPoints[i];
		Point2d point(pPoint.x, pPoint.y);
		int PID = mHashTable[point];
		//visited[PID] = true;           //bug!! 导致终止点未加入点集！！
		for (int k = 0; k < pTopoPointCollection[PID].nLineCount; k++) {
			int LID = pTopoPointCollection[PID].pConnectLineIDs[k];
			DCEL *pEdge = m_pDelaunayEdge[LID];
			point.x = pEdge->e[1].oData->x;
			point.y = pEdge->e[1].oData->y;
			int P1 = mHashTable[point];
			point.x = pEdge->e[0].oData->x;
			point.y = pEdge->e[0].oData->y;
			int P2 = mHashTable[point];

			if (!visited[P1]) {
				if (pEdge->e[0].oData->x == pPathPoints[i].x && pEdge->e[0].oData->y == pPathPoints[i].y) {
					vecPoints.push_back(pEdge->e[1].oData);
					visited[P1] = true;
				}
			}
			if(!visited[P2]) {
				if (pEdge->e[1].oData->x == pPathPoints[i].x && pEdge->e[1].oData->y == pPathPoints[i].y) {
					vecPoints.push_back(pEdge->e[0].oData);
					visited[P2] = true;
				}
			}
		}
	}

	int origin_num = vecPoints.size();
	int total_num = origin_num * 5;
	MyPoint *pNewPoints = new MyPoint[total_num];

	CString str1;
	str1.Format("EndPoint( %d ): (%.3lf, %.3lf), (%.3lf, %.3lf)\n", nEndPointID, pPathPoints[0].x, pPathPoints[0].y, PointData[nEndPointID].x, PointData[nEndPointID].y);
	AfxMessageBox(str1);

	str1.Format("StartPoint( %d ): (%.3lf, %.3lf), (%.3lf, %.3lf)\n", nStartPointID, pPathPoints[nPathPointNum - 1].x, pPathPoints[nPathPointNum - 1].y, PointData[nStartPointID].x, PointData[nStartPointID].y);
	AfxMessageBox(str1);

	for (int i = 0; i < vecPoints.size(); i++) {
		pNewPoints[i].x = vecPoints[i]->x;
		pNewPoints[i].y = vecPoints[i]->y;

		//重新确定起点与终点的ID
		if (vecPoints[i]->x == pPathPoints[0].x && vecPoints[i]->y == pPathPoints[0].y) {
			nEndPointID = i;
		}
		if (vecPoints[i]->x == pPathPoints[nPathPointNum - 1].x && vecPoints[i]->y == pPathPoints[nPathPointNum - 1].y) {
			nStartPointID = i;
		}
	}

	//nStartPointID = vecPoints.size() - 1;
	//nEndPointID = nPathPointNum + vecPoints.size() - 1;

	double x, y;
	for (int i = origin_num; i < total_num; ) {
		int idx1 = rand() % i;
		int idx2 = rand() % i;
		if (idx1 == idx2) {
			continue;
		}
		double x0 = pNewPoints[idx1].x;
		double y0 = pNewPoints[idx1].y;
		double x1 = pNewPoints[idx2].x;
		double y1 = pNewPoints[idx2].y;
		GenerateRandomPoint(x0, y0, x1, y1, x, y);
		int k = 0;
		for (; k < i; k++) {
			double dis = DistanceOfTwoPoints(pNewPoints[k].x, pNewPoints[k].y, x, y);
			if (dis < MIN_DIS_VALUE)
				break;
		}
		if (k != i)	continue;
		pNewPoints[i].x = x;
		pNewPoints[i++].y = y;
	}
	
	
	delete[]PointData;
	PointData = pNewPoints;
	pointNumber = total_num;

	CString cstr;
	cstr.Format("total_num: %d\n", total_num);
	AfxMessageBox(cstr);

	CRect Rect;
	GetClientRect(&Rect);
	InvalidateRect(&Rect);
}

//HBITMAP CopyScreenToBitmap(LPRECT lpRect)
//{
//	HDC hScrDC, hMemDC;
//	// 屏幕和内存设备描述表
//	HBITMAP hBitmap, hOldBitmap;
//	// 位图句柄
//	int        nX, nY, nX2, nY2;
//	// 选定区域坐标
//	int        nWidth, nHeight;
//	// 位图宽度和高度
//	int        xScrn, yScrn;
//	// 屏幕分辨率
//	// 确保选定区域不为空矩形
//	if (IsRectEmpty(lpRect))
//		return NULL;
//	//为屏幕创建设备描述表
//	hScrDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
//	//为屏幕设备描述表创建兼容的内存设备描述表
//	hMemDC = CreateCompatibleDC(hScrDC);
//	// 获得选定区域坐标
//	nX = lpRect->left;
//	nY = lpRect->top;
//	nX2 = lpRect->right;
//	nY2 = lpRect->bottom;
//	// 获得屏幕分辨率
//	xScrn = GetDeviceCaps(hScrDC, HORZRES);
//	yScrn = GetDeviceCaps(hScrDC, VERTRES);
//	//确保选定区域是可见的
//	if (nX < 0)
//		nX = 0;
//	if (nY < 0)
//		nY = 0;
//	if (nX2 > xScrn)
//		nX2 = xScrn;
//	if (nY2 > yScrn)
//		nY2 = yScrn;
//	nWidth = nX2 - nX;
//	nHeight = nY2 - nY;
//	// 创建一个与屏幕设备描述表兼容的位图
//	hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);
//	// 把新位图选到内存设备描述表中
//	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
//	// 把屏幕设备描述表拷贝到内存设备描述表中
//	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);
//	//得到屏幕位图的句柄
//	hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
//	//清除 
//	DeleteDC(hScrDC);
//	DeleteDC(hMemDC);
//	// 返回位图句柄
//	return hBitmap;
//}


