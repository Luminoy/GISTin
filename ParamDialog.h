#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "afxeditbrowsectrl.h"
#include "afxdb.h"
#include "odbcinst.h"
#include <map>
#include <vector>
#include <utility>
using namespace std;

// Excel��д����ͷ�ļ�
//#include "CApplication.h"
//#include "CExcelFont.h"
//#include "CRange.h"
//#include "CWorkbook.h"
//#include "CWorkbooks.h"
//#include "CWorksheet.h"
//#include "CWorksheets.h"
//#include "comutil.h"

// ���ݿ�����֧��
#include "AdoDatabase.h"

#pragma comment(lib, "comsupp.lib")
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
	CComboBox          m_targetType;
	CListBox           m_surfaceType;
	CMFCEditBrowseCtrl m_fileBrowser;
	
	int                ManTypeID;
	int                WalkTypeID;
	int                TargetTypeID;
	int                SurfaceTypeID;
	std::vector<std::vector<CString> > collection;
	std::vector< std::pair<int, std::vector<std::vector<CString> > > > full_table;
	// behaviors
	void OnEnChangeFilebrowse();
	void SetGroupBoxStatus(BOOL bFlag = 1);
public:
	//����ӿ������ 
	// 
	CAdoDatabase m_database;
	CString       m_strText;

	std::map<CString, long> mapWorksheet;
	void RefreshAttrTable();
	afx_msg void OnFactorTypeSelectChanged();
	afx_msg void OnManTypeSelectChanged();
	afx_msg void OnWalkTypeSelectChanged();
	afx_msg void OnTargetTypeSelectChanged();
//	virtual INT_PTR DoModal();
	
	virtual void OnOK();
	BOOL GetMicTableInfo(const CString & DatabasePath, vector<CString>& tableNames);
	vector<vector<CString>> GetTableByName(CString tName);
};
