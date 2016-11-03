// InputDataDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "GISTin.h"
#include "InputDataDialog.h"
#include "afxdialogex.h"


// CInputDataDialog 对话框

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


// CInputDataDialog 消息处理程序


BOOL CInputDataDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetDlgItemText(IDC_STATIC1, m_szStaticText);
	SetDlgItemText(IDC_EDIT1, m_szDefaultValue);
	SetWindowText(m_szCaption);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
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
	if (AfxMessageBox("取消则采用默认值设置，是否继续？", MB_OKCANCEL) == IDOK)
	{
		CDialogEx::OnCancel();
	}

}
