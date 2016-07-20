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
END_MESSAGE_MAP()


// CParamDialog 消息处理程序


BOOL CParamDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_attrTable.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_fileBrowser.EnableFileBrowseButton(_T(""), _T("Excel File(.xlsx)|*.xlsx|All Files|*.*|"));
	SetGroupBoxStatus(0);

	// 以后改成从数据库中读取表头信息
	CString man_type[] = {_T("成人"),_T("老弱"),_T("儿童") };
	for (int i = 0; i < sizeof(man_type) / sizeof(CString); i++) {
		m_manType.InsertString(i, man_type[i]);
	}
	m_manType.SetCurSel(0);

	CString walk_type[] = { _T("步行"),_T("自行车"),_T("汽车") };
	for (int i = 0; i < sizeof(walk_type) / sizeof(CString); i++) {
		m_walkType.InsertString(i, walk_type[i]);
	}
	m_walkType.SetCurSel(0);

	CString meta_type[] = { _T("地表类型"), _T("地形坡度"), _T("速度"), _T("时间"), _T("体力") };
	for (int i = 0; i < sizeof(meta_type) / sizeof(CString); i++) {
		m_metaType.InsertString(i, meta_type[i]);
	}
	m_metaType.SetCurSel(0);

	CString headers[] = { _T("NO"), _T("类型"), _T("值")};
	for (int i = 0; i < sizeof(headers) / sizeof(CString); i++) {
		m_attrTable.InsertColumn(i, headers[i], LVCFMT_LEFT, 80);
	}
	
	m_attrTable.InsertItem(0, "NO.1");
	m_attrTable.SetItemText(0, 1, "daolu");
	m_attrTable.SetItemText(0, 2, "slow");
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
	}
}

void CParamDialog::SetGroupBoxStatus(BOOL bFlag) 
{
	GetDlgItem(IDC_MAN_TYPE)->EnableWindow(bFlag);
	GetDlgItem(IDC_WALK_TYPE)->EnableWindow(bFlag);
	GetDlgItem(IDC_META_TYPE)->EnableWindow(bFlag);
	GetDlgItem(IDC_TABLE)->EnableWindow(bFlag);
}

void CParamDialog::OnReadExcelFile(CString pathname) 
{
	// TODO: Add your control notification handler code here
	CDatabase database;
	CString sSql;
	CString sItem1, sItem2, sItem3;
	CString sDriver;
	CString sDsn;
	CString sFile, sPath;


	//获取主程序所在路径,存在sPath中
	GetModuleFileName(NULL, sPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	sPath.ReleaseBuffer();
	int nPos;
	nPos = sPath.ReverseFind('\\');
	sPath = sPath.Left(nPos);

	sFile = sPath + "\\Demo.xls";     // 将被读取的Excel文件名

									  // 检索是否安装有Excel驱动 "Microsoft Excel Driver (*.xls)" 
	sDriver = GetExcelDriver();
	if (sDriver.IsEmpty())
	{
		// 没有发现Excel驱动
		AfxMessageBox("没有安装Excel驱动!");
		return;
	}

	// 创建进行存取的字符串
	sDsn.Format("ODBC;DRIVER={%s};DSN=' ';DBQ=%s", sDriver, sFile);

	TRY
	{
		// 打开数据库(既Excel文件)
		database.Open(NULL, false, false, sDsn);

	CRecordset recset(&database);

	// 设置读取的查询语句.
	sSql = "SELECT Num,Name, Age " //设置索引顺序     
		"FROM Exceldemo ";
	"ORDER BY Name ";

	// 执行查询语句
	recset.Open(CRecordset::forwardOnly, sSql, CRecordset::readOnly);

	// 获取查询结果
	while (!recset.IsEOF())
	{
		//读取Excel内部数值
		recset.GetFieldValue("Num", sItem1);
		recset.GetFieldValue("Name", sItem2);
		recset.GetFieldValue("Age", sItem3);

		//显示记取的内容
		m_metaType.AddString(sItem1 + " --> " + sItem2 + " --> " + sItem3);

		// 移到下一行
		recset.MoveNext();
	}

	// 关闭数据库
	database.Close();

	}
		CATCH(CDBException, e)
	{
		// 数据库操作产生异常时...
		AfxMessageBox("数据库错误: " + e->m_strError);
	}
	END_CATCH;
}

CString CParamDialog::GetExcelDriver()
{
	char szBuf[2001];
	WORD cbBufMax = 2000;
	WORD cbBufOut;
	char *pszBuf = szBuf;
	CString sDriver;

	// 获取已安装驱动的名称(函数在odbcinst.h里)
	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
		return "";

	// 检索已安装的驱动是否有Excel...
	do
	{
		if (strstr(pszBuf, "Excel") != 0)
		{
			//发现 !
			sDriver = CString(pszBuf);
			break;
		}
		pszBuf = strchr(pszBuf, '\0') + 1;
	} while (pszBuf[1] != '\0');

	return sDriver;

}

BOOL CParamDialog::ExcelRead(CString strPathName) {
	//导出
	COleVariant covTrue((short)TRUE);
	COleVariant covFalse((short)FALSE);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (!app.CreateDispatch(_T("Excel.Application")))
	{
		this->MessageBox(_T("无法创建Excel应用！"));
		return FALSE;
	}
	books = app.get_Workbooks();
	//打开Excel，其中pathname为Excel表的路径名  

	book = books.Add(covOptional);
	sheets = book.get_Worksheets();
	int count = sheets.get_Count();
	sheet = sheets.get_Item(COleVariant((short)1));  //获得坐标为（A，1）和（B，1）的两个单元格 
	range = sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("B1")));  //设置单元格类容为Hello Exce
	range.put_Value2(COleVariant(_T("Hello Excel")));  //选择整列，并设置宽度为自适应 
	sheet.get_UsedRange();
	cols = range.get_EntireColumn();
	cols.AutoFit();
	//设置字体为粗体 
	font = range.get_Font();
	font.put_Bold(COleVariant((short)TRUE));
	//获得坐标为（C，2）单元格 
	range = sheet.get_Range(COleVariant(_T("C2")), COleVariant(_T("C2")));
	//设置公式“=RAND()*100000”
	range.put_Formula(COleVariant(_T("=RAND()*100000")));
	//设置数字格式为货币型  
	range.put_NumberFormat(COleVariant(_T("$0.00")));
	//选择整列，并设置宽度为自适应  
	cols = range.get_EntireColumn();
	cols.AutoFit();
	//显示Excel表
	app.put_Visible(TRUE);
	app.put_UserControl(TRUE);
	return TRUE;

}

BOOL CParamDialog::ExcelRead2(CString strPathName) {
	if (m_oExcelApp.CreateDispatch(_T("Excel.Application"))) {
		::MessageBox(NULL, _T("创建Excel服务失败！"), _T("错误提示！"), MB_OK | MB_ICONERROR);
		exit(1);
	}
	m_oExcelApp.put_Visible(TRUE); //m_oExcelApp.SetVisible(FALSE);
	m_oWorkBooks.AttachDispatch(m_oExcelApp.get_Workbooks(), TRUE);  // m_oExcelApp.GetWorkbooks()
	CRange oCurCell;
	LPDISPATCH lpDisp = NULL;
	COleVariant covTrue((short)TRUE);
	COleVariant covFalse((short)FALSE);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	lpDisp = m_oWorkBooks.Open(strPathName, _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing),
		_variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing), _variant_t(vtMissing));

}