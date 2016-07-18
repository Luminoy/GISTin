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
}


BEGIN_MESSAGE_MAP(CParamDialog, CDialog)
END_MESSAGE_MAP()


// CParamDialog 消息处理程序


BOOL CParamDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str[4] = { _T("Header1"), _T("Header2"), _T("Header3"), _T("Header4") };
	for (int i = 0; i < 4; i++) {
		m_attrTable.InsertColumn(i, str[i], LVCFMT_LEFT, 40);
	}
	
	return TRUE;  
	// return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
