#pragma once
class CAdoDatabase
{
public:
	CAdoDatabase();
	~CAdoDatabase();
	BOOL OpenDatabase(CString strConnection);
	BOOL CloseDatabase();
	_RecordsetPtr Select(CString sql);
	_RecordsetPtr Select2(CString sql);
	BOOL Execute(CString sql);
public:
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;

};

