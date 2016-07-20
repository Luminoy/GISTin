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


// CParamDialog ��Ϣ�������


BOOL CParamDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_attrTable.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_fileBrowser.EnableFileBrowseButton(_T(""), _T("Excel File(.xlsx)|*.xlsx|All Files|*.*|"));
	SetGroupBoxStatus(0);

	// �Ժ�ĳɴ����ݿ��ж�ȡ��ͷ��Ϣ
	CString man_type[] = {_T("����"),_T("����"),_T("��ͯ") };
	for (int i = 0; i < sizeof(man_type) / sizeof(CString); i++) {
		m_manType.InsertString(i, man_type[i]);
	}
	m_manType.SetCurSel(0);

	CString walk_type[] = { _T("����"),_T("���г�"),_T("����") };
	for (int i = 0; i < sizeof(walk_type) / sizeof(CString); i++) {
		m_walkType.InsertString(i, walk_type[i]);
	}
	m_walkType.SetCurSel(0);

	CString meta_type[] = { _T("�ر�����"), _T("�����¶�"), _T("�ٶ�"), _T("ʱ��"), _T("����") };
	for (int i = 0; i < sizeof(meta_type) / sizeof(CString); i++) {
		m_metaType.InsertString(i, meta_type[i]);
	}
	m_metaType.SetCurSel(0);

	CString headers[] = { _T("NO"), _T("����"), _T("ֵ")};
	for (int i = 0; i < sizeof(headers) / sizeof(CString); i++) {
		m_attrTable.InsertColumn(i, headers[i], LVCFMT_LEFT, 80);
	}
	
	m_attrTable.InsertItem(0, "NO.1");
	m_attrTable.SetItemText(0, 1, "daolu");
	m_attrTable.SetItemText(0, 2, "slow");
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


	//��ȡ����������·��,����sPath��
	GetModuleFileName(NULL, sPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	sPath.ReleaseBuffer();
	int nPos;
	nPos = sPath.ReverseFind('\\');
	sPath = sPath.Left(nPos);

	sFile = sPath + "\\Demo.xls";     // ������ȡ��Excel�ļ���

									  // �����Ƿ�װ��Excel���� "Microsoft Excel Driver (*.xls)" 
	sDriver = GetExcelDriver();
	if (sDriver.IsEmpty())
	{
		// û�з���Excel����
		AfxMessageBox("û�а�װExcel����!");
		return;
	}

	// �������д�ȡ���ַ���
	sDsn.Format("ODBC;DRIVER={%s};DSN=' ';DBQ=%s", sDriver, sFile);

	TRY
	{
		// �����ݿ�(��Excel�ļ�)
		database.Open(NULL, false, false, sDsn);

	CRecordset recset(&database);

	// ���ö�ȡ�Ĳ�ѯ���.
	sSql = "SELECT Num,Name, Age " //��������˳��     
		"FROM Exceldemo ";
	"ORDER BY Name ";

	// ִ�в�ѯ���
	recset.Open(CRecordset::forwardOnly, sSql, CRecordset::readOnly);

	// ��ȡ��ѯ���
	while (!recset.IsEOF())
	{
		//��ȡExcel�ڲ���ֵ
		recset.GetFieldValue("Num", sItem1);
		recset.GetFieldValue("Name", sItem2);
		recset.GetFieldValue("Age", sItem3);

		//��ʾ��ȡ������
		m_metaType.AddString(sItem1 + " --> " + sItem2 + " --> " + sItem3);

		// �Ƶ���һ��
		recset.MoveNext();
	}

	// �ر����ݿ�
	database.Close();

	}
		CATCH(CDBException, e)
	{
		// ���ݿ���������쳣ʱ...
		AfxMessageBox("���ݿ����: " + e->m_strError);
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

	// ��ȡ�Ѱ�װ����������(������odbcinst.h��)
	if (!SQLGetInstalledDrivers(szBuf, cbBufMax, &cbBufOut))
		return "";

	// �����Ѱ�װ�������Ƿ���Excel...
	do
	{
		if (strstr(pszBuf, "Excel") != 0)
		{
			//���� !
			sDriver = CString(pszBuf);
			break;
		}
		pszBuf = strchr(pszBuf, '\0') + 1;
	} while (pszBuf[1] != '\0');

	return sDriver;

}

BOOL CParamDialog::ExcelRead(CString strPathName) {
	//����
	COleVariant covTrue((short)TRUE);
	COleVariant covFalse((short)FALSE);
	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	if (!app.CreateDispatch(_T("Excel.Application")))
	{
		this->MessageBox(_T("�޷�����ExcelӦ�ã�"));
		return FALSE;
	}
	books = app.get_Workbooks();
	//��Excel������pathnameΪExcel���·����  

	book = books.Add(covOptional);
	sheets = book.get_Worksheets();
	int count = sheets.get_Count();
	sheet = sheets.get_Item(COleVariant((short)1));  //�������Ϊ��A��1���ͣ�B��1����������Ԫ�� 
	range = sheet.get_Range(COleVariant(_T("A1")), COleVariant(_T("B1")));  //���õ�Ԫ������ΪHello Exce
	range.put_Value2(COleVariant(_T("Hello Excel")));  //ѡ�����У������ÿ��Ϊ����Ӧ 
	sheet.get_UsedRange();
	cols = range.get_EntireColumn();
	cols.AutoFit();
	//��������Ϊ���� 
	font = range.get_Font();
	font.put_Bold(COleVariant((short)TRUE));
	//�������Ϊ��C��2����Ԫ�� 
	range = sheet.get_Range(COleVariant(_T("C2")), COleVariant(_T("C2")));
	//���ù�ʽ��=RAND()*100000��
	range.put_Formula(COleVariant(_T("=RAND()*100000")));
	//�������ָ�ʽΪ������  
	range.put_NumberFormat(COleVariant(_T("$0.00")));
	//ѡ�����У������ÿ��Ϊ����Ӧ  
	cols = range.get_EntireColumn();
	cols.AutoFit();
	//��ʾExcel��
	app.put_Visible(TRUE);
	app.put_UserControl(TRUE);
	return TRUE;

}

BOOL CParamDialog::ExcelRead2(CString strPathName) {
	if (m_oExcelApp.CreateDispatch(_T("Excel.Application"))) {
		::MessageBox(NULL, _T("����Excel����ʧ�ܣ�"), _T("������ʾ��"), MB_OK | MB_ICONERROR);
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