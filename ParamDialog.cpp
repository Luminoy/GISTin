// ParamDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GISTin.h"
#include "ParamDialog.h"
#include "afxdialogex.h"

// CParamDialog �Ի���

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


// CParamDialog ��Ϣ�������


BOOL CParamDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_attrTable.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_fileBrowser.EnableFileBrowseButton(_T(""), _T("Excel File(.xlsx)|*.xlsx|All Files|*.*|"));
	SetGroupBoxStatus(0);

	// �Ժ�ĳɴ����ݿ��ж�ȡ��ͷ��Ϣ
	//CString man_type[] = {_T("����"),_T("����"),_T("��ͯ") };
	//for (int i = 0; i < sizeof(man_type) / sizeof(CString); i++) {
	//	m_manType.InsertString(i, man_type[i]);
	//}
	//m_manType.SetCurSel(0);

	//CString walk_type[] = { _T("����"),_T("���г�"),_T("����") };
	//for (int i = 0; i < sizeof(walk_type) / sizeof(CString); i++) {
	//	m_walkType.InsertString(i, walk_type[i]);
	//}
	//m_walkType.SetCurSel(0);

	//CString meta_type[] = { _T("�ر�����"), _T("�����¶�"), _T("�ٶ�"), _T("ʱ��"), _T("����") };
	//for (int i = 0; i < sizeof(meta_type) / sizeof(CString); i++) {
	//	m_metaType.InsertString(i, meta_type[i]);
	//}
	//m_metaType.SetCurSel(0);

	//CString headers[] = { _T("NO"), _T("����"), _T("ֵ")};
	//for (int i = 0; i < sizeof(headers) / sizeof(CString); i++) {
	//	m_attrTable.InsertColumn(i, headers[i], LVCFMT_LEFT, 80);
	//}
	//
	//m_attrTable.InsertItem(0, "NO.1");
	//m_attrTable.SetItemText(0, 1, "daolu");
	//m_attrTable.SetItemText(0, 2, "slow");
	return TRUE;  
	// return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CParamDialog::OnEnChangeFilebrowse()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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

		sheet = GetWorksheet("��������");
		cells = GetTable(sheet, rows, columns);

		long j = 1; // �ڶ���
		for (long i = 0; i < rows; i++) {
			CRange cell;
			cell.AttachDispatch(cells.get_Item(COleVariant(i + 1), COleVariant(j + 1)).pdispVal, TRUE); // ��1��ʼ������
			VARIANT item = cell.get_Text();
			m_manType.InsertString(i, CString(item.bstrVal));
		}
		sheet.DetachDispatch();
		cells.DetachDispatch();
		m_manType.SetCurSel(0);
		

		sheet = GetWorksheet("ͨ������");
		cells = GetTable(sheet, rows, columns);

		j = 1; // �ڶ���
		for (long i = 0; i < rows; i++) {
			CRange cell;
			cell.AttachDispatch(cells.get_Item(COleVariant(i + 1), COleVariant(j + 1)).pdispVal, TRUE); // ��1��ʼ������
			VARIANT item = cell.get_Text();
			m_walkType.InsertString(i, CString(item.bstrVal));
		}
		sheet.DetachDispatch();
		cells.DetachDispatch();
		m_walkType.SetCurSel(0);


		sheet = GetWorksheet("��������");
		cells = GetTable(sheet, rows, columns);

		j = 1; // �ڶ���
		for (long i = 0; i < rows; i++) {
			CRange cell;
			cell.AttachDispatch(cells.get_Item(COleVariant(i + 1), COleVariant(j + 1)).pdispVal, TRUE); // ��1��ʼ������
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
		::MessageBox(NULL, _T("����Excel����ʧ�ܣ�"), _T("������ʾ��"), MB_OK | MB_ICONERROR);
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

	// ��ȡ����������Ƽ���
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
	// �ͷ�
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
//	// ��ȡ�Ѱ�װ����������(������odbcinst.h��)
//	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
//		return "";
//
//	// �����Ѱ�װ�������Ƿ���Excel...
//	do
//	{
//		if (strstr(pszBuf, "Excel") != 0)
//		{
//			//���� !
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
//	//����
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
//		this->MessageBox(_T("�޷�����ExcelӦ�ã�"));
//		return FALSE;
//	}
//	books = app.get_Workbooks();
//	//��Excel������pathnameΪExcel���·����  
//
//	book = books.Add(covOptional);
//	sheets = book.get_Worksheets();
//	int count = sheets.get_Count();
//	sheet = sheets.get_Item(COleVariant((short)1));  //�������Ϊ��A��1���ͣ�B��1����������Ԫ�� 
//	range = sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("B1")));  //���õ�Ԫ������ΪHello Exce
//	range.put_Value2(COleVariant(_T("Hello Excel")));  //ѡ�����У������ÿ��Ϊ����Ӧ 
//	sheet.get_UsedRange();
//	cols = range.get_EntireColumn();
//	cols.AutoFit();
//	//��������Ϊ���� 
//	font = range.get_Font();
//	font.put_Bold(COleVariant((short)TRUE));
//	//�������Ϊ��C��2����Ԫ�� 
//	range = sheet.get_Range(COleVariant(_T("C2")), COleVariant(_T("C2")));
//	//���ù�ʽ��=RAND()*100000��
//	range.put_Formula(COleVariant(_T("=RAND()*100000")));
//	//�������ָ�ʽΪ������  
//	range.put_NumberFormat(COleVariant(_T("$0.00")));
//	//ѡ�����У������ÿ��Ϊ����Ӧ  
//	cols = range.get_EntireColumn();
//	cols.AutoFit();
//	//��ʾExcel��
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
	// ��ȡ��Ӧ�ı���
	CString szWorksheet;
	szWorksheet.AppendFormat("%d%d%d", m_manType.GetCurSel(), m_walkType.GetCurSel(), m_metaType.GetCurSel());
	//AfxMessageBox(szWorksheet);

	// ͨ��������ȡ��Ӧ�ı�
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
			cell.AttachDispatch(cells.get_Item(COleVariant(i + 1), COleVariant(j + 1)).pdispVal, TRUE); // ��1��ʼ������
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