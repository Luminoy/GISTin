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

void CParamDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamDialog, CDialog)
END_MESSAGE_MAP()


// CParamDialog ��Ϣ�������
