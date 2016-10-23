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
	m_database.CloseDatabase();
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
	DDX_Control(pDX, IDC_TARGET_TYPE, m_targetType);
	DDX_Control(pDX, IDC_FILEBROWSE, m_fileBrowser);
	DDX_Control(pDX, IDC_SURFACE_TYPE, m_surfaceType);
}


BEGIN_MESSAGE_MAP(CParamDialog, CDialog)
	ON_EN_CHANGE(IDC_FILEBROWSE, &CParamDialog::OnEnChangeFilebrowse)
	ON_CBN_SELCHANGE(IDC_SURFACE_TYPE, &CParamDialog::OnFactorTypeSelectChanged)
	ON_CBN_SELCHANGE(IDC_MAN_TYPE, &CParamDialog::OnManTypeSelectChanged)
	ON_CBN_SELCHANGE(IDC_TARGET_TYPE, &CParamDialog::OnTargetTypeSelectChanged)
	ON_CBN_SELCHANGE(IDC_WALK_TYPE, &CParamDialog::OnWalkTypeSelectChanged)
END_MESSAGE_MAP()


// CParamDialog 消息处理程序


BOOL CParamDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_attrTable.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_fileBrowser.EnableFileBrowseButton(_T(""), _T("Access数据库文件|*.mdb|All Files|*.*|"));
	SetGroupBoxStatus(0);
	return TRUE;  
	// return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CParamDialog::OnEnChangeFilebrowse()
{
	CString strText;
	m_fileBrowser.GetWindowText(strText);
	AfxMessageBox(strText);

	m_strText = strText;

	CFileFind finder;
	if (!strText.GetLength() || !finder.FindFile(strText))
	{
		SetGroupBoxStatus(FALSE);
	}
	else
	{
		SetGroupBoxStatus(TRUE);

		CString strConnection = _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=");
		strConnection += strText;
		m_database.OpenDatabase(strConnection);

		_RecordsetPtr pRecordset;
		CString tableName;
		CString fieldName;
		CString SQL;
		long nPos;

		fieldName = _T("类型名");
		tableName = _T("行人类型");
		SQL.Format(_T("select * from %s"), tableName);
		pRecordset = m_database.Select(SQL);

		nPos = 0;
		while (!pRecordset->adoEOF) {
			CString value = pRecordset->GetCollect(fieldName.GetBuffer()).bstrVal;
			m_manType.InsertString(nPos++, value);
			pRecordset->MoveNext();
		}
		m_manType.SetCurSel(0);
		
		fieldName = _T("类型名");
		tableName = _T("通行类型");
		SQL.Format(_T("select * from %s"), tableName);
		pRecordset = m_database.Select(SQL);

		nPos = 0;
		while (!pRecordset->adoEOF) {
			CString value = pRecordset->GetCollect(fieldName.GetBuffer()).bstrVal;
			m_walkType.InsertString(nPos++, value);
			pRecordset->MoveNext();
		}
		m_walkType.SetCurSel(0);

		fieldName = _T("类型名");
		tableName = _T("地表类型");
		SQL.Format(_T("select * from %s"), tableName);
		pRecordset = m_database.Select(SQL);

		nPos = 0;
		while (!pRecordset->adoEOF) {
			CString value = pRecordset->GetCollect(fieldName.GetBuffer()).bstrVal;
			m_surfaceType.InsertString(nPos++, value);
			pRecordset->MoveNext();
		}
		m_surfaceType.SetCurSel(0);

		fieldName = _T("类型名");
		tableName = _T("通行目标");
		SQL.Format(_T("select * from %s"), tableName);
		pRecordset = m_database.Select(SQL);

		nPos = 0;
		while (!pRecordset->adoEOF) {
			CString value = pRecordset->GetCollect(fieldName.GetBuffer()).bstrVal;
			m_targetType.InsertString(nPos++, value);
			pRecordset->MoveNext();
		}
		m_targetType.SetCurSel(0);

		RefreshAttrTable();
		//m_database.CloseDatabase();
	}
}

void CParamDialog::SetGroupBoxStatus(BOOL bFlag) 
{
	GetDlgItem(IDC_MAN_TYPE)->EnableWindow(bFlag);
	GetDlgItem(IDC_WALK_TYPE)->EnableWindow(bFlag);
	GetDlgItem(IDC_SURFACE_TYPE)->EnableWindow(bFlag);
	GetDlgItem(IDC_TARGET_TYPE)->EnableWindow(bFlag);
	GetDlgItem(IDC_TABLE)->EnableWindow(bFlag);
}


void CParamDialog::RefreshAttrTable()
{
	ManTypeID = m_manType.GetCurSel();
	WalkTypeID = m_walkType.GetCurSel();
	SurfaceTypeID = m_surfaceType.GetCurSel();
	TargetTypeID = m_targetType.GetCurSel();

	// 获取对应的表名
	CString szWorksheetName;
	szWorksheetName.AppendFormat("%d%d%d%d", ManTypeID, WalkTypeID, TargetTypeID, SurfaceTypeID);
	AfxMessageBox(szWorksheetName);

	// 通过表名获取对应的表单
	collection = GetTableByName(szWorksheetName);
	if (!collection.size())
		return;

	//清除表头和已有数据
	m_attrTable.DeleteAllItems();

	CHeaderCtrl *pHeaderCtrl = m_attrTable.GetHeaderCtrl();
	if (pHeaderCtrl != NULL) {
		int nColCount = pHeaderCtrl->GetItemCount();
		for (int i = 0; i < nColCount; i++) {
			m_attrTable.DeleteColumn(0);
		}
		nColCount = pHeaderCtrl->GetItemCount();
	}

	//重新填入数据
	vector<CString>& header = collection[0];
	for (int k = 0; k < header.size(); k++) {
		m_attrTable.InsertColumn(k, header[k], LVCFMT_LEFT, 80, k);
	}

	collection.erase(collection.begin());
	int nRows = collection.size();
	for (long i = 0; i < nRows; i++) {
		m_attrTable.InsertItem(i, _T(" "));
		vector<CString>& record = collection[i];
		vector<CString> vec_item;
		int nCols = record.size();
		for (long j = 0; j < nCols; j++) {
			m_attrTable.SetItemText(i, j, record[j]);
		}
	}
	UpdateData();
}

void CParamDialog::OnFactorTypeSelectChanged()
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

void CParamDialog::OnTargetTypeSelectChanged()
{
	RefreshAttrTable();
}

void CParamDialog::OnOK()
{
	for (int k = 0; k < m_surfaceType.GetCount(); k++) {
		// 获取对应的表名
		CString szWorksheetName;
		szWorksheetName.AppendFormat("%d%d%d%d", ManTypeID, WalkTypeID, TargetTypeID, k);
		//AfxMessageBox(szWorksheetName);

		// 通过表名获取对应的表单
		vector<vector<CString> > table = GetTableByName(szWorksheetName);
		table.erase(table.begin()); // 删除表头
		full_table.push_back(make_pair(k, table));
	}
	CDialog::OnOK();
}

BOOL CParamDialog::GetMicTableInfo(const CString& DatabasePath, vector<CString>& tableNames) //需要传入数据库路径
{
	
	CoInitialize(NULL);//此处在线程中需要用到.

	CFileFind finder;
	if (!finder.FindFile(DatabasePath))
		return FALSE;

	_ConnectionPtr pConn;
	if (FAILED(pConn.CreateInstance("ADODB.Connection"))) {
		return FALSE;
	}
	pConn->put_CommandTimeout(long(5));

	try {
		CString SqlCmd;
		BSTR bstr;
		SqlCmd.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0; Data Source=%s"), DatabasePath);
		bstr = (_bstr_t)SqlCmd;
		pConn->Open(bstr, "", "", adModeUnknown);
	}
	catch (_com_error e) {
		AfxMessageBox(e.ErrorMessage());
		return FALSE;
	}

	_RecordsetPtr pRst1;
	if (FAILED(pRst1.CreateInstance("ADODB.Recordset"))) {
		return FALSE;
	}

	try {
		pRst1 = pConn->OpenSchema(adSchemaTables);
		tableNames.clear();
		while (!(pRst1->adoEOF)) {
			_bstr_t table_name = pRst1->Fields->GetItem("TABLE_NAME")->Value;
			_bstr_t table_type = pRst1->Fields->GetItem("TABLE_TYPE")->Value;
			if (strcmp(((LPCSTR)table_type), "TABLE") == 0) {
				tableNames.push_back(CString((LPCSTR)table_name));//将表名存入动态数组中
			}
			pRst1->MoveNext();
		}
		pRst1->Close();
		pConn->Close();
	}
	catch (_com_error e) {
		AfxMessageBox(e.ErrorMessage());
		return FALSE;
	}
	return TRUE;
}

vector<vector<CString> > CParamDialog::GetTableByName(CString tName)
{
	vector<vector<CString> > vecTable;
	CString SQL;
	SQL.Format(_T("select * from %s"), tName);
	//m_database.OpenDatabase(m_strText);
	_RecordsetPtr pRst1 = m_database.Select(SQL);

	if (!(pRst1->adoEOF)) {
		vector<CString> vecFieldNames;
		FieldsPtr pFields = pRst1->GetFields();
		long field_count = pFields->GetCount();
		for (int i = 0; i < field_count; i++) 
		{
			FieldPtr pField = pFields->GetItem(long(i));
			_bstr_t field_name = pField->GetName();
			vecFieldNames.push_back(CString((LPSTR)field_name));
		}
		vecTable.push_back(vecFieldNames);
	}
	while (!(pRst1->adoEOF)) {
		vector<CString> vec;
		FieldsPtr pFields = pRst1->GetFields();
		long field_count = pFields->GetCount();
		for (int i = 0; i < field_count; i++) {
			FieldPtr pField = pFields->GetItem(long(i));
			_variant_t  vValue = pField->GetValue();
			CString tName = CString((LPSTR)_bstr_t(vValue));
			vec.push_back(tName);
		}
		vecTable.push_back(vec);
		pRst1->MoveNext();
	}
	pRst1->Close();
	return vecTable;
}