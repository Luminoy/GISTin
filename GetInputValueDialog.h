#pragma once
#include "StdAfx.h"
#include "GISTinView.h"


// CGetInputValueDialog 对话框

class CGetInputValueDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CGetInputValueDialog)

public:
	CGetInputValueDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGetInputValueDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
