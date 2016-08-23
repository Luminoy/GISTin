#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxeditbrowsectrl.h"
#include "afxdb.h"
#include "odbcinst.h"
#include <map>
#include <vector>
#include <utility>

// Excel��д����ͷ�ļ�
#include "CApplication.h"
#include "CExcelFont.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"
#include "comutil.h"

// CParamDialog �Ի���

class CParamDialog : public CDialog
{
	DECLARE_DYNAMIC(CParamDialog)
public:
	CParamDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CParamDialog();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PARAM_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	// attributes
	CListCtrl          m_attrTable;
	CComboBox          m_manType;
	CComboBox          m_walkType;
	CListBox           m_metaType;
	CMFCEditBrowseCtrl m_fileBrowser;

	// behaviors
	void OnEnChangeFilebrowse();
	void SetGroupBoxStatus(BOOL bFlag = 1);
	CWorksheets OnReadExcelFile(CString strPathName);
	CWorksheet  GetWorksheet(CString strWorksheetName);
	void ReleaseExcelHandle(CString strPathName);
	CRange GetTable(CWorksheet &sheet, long &nUsedRow, long &nUsedColumn);

	//CString GetExcelDriver();
	//BOOL ExcelRead(CString strPathName);
	//BOOL ExcelRead2(CString strPathName);
public:
	//����ӿ������ 
	// 
	CApplication  m_oExcelApp;
	CWorksheets   m_oWorkSheets;
	CWorksheet    m_oWorkSheet;
	CWorkbooks    m_oWorkBooks;
	CWorkbook     m_oWorkBook;
	CRange        m_oCurrRange;
	LPDISPATCH    m_lpDisp;
	CString       m_strText;
	std::map<CString, long> mapWorksheet;
	void RefreshAttrTable();
	afx_msg void OnMetaTypeSelectChanged();
	afx_msg void OnManTypeSelectChanged();
	afx_msg void OnWalkTypeSelectChanged();
};
