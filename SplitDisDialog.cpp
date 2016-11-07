// SplitDisDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "GISTin.h"
#include "SplitDisDialog.h"
#include "afxdialogex.h"

// CSplitDisDialog 对话框

IMPLEMENT_DYNAMIC(CSplitDisDialog, CDialogEx)

CSplitDisDialog::CSplitDisDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_INIT, pParent)
{
}

CSplitDisDialog::CSplitDisDialog(double minDisValue, double maxDisValue, CWnd * pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_INIT, pParent), min_dis_value(minDisValue), max_dis_value(maxDisValue)
{
}

CSplitDisDialog::~CSplitDisDialog()
{
}

void CSplitDisDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSplitDisDialog, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSplitDisDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSplitDisDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSplitDisDialog 消息处理程序


void CSplitDisDialog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString cstr;

	GetDlgItemText(IDC_MIN_DIS, cstr);
	min_dis_value = atof(cstr.GetBuffer());

	GetDlgItemText(IDC_MAX_DIS, cstr);
	max_dis_value = atof(cstr.GetBuffer());
	CDialogEx::OnOK();
}


BOOL CSplitDisDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString cstr;
	cstr.Format("%.2lf", min_dis_value);
	SetDlgItemText(IDC_MIN_DIS, cstr);

	cstr.Format("%.2lf", max_dis_value);
	SetDlgItemText(IDC_MAX_DIS, cstr);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSplitDisDialog::OnBnClickedCancel()
{
	CString cstr;
	cstr.Format(_T("取消设置则采用默认值(%.2lf-%.2lf)，是否继续？"), min_dis_value, max_dis_value);
	if (AfxMessageBox(cstr, MB_OKCANCEL) == IDOK) {
		CDialogEx::OnCancel();
	}
}
