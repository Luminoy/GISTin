// ParamDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "GISTin.h"
#include "ParamDialog.h"
#include "afxdialogex.h"

// CParamDialog 对话框

IMPLEMENT_DYNAMIC(CParamDialog, CDialog)

CParamDialog::CParamDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PARAM_DIALOG, pParent)
{
	
}

CParamDialog::~CParamDialog()
{
	if (!m_strText) return;
	ReleaseExcelHandle(m_strText);
}

//int CParamDialog::OnInitDialog() {
//	
//	m_attrTable.InsertColumn(0,"Y", LVCFMT_LEFT, 40);
//	
//	return 1;
//}
void CParamDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TABLE, m_attrTable);
	DDX_Control(pDX, IDC_MAN_TYPE, m_manType);
	DDX_Control(pDX, IDC_WALK_TYPE, m_walkType);
	DDX_Control(pDX, IDC_META_TYPE, m_metaType);
	DDX_Control(pDX, IDC_FILEBROWSE, m_fileBrowser);
}


BEGIN_MESSAGE_MAP(CParamDialog, CDialog)
	ON_EN_CHANGE(IDC_FILEBROWSE, &CParamDialog::OnEnChangeFilebrowse)
	ON_LBN_SELCHANGE(IDC_META_TYPE, &CParamDialog::OnMetaTypeSelectChanged)
	ON_CBN_SELCHANGE(IDC_MAN_TYPE, &CParamDialog::OnManTypeSelectChanged)
END_MESSAGE_MAP()


// CParamDialog 消息处理程序


BOOL CParamDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_attrTable.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_fileBrowser.EnableFileBrowseButton(_T(""), _T("Excel File(.xlsx)|*.xlsx|All Files|*.*|"));
	SetGroupBoxStatus(0);

	// 以后改成从数据库中读取表头信息
	//CString man_type[] = {_T("成人"),_T("老弱"),_T("儿童") };
	//for (int i = 0; i < sizeof(man_type) / sizeof(CString); i++) {
	//	m_manType.InsertString(i, man_type[i]);
	//}
	//m_manType.SetCurSel(0);

	//CString walk_type[] = { _T("步行"),_T("自行车"),_T("汽车") };
	//for (int i = 0; i < sizeof(walk_type) / sizeof(CString); i++) {
	//	m_walkType.InsertString(i, walk_type[i]);
	//}
	//m_walkType.SetCurSel(0);

	//CString meta_type[] = { _T("地表类型"), _T("地形坡度"), _T("速度"), _T("时间"), _T("体力") };
	//for (int i = 0; i < sizeof(meta_type) / sizeof(CString); i++) {
	//	m_metaType.InsertString(i, meta_type[i]);
	//}
	//m_metaType.SetCurSel(0);

	//CString headers[] = { _T("NO"), _T("类型"), _T("值")};
	//for (int i = 0; i < sizeof(headers) / sizeof(CString); i++) {
	//	m_attrTable.InsertColumn(i, headers[i], LVCFMT_LEFT, 80);
	//}
	//
	//m_attrTable.InsertItem(0, "NO.1");
	//m_attrTable.SetItemText(0, 1, "daolu");
	//m_attrTable.SetItemText(0, 2, "slow");
	return TRUE;  
	// return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CParamDialog::OnEnChangeFilebrowse()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString strText;
	m_fileBrowser.GetWindowTextA(strText);
	AfxMessageBox(strText);

	if (!strText.GetLength()) 
	{
		SetGroupBoxStatus(FALSE);
	}
	else
	{
		SetGroupBoxStatus(TRUE);

		OnReadExcelFile(strText);
		CWorksheet sheet;
		CRange cells;
		long rows, columns;

		sheet = GetWorksheet("行人类型");
		cells = GetTable(sheet, rows, columns);

		long j = 1; // 第二列
		for (long i = 0; i < rows; i++) {
			CRange cell;
			cell.AttachDispatch(cells.get_Item(COleVariant(i + 1), COleVariant(j + 1)).pdispVal, TRUE); // 从1开始的索引
			VARIANT item = cell.get_Text();
			m_manType.InsertString(i, CString(item.bstrVal));
		}
		sheet.DetachDispatch();
		cells.DetachDispatch();
		m_manType.SetCurSel(0);
		

		sheet = GetWorksheet("通行类型");
		cells = GetTable(sheet, rows, columns);

		j = 1; // 第二列
		for (long i = 0; i < rows; i++) {
			CRange cell;
			cell.AttachDispatch(cells.get_Item(COleVariant(i + 1), COleVariant(j + 1)).pdispVal, TRUE); // 从1开始的索引
			VARIANT item = cell.get_Text();
			m_walkType.InsertString(i, CString(item.bstrVal));
		}
		sheet.DetachDispatch();
		cells.DetachDispatch();
		m_walkType.SetCurSel(0);


		sheet = GetWorksheet("因子类型");
		cells = GetTable(sheet, rows, columns);

		j = 1; // 第二列
		for (long i = 0; i < rows; i++) {
			CRange cell;
			cell.AttachDispatch(cells.get_Item(COleVariant(i + 1), COleVariant(j + 1)).pdispVal, TRUE); // 从1开始的索引
			VARIANT item = cell.get_Text();
			m_metaType.InsertString(i, CString(item.bstrVal));
		}
		sheet.DetachDispatch();
		cells.DetachDispatch();
		m_metaType.SetCurSel(0);

		m_attrTable.InsertColumn(0, "Header", LVCFMT_LEFT, 85, 0);
		m_attrTable.InsertColumn(1, "Value1", LVCFMT_LEFT, 85, 1);
		m_attrTable.InsertColumn(2, "Value2", LVCFMT_LEFT, 85, 2);
		m_attrTable.InsertColumn(3, "Value3", LVCFMT_LEFT, 85, 3);

		m_strText = strText;
		RefreshAttrTable();
	}
}

CRange CParamDialog::GetTable(CWorksheet &sheet, long &nUsedRow, long &nUsedColumn) {
	CRange range, cells;
	range.AttachDispatch(sheet.get_UsedRange(), TRUE);

	range.AttachDispatch(range.get_Rows(), TRUE);
	nUsedRow = range.get_Count();

	range.AttachDispatch(range.get_Columns(), TRUE);
	nUsedColumn = range.get_Count();

	cells.AttachDispatch(sheet.get_Cells(), TRUE);
	return cells;
}

void CParamDialog::SetGroupBoxStatus(BOOL bFlag) 
{
	GetDlgItem(IDC_MAN_TYPE)->EnableWindow(bFlag);
	GetDlgItem(IDC_WALK_TYPE)->EnableWindow(bFlag);
	GetDlgItem(IDC_META_TYPE)->EnableWindow(bFlag);
	GetDlgItem(IDC_TABLE)->EnableWindow(bFlag);
}

CWorksheets CParamDialog::OnReadExcelFile(CString strPathName)
{
	COleVariant covTrue((short)TRUE);
	COleVariant covFalse((short)FALSE);

	if (!m_oExcelApp.CreateDispatch(_T("Excel.Application"))) {
		::MessageBox(NULL, _T("创建Excel服务失败！"), _T("错误提示！"), MB_OK | MB_ICONERROR);
		exit(1);
	}
	m_oExcelApp.put_Visible(TRUE); //m_oExcelApp.SetVisible(FALSE);
	m_oWorkBooks.AttachDispatch(m_oExcelApp.get_Workbooks(), TRUE);  // m_oExcelApp.GetWorkbooks()

	m_lpDisp = m_oWorkBooks.Open(strPathName, _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing),
		_variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing));

	m_oWorkBook.AttachDispatch(m_lpDisp, TRUE);
	m_oWorkSheets.AttachDispatch(m_oWorkBook.get_Sheets(), TRUE);

	long nSheetCount = 0;
	nSheetCount = m_oWorkSheets.get_Count();

	// 获取工作表的名称集合
	for (int i = 1; i <= nSheetCount; i++) {
		CWorksheet wsheet;
		wsheet.AttachDispatch(m_oWorkSheets.get_Item(COleVariant((long)i)), TRUE);
		//AfxMessageBox(wsheet.get_Name());
		mapWorksheet.insert(std::make_pair(wsheet.get_Name(), i));
	}

	return m_oWorkSheets;
}

CWorksheet CParamDialog::GetWorksheet(CString strWorksheetName) {
	long idx = 0;
	CWorksheet wsheet;
	if ((idx = mapWorksheet[strWorksheetName])) {
		wsheet.AttachDispatch(m_oWorkSheets.get_Item(COleVariant(idx)), TRUE);
	}
	return wsheet;
}

void CParamDialog::ReleaseExcelHandle(CString strPathName) {
	// 释放
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	m_oWorkBook.Close(covOptional, COleVariant(strPathName), covOptional);
	m_oWorkBooks.Close();

	m_oCurrRange.ReleaseDispatch();
	m_oWorkSheet.ReleaseDispatch();
	m_oWorkSheets.ReleaseDispatch();
	m_oWorkBook.ReleaseDispatch();
	m_oWorkBooks.ReleaseDispatch();
	m_oExcelApp.ReleaseDispatch();
	m_oExcelApp.Quit();
}
//CString CParamDialog::GetExcelDriver()
//{
//	char szBuf[2001];
//	WORD cbBufMax = 2000;
//	WORD cbBufOut;
//	char *pszBuf = szBuf;
//	CString sDriver;
//
//	// 获取已安装驱动的名称(函数在odbcinst.h里)
//	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
//		return "";
//
//	// 检索已安装的驱动是否有Excel...
//	do
//	{
//		if (strstr(pszBuf, "Excel") != 0)
//		{
//			//发现 !
//			sDriver = CString(pszBuf);
//			break;
//		}
//		pszBuf = strchr(pszBuf, '\0') + 1;
//	} while (pszBuf[1] != '\0');
//
//	return sDriver;
//
//}
//
//BOOL CParamDialog::ExcelRead(CString strPathName) {
//	//导出
//	CApplication app;
//	CWorkbook book;
//	CWorkbooks books;
//	CWorksheet sheet;
//	CWorksheets sheets;
//	CRange range;
//	CExcelFont font;
//	CRange cols;
//
//	COleVariant covTrue((short)TRUE);
//	COleVariant covFalse((short)FALSE);
//	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
//	if (!app.CreateDispatch(_T("Excel.Application")))
//	{
//		this->MessageBox(_T("无法创建Excel应用！"));
//		return FALSE;
//	}
//	books = app.get_Workbooks();
//	//打开Excel，其中pathname为Excel表的路径名  
//
//	book = books.Add(covOptional);
//	sheets = book.get_Worksheets();
//	int count = sheets.get_Count();
//	sheet = sheets.get_Item(COleVariant((short)1));  //获得坐标为（A，1）和（B，1）的两个单元格 
//	range = sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("B1")));  //设置单元格类容为Hello Exce
//	range.put_Value2(COleVariant(_T("Hello Excel")));  //选择整列，并设置宽度为自适应 
//	sheet.get_UsedRange();
//	cols = range.get_EntireColumn();
//	cols.AutoFit();
//	//设置字体为粗体 
//	font = range.get_Font();
//	font.put_Bold(COleVariant((short)TRUE));
//	//获得坐标为（C，2）单元格 
//	range = sheet.get_Range(COleVariant(_T("C2")), COleVariant(_T("C2")));
//	//设置公式“=RAND()*100000”
//	range.put_Formula(COleVariant(_T("=RAND()*100000")));
//	//设置数字格式为货币型  
//	range.put_NumberFormat(COleVariant(_T("$0.00")));
//	//选择整列，并设置宽度为自适应  
//	cols = range.get_EntireColumn();
//	cols.AutoFit();
//	//显示Excel表
//	app.put_Visible(TRUE);
//	app.put_UserControl(TRUE);
//	return TRUE;
//
//}
//
//BOOL CParamDialog::ExcelRead2(CString strPathName) {
//	return TRUE;
//}


void CParamDialog::RefreshAttrTable()
{
	// 获取对应的表名
	CString szWorksheet;
	szWorksheet.AppendFormat("%d%d%d", m_manType.GetCurSel(), m_walkType.GetCurSel(), m_metaType.GetCurSel());
	//AfxMessageBox(szWorksheet);

	// 通过表名获取对应的表单
	long rows, columns;
	CWorksheet worksheet = GetWorksheet(szWorksheet);

	m_attrTable.DeleteAllItems();
	if (!worksheet)	return;

	CRange cells = GetTable(worksheet, rows, columns);
	std::vector<std::vector<CString> > collection;
	for (long i = 0; i < rows; i++) {
		m_attrTable.InsertItem(i, " ");
		std::vector<CString> vec_item;
		for (long j = 0; j < columns; j++) {
			CRange cell;
			cell.AttachDispatch(cells.get_Item(COleVariant(i + 1), COleVariant(j + 1)).pdispVal, TRUE); // 从1开始的索引
			VARIANT item = cell.get_Text();
			m_attrTable.SetItemText(i, j, CString(item.bstrVal));
			vec_item.push_back(CString(item.bstrVal));
		}
		collection.push_back(vec_item);
	}
}

void CParamDialog::OnMetaTypeSelectChanged()
{
	RefreshAttrTable();
}

void CParamDialog::OnManTypeSelectChanged()
{
	RefreshAttrTable();
}

void CParamDialog::OnWalkTypeSelectChanged()
{
	RefreshAttrTable();
}