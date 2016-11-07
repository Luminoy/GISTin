#pragma once

#include "StdAfx.h"
#include "GISTinView.h"

// CSplitDisDialog �Ի���

class CSplitDisDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CSplitDisDialog)

public:
	CSplitDisDialog(CWnd* pParent = NULL);   // ��׼���캯��
	CSplitDisDialog(double minDisValue, double maxDisValue, CWnd* pParent = NULL);
	virtual ~CSplitDisDialog();

	double min_dis_value;
	double max_dis_value;
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_INIT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
};
