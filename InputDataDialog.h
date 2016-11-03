#pragma once
#include "afxwin.h"


// CInputDataDialog 对话框

class CInputDataDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CInputDataDialog)

public:
	CInputDataDialog(CWnd* pParent = NULL);   // 标准构造函数
	CInputDataDialog(CString szCaption, CString szStaticText, CString szDefaultValue, CWnd* pParent = NULL);

	virtual ~CInputDataDialog();
	virtual BOOL OnInitDialog();
public:
	double GetReturnValue();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	
	CEdit m_EditControl;

	CStatic m_StaticText;
	CString m_szCaption, m_szStaticText, m_szDefaultValue;
	CString m_szReturnValue;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
