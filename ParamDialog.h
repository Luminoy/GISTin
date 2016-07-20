#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxeditbrowsectrl.h"
#include "afxdb.h"
#include "odbcinst.h"

// Excel读写依赖头文件
#include "CApplication.h"
#include "CExcelFont.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
#include "comutil.h"
// CParamDialog 对话框

class CParamDialog : public CDialog
{
	DECLARE_DYNAMIC(CParamDialog)
public:
	CParamDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CParamDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PARAM_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_attrTable;
	virtual BOOL OnInitDialog();
	CComboBox m_manType;
	CComboBox m_walkType;
	CListBox m_metaType;
	CMFCEditBrowseCtrl m_fileBrowser;
	afx_msg void OnEnChangeFilebrowse();
	void CParamDialog::SetGroupBoxStatus(BOOL bFlag = 1);
	void OnReadExcelFile(CString pathname);
	CString GetExcelDriver();
	BOOL ExcelRead(CString strPathName);
	BOOL ExcelRead2(CString strPathName);

public:
	//定义接口类变量 
	CApplication app;
	CWorkbook book;
	CWorkbooks books;
	CWorksheet sheet;
	CWorksheets sheets;
	CRange range;
	CExcelFont font;
	CRange cols;
	LPDISPATCH lpDisp;

	// 
	CApplication m_oExcelApp;
	CWorksheets m_oWorkSheets;
	CWorksheet m_oWorkSheet;
	CWorkbooks m_oWorkBooks;
	CWorkbook m_oWorkBook;
	CRange m_oCurrRange;

};
