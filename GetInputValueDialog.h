#pragma once
#include "StdAfx.h"
#include "GISTinView.h"


// CGetInputValueDialog �Ի���

class CGetInputValueDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CGetInputValueDialog)

public:
	CGetInputValueDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGetInputValueDialog();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
