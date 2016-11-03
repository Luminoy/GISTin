// InputDataDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GISTin.h"
#include "InputDataDialog.h"
#include "afxdialogex.h"


// CInputDataDialog �Ի���

IMPLEMENT_DYNAMIC(CInputDataDialog, CDialogEx)

CInputDataDialog::CInputDataDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_INPUT_DLG, pParent)
{
	m_szCaption = m_szStaticText = m_szDefaultValue = "";
}

CInputDataDialog::CInputDataDialog(CString szCaption, CString szStaticText, CString szDefaultValue, CWnd * pParent /*=NULL*/)
	: CDialogEx(IDD_INPUT_DLG, pParent), m_szCaption(szCaption), m_szStaticText(szStaticText), m_szDefaultValue(szDefaultValue)
{
	m_szReturnValue = m_szDefaultValue;
}

CInputDataDialog::~CInputDataDialog()
{
}

void CInputDataDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditControl);
	DDX_Control(pDX, IDC_STATIC1, m_StaticText);
}


BEGIN_MESSAGE_MAP(CInputDataDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CInputDataDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CInputDataDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// CInputDataDialog ��Ϣ�������


BOOL CInputDataDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetDlgItemText(IDC_STATIC1, m_szStaticText);
	SetDlgItemText(IDC_EDIT1, m_szDefaultValue);
	SetWindowText(m_szCaption);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

double CInputDataDialog::GetReturnValue()
{
	return atof(m_szReturnValue.GetBuffer());
}

void CInputDataDialog::OnBnClickedOk()
{
	GetDlgItemText(IDC_EDIT1, m_szReturnValue);

	CDialogEx::OnOK();
}


void CInputDataDialog::OnBnClickedCancel()
{
	if (AfxMessageBox("ȡ�������Ĭ��ֵ���ã��Ƿ������", MB_OKCANCEL) == IDOK)
	{
		CDialogEx::OnCancel();
	}

}
