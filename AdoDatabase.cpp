#include "StdAfx.h"
#include "AdoDatabase.h"


CAdoDatabase::CAdoDatabase()
{
}


CAdoDatabase::~CAdoDatabase()
{
}

BOOL CAdoDatabase::OpenDatabase(CString strConnection)
{
	HRESULT hr = ::CoInitialize(NULL); // ��ʼ�� COM
	if (!SUCCEEDED(hr)) // ��ʼ��ʧ��
	{
		return FALSE;
	}
	_bstr_t strConnect(strConnection);
	try
	{
		// ���� Connection ����
		hr = m_pConnection.CreateInstance("ADODB.Connection");
		if (SUCCEEDED(hr))
		{
			// �������ݿ�
			if (SUCCEEDED(m_pConnection->Open(strConnect, "", "", adModeUnknown)))
			{
				return TRUE;
			}
		}
	}
	catch (_com_error e)
	{
		TRACE(_T("�������ݿⷢ������%s\n"), e.ErrorMessage());
	}
	return FALSE;
}

BOOL CAdoDatabase::CloseDatabase()
{
	if (m_pConnection == NULL)
	{
		// ��������Ѿ�Ϊ��
		return TRUE;
	}
	try
	{
		m_pConnection->Close();
		m_pConnection = NULL;
		return TRUE;
	}
	catch (_com_error e)
	{
		TRACE(_T("�ر����ݿⷢ������ %s\n"), e.ErrorMessage());
	}
	return FALSE;
}

_RecordsetPtr CAdoDatabase::Select(CString sql)
{
	_bstr_t CommandText(sql); // ����Ҫִ�е� sql ����ַ���
	m_pRecordset.CreateInstance("ADODB.Recordset"); // ����_RecordsetPtr ʵ��
	m_pRecordset->Open(CommandText, // �������ݿ��ѯ
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic,
		adLockBatchOptimistic,
		adCmdText);
	return m_pRecordset;
}

_RecordsetPtr CAdoDatabase::Select2(CString tableName)
{
	_CommandPtr m_pCommand;
	m_pCommand.CreateInstance("ADODB.Command");
	_variant_t vNULL;
	vNULL.vt = VT_ERROR;
	vNULL.scode = DISP_E_PARAMNOTFOUND; //����Ϊ�޲���
	m_pCommand->ActiveConnection = m_pConnection; //�ǳ��ؼ���һ�䣬�����������Ӹ�ֵ����
	CString sql = _T("SELECT * FROM ");
	sql += tableName;
	m_pCommand->CommandText = sql.GetBuffer(); //�����ִ�
	m_pRecordset = m_pCommand->Execute(&vNULL, &vNULL, adCmdText); 
	return m_pRecordset;
}

BOOL CAdoDatabase::Execute(CString sql)
{
	_bstr_t CommandText(sql);
	_variant_t RecordsAffected;
	m_pConnection->Execute(CommandText, &RecordsAffected, adCmdText);
	return TRUE;
}
