// GetInputValueDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "GISTin.h"
#include "GetInputValueDialog.h"
#include "afxdialogex.h"


// CGetInputValueDialog 对话框

IMPLEMENT_DYNAMIC(CGetInputValueDialog, CDialogEx)

CGetInputValueDialog::CGetInputValueDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_INPUT_DLG, pParent)
{

}

CGetInputValueDialog::~CGetInputValueDialog()
{
}

void CGetInputValueDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGetInputValueDialog, CDialogEx)
END_MESSAGE_MAP()


// CGetInputValueDialog 消息处理程序
