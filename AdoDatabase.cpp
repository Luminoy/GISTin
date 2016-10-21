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
	HRESULT hr = ::CoInitialize(NULL); // 初始化 COM
	if (!SUCCEEDED(hr)) // 初始化失败
	{
		return FALSE;
	}
	_bstr_t strConnect(strConnection);
	try
	{
		// 创建 Connection 对象
		hr = m_pConnection.CreateInstance("ADODB.Connection");
		if (SUCCEEDED(hr))
		{
			// 连接数据库
			if (SUCCEEDED(m_pConnection->Open(strConnect, "", "", adModeUnknown)))
			{
				return TRUE;
			}
		}
	}
	catch (_com_error e)
	{
		TRACE(_T("连接数据库发生错误%s\n"), e.ErrorMessage());
	}
	return FALSE;
}

BOOL CAdoDatabase::CloseDatabase()
{
	if (m_pConnection == NULL)
	{
		// 如果连接已经为空
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
		TRACE(_T("关闭数据库发生错误： %s\n"), e.ErrorMessage());
	}
	return FALSE;
}

_RecordsetPtr CAdoDatabase::Select(CString sql)
{
	_bstr_t CommandText(sql); // 生成要执行的 sql 语句字符串
	m_pRecordset.CreateInstance("ADODB.Recordset"); // 生成_RecordsetPtr 实例
	m_pRecordset->Open(CommandText, // 连接数据库查询
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
	vNULL.scode = DISP_E_PARAMNOTFOUND; //定义为无参数
	m_pCommand->ActiveConnection = m_pConnection; //非常关键的一句，将建立的连接赋值给它
	CString sql = _T("SELECT * FROM ");
	sql += tableName;
	m_pCommand->CommandText = sql.GetBuffer(); //命令字串
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
