// SplitDisDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GISTin.h"
#include "SplitDisDialog.h"
#include "afxdialogex.h"

// CSplitDisDialog �Ի���

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


// CSplitDisDialog ��Ϣ�������


void CSplitDisDialog::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CSplitDisDialog::OnBnClickedCancel()
{
	CString cstr;
	cstr.Format(_T("ȡ�����������Ĭ��ֵ(%.2lf-%.2lf)���Ƿ������"), min_dis_value, max_dis_value);
	if (AfxMessageBox(cstr, MB_OKCANCEL) == IDOK) {
		CDialogEx::OnCancel();
	}
}
