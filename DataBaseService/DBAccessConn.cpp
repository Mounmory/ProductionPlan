// ADOConn.cpp : 定义 DLL 应用程序的导出函数。
//
#pragma once
#include "stdafx.h"
#include <atlstr.h>
#include "DataType.h"
#include "DBAccessConn.h"
#include "Logger.h"
//#include <io.h>
//#define BASE_FUNC_EXPORTS

#define DATABASE_LOGIN_CHECK(flag,lgostu)	\
if (flag)								\
{										\
	if (!CheckConnecInfo(lgostu))		\
	{									\
		return FALSE;					\
	}									\
	if (!OnInitADOConn(lgostu))			\
	{									\
		return FALSE;					\
	}									\
}

#define DATABASE_EXIT_CHECK(flag)		\
if (flag)								\
{										\
	ExitConnect();						\
}

DBAccessConn::DBAccessConn():m_bIsLog(FALSE)
{

}

DBAccessConn::~DBAccessConn()
{
	//if (m_pCatalog != nullptr)
	//{
	//	m_pCatalog->Release();
	//}
}

BOOL DBAccessConn::CheckConnecInfo(PDBLOGINFO pAdoInfo)
{
	//校验信息是否正确

	if (pAdoInfo->strDataSource == "")
	{
		MessageBox(NULL,_T("登录信息错误：ACCESS数据库连接信息错误！"),_T("错误"),MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL DBAccessConn::OnInitADOConn(PDBLOGINFO pAdoInfo)
{
	if (m_bIsLog)//如果已经登录了
	{
		//可以输出日志信息
		return TRUE;
	}
	CString strConnec;
	//设置连接字符串

#if defined _WIN64//如果是64位程序 
	strConnec.Format(_T("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;"),pAdoInfo->strDataSource);
#elif defined _WIN32//如果是32位程序 
	strConnec.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;"),pAdoInfo->strDataSource);
#endif

	//初始化OLE/COM库环境
	::CoInitialize(NULL);
	try
	{
		//创建connection对象
		m_pConnection.CreateInstance("ADODB.Connection");	   
		m_pConnection->Open((_bstr_t)strConnec,"","",adModeUnknown);
	}
	//捕捉异常
	catch(_com_error e)
	{
		CString strError = "连接数据库"+pAdoInfo->strDataSource+"失败："+(LPCSTR)e.Description();
		MessageBox(NULL,strError,_T("错误"),MB_ICONEXCLAMATION);
		m_bIsLog = FALSE;
		return FALSE;
	}
	m_bIsLog = TRUE;
	return TRUE;
}


_RecordsetPtr& DBAccessConn::GetRecordSet(_bstr_t bstrSQL, RecordSetOpenType adType)
{
	try
	{
		//if(m_pConnection==NULL)
		if(m_pConnection->GetState() != adStateOpen)
			MessageBox(NULL,_T("记录集连接指针为空！"),_T("错误"),MB_ICONEXCLAMATION);
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		switch(adType)
		{
		case RecordSetOpenType::AD_OPEN_DANAMIC:
			// 动态游标,所有的修改对当前用户是见的
			m_pRecordset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
			break;
		case RecordSetOpenType::AD_OPEN_STATIC:
			//静态游标 ,其它用户的修改对当前用户是不可见的. 
			m_pRecordset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenStatic,adLockOptimistic,adCmdText);
			break;
		case RecordSetOpenType::AD_OPEN_KEYSET:
			//是前两种的折衷   ,   改对当前用户是见的.   但性能是最低的.
			m_pRecordset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);
			break;
		default:
			break;
		}
	}
	catch(_com_error e)
	{
		CString strError =(LPCSTR)e.Description();
		MessageBox(NULL,strError,_T("错误"),MB_ICONEXCLAMATION);
		if (m_pRecordset->State)
			m_pRecordset->Close();
	}
	return m_pRecordset;
}


BOOL DBAccessConn::ExitConnect()
{
	//关闭记录集和连接
	m_bIsLog = FALSE;//不管是否关闭失败，都应该置为false
	try
	{
		////另外一种方法，效果应该一样
		//if(m_pRecordset->GetState() == adStateOpen) //判断当前的记录集状态
		//	m_pRecordset->Close();     //关闭记录集
		if(m_pRecordset->State)
			m_pRecordset->Close();
		if(m_pConnection->State)
			m_pConnection->Close();
		//释放环境
		::CoUninitialize();
	}
	catch(_com_error e)
	{
		CString strError =(LPCSTR)e.Description();
		MessageBox(NULL,strError,_T("错误"),MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL DBAccessConn::GetDataFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn)
{
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PVALUEINFO pValueInfo = &pAdoInfo->stuValueInfo;
	DATABASE_LOGIN_CHECK(bIsLogIn,pLogInfo);

	m_pRecordset = GetRecordSet((_bstr_t)pValueInfo->strInReadSQL, RecordSetOpenType::AD_OPEN_STATIC);
	//edit beginr
	int nRowCount = m_pRecordset->GetRecordCount();//行数
	int nColCount = pValueInfo->szInReadColumnName.size();//列数

	pValueInfo->szOutReadValue.reSizeArray(nRowCount, nColCount);
	int nRowNum = 0 ;
	CString strTemp;
	while(m_pRecordset->adoEOF==0)
	{
		for (int i = 0; i < nColCount; ++i)
		{
			strTemp =  m_pRecordset->GetCollect((_bstr_t)pValueInfo->szInReadColumnName[i]);
			pValueInfo->szOutReadValue[nRowNum][i] = strTemp;
		}
		m_pRecordset->MoveNext();
		nRowNum++;
	}

	//关闭数据库
	DATABASE_EXIT_CHECK(bIsLogIn);
	return TRUE;
}

BOOL DBAccessConn::CreatDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn)
{
	PDBOPERATEINFO pCreatDb = &pAdoInfo->stuDBOperate;//创建数据库信息

	CString strDataSource = pCreatDb->strInDBPath + "\\" + pCreatDb->strInDBname + ".mdb";		
	if (pCreatDb->EnuCreatType == EnumDBCreatType::CREAT_DB || pCreatDb->EnuCreatType == EnumDBCreatType::CREAT_DB_AND_TABLE)//创建数据库
	{
		try
		{
			//初始化OLE/COM库环境
			::CoInitialize(NULL);
			if (m_pCatalog == nullptr)
			{
				m_pCatalog.CreateInstance(__uuidof(Catalog));
			}
#if defined _WIN64//如果是64位程序 
			CString strSQL ="Provider=Microsoft.ACE.OLEDB.12.0;Data Source=" + strDataSource + ";";
#elif defined _WIN32//如果是32位程序 
			CString strSQL ="Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + strDataSource + ";";
#endif
			m_pCatalog->Create((_bstr_t)strSQL);
			//释放环境
			::CoUninitialize();
		}
		catch(_com_error e)
		{
			CString strError = "创建数据库 "+ pCreatDb->strInDBname+" 失败："+(LPCSTR)e.Description();
			MessageBox(NULL,strError,_T("提示！"),MB_OK);
			return FALSE;
		}
		//m_pCatalog->Release();
	}
	if (pCreatDb->EnuCreatType == EnumDBCreatType::CREAT_TABLE || pCreatDb->EnuCreatType == EnumDBCreatType::CREAT_DB_AND_TABLE)//创建表格
	{
		if(bIsLogIn)
		{
			DBLOGINFO LogInfo ;//数据库登录信息
			LogInfo.strDataSource = strDataSource;
			if (!CheckConnecInfo(&LogInfo))
			{
				return FALSE;
			}
			if(!OnInitADOConn(&LogInfo))
			{
				return FALSE;
			}
		}
		m_pRecordset = GetRecordSet((_bstr_t)pCreatDb->strInCreatTableSQL);
		//if(!GetRecordSet((_bstr_t)pCreatDb->strInCreatTableSQL)->State)
		//if(m_pRecordset->GetState() != adStateOpen)//无论正确与否，都会返回0
		//{
		//	ExitConnect();
		//	return FALSE;
		//}
		if (bIsLogIn)
		{
			ExitConnect();
		}
	}
	if(pCreatDb->EnuCreatType < EnumDBCreatType::CREAT_DB || pCreatDb->EnuCreatType > EnumDBCreatType::CREAT_DB_AND_TABLE)
	{
		MessageBox(NULL,_T("创建数据库及表格：未知的创建类型！"),_T("错误"),MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL DBAccessConn::AddDataToDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn)
{
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PVALUEINFO pValueInfo = &pAdoInfo->stuValueInfo;
	DATABASE_LOGIN_CHECK(bIsLogIn, pLogInfo);

	m_pRecordset = GetRecordSet((_bstr_t)pValueInfo->strInWriteSQL);
	//if(m_pRecordset->adoEOF==0)//如果表格内容为空，无法写入数据
	if(m_pRecordset->GetState() == adStateOpen)
	{
		//检查行，列数
		int nWriteColumnNum = pValueInfo->szInWtriteColuName.size();//要写入的列数
		int nWriteRowNum =  pValueInfo->szInWriteValue.getRowCount();//要写入的行数

		if (nWriteColumnNum > pValueInfo->szInWriteValue.getColCount())//校验要写入的列数,基本不会出错
		{
			nWriteColumnNum = pValueInfo->szInWriteValue.getColCount();
		}	
		//写入数据
		try
		{
			for (int i = 0 ; i < nWriteRowNum ; ++i)
			{
				m_pRecordset->AddNew();//默认插到行尾
				for (int j = 0; j < nWriteColumnNum; ++j)
				{
					m_pRecordset->PutCollect((_bstr_t)pValueInfo->szInWtriteColuName[j],(_bstr_t)pValueInfo->szInWriteValue[i][j]);
				}
				//m_pRecordset->Update();
				//m_pRecordset->MoveNext();
			}
			m_pRecordset->Update();
		}
		catch(_com_error e)
		{
			CString strError =(LPCSTR)e.Description();
			MessageBox(NULL,strError,_T("错误"),MB_ICONEXCLAMATION);
			ExitConnect();
			return FALSE;
		}
		//关闭数据库
		DATABASE_EXIT_CHECK(bIsLogIn);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL DBAccessConn::UpdateDataFromToBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn)
{
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PVALUEINFO pValueInfo = &pAdoInfo->stuValueInfo;
	DATABASE_LOGIN_CHECK(bIsLogIn, pLogInfo);

	m_pRecordset = GetRecordSet((_bstr_t)pValueInfo->strUpdateSQL);
	while(m_pRecordset->adoEOF==0)
	{
		try
		{
			for (int i=0 ; i<pValueInfo->nUpdateColumnNum ; ++i)
			{
				m_pRecordset->PutCollect(pValueInfo->szUpdateColuName[i].GetBuffer(),(_bstr_t)pValueInfo->szUpdateValue[i]);
				pValueInfo->szUpdateColuName[i].ReleaseBuffer();
			}
			m_pRecordset->Update();
			m_pRecordset->MoveNext();
		}
		catch(_com_error e)
		{
			CString strError =(LPCSTR)e.Description();
			MessageBox(NULL,strError,_T("错误"),MB_ICONEXCLAMATION);
			ExitConnect();
			return FALSE;
		}
	}
	//关闭数据库
	DATABASE_EXIT_CHECK(bIsLogIn);
	return TRUE;
}
BOOL DBAccessConn::DeleteAllDataFromTable(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn)
{
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PVALUEINFO pValueInfo = &pAdoInfo->stuValueInfo;
	DATABASE_LOGIN_CHECK(bIsLogIn, pLogInfo);

	//m_pRecordset = GetRecordSet((_bstr_t)pValueInfo->strInDeletSQL);
	CString strSQL = "delete from " + pValueInfo->strInDelTableName;
	m_pRecordset = GetRecordSet((_bstr_t)strSQL);
	//关闭数据库
	DATABASE_EXIT_CHECK(bIsLogIn);
	return TRUE;
}

BOOL DBAccessConn::DeleteTableFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn)//删除表格
{
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PDBOPERATEINFO pDbInfo = &pAdoInfo->stuDBOperate;
	DATABASE_LOGIN_CHECK(bIsLogIn, pLogInfo);

	CString strDelTable = "drop table "+pDbInfo->strInDelTableName+";";
	m_pRecordset = GetRecordSet((_bstr_t)strDelTable);
	//if(!GetRecordSet((_bstr_t)pDeleTable->strDelSQL)->State)//无论正确与否，GetRecordSet都返回0
	//{
	//	ExitConnect();
	//	return FALSE;
	//}
	DATABASE_EXIT_CHECK(bIsLogIn);
	return TRUE;
}
BOOL DBAccessConn::GetTableNameFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn)
{
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PDBOPERATEINFO pDbInfo = &pAdoInfo->stuDBOperate;
	DATABASE_LOGIN_CHECK(bIsLogIn, pLogInfo);

	pDbInfo->strOutQueryTableResult.clear();//清空查询结果

	CString strTableType;
	m_pRecordset = m_pConnection->OpenSchema(adSchemaTables);
	while (m_pRecordset->adoEOF == 0)
	{
		strTableType = m_pRecordset->Fields->GetItem("TABLE_TYPE")->Value;
		if (strTableType == "TABLE")
		{
			pDbInfo->strOutQueryTableResult.push_back(m_pRecordset->Fields->GetItem("TABLE_NAME")->Value);
		}
		m_pRecordset->MoveNext();
	}
	DATABASE_EXIT_CHECK(bIsLogIn);
	return TRUE;
}

BOOL DBAccessConn::GetColumnNameFromTable(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn)
{
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PDBOPERATEINFO pDbInfo = &pAdoInfo->stuDBOperate;
	DATABASE_LOGIN_CHECK(bIsLogIn, pLogInfo);

	pDbInfo->strOutQueryColumnResult.clear();//清空查询结果

	CString strSQL = "select * from "+pDbInfo->strInQueryedColTableName;
	m_pRecordset = GetRecordSet((_bstr_t)strSQL);
	_bstr_t bstr;
	try
	{
		long nfieldCount = m_pRecordset->GetFields()->GetCount();
		for (int i = 0; i < nfieldCount; ++i)
		{
			bstr = m_pRecordset->GetFields()->GetItem(_variant_t((long)i))->GetName();
			pDbInfo->strOutQueryColumnResult.push_back(LPCTSTR(bstr));  
		}
	}
	catch (_com_error e)
	{
		MessageBox(NULL,e.ErrorMessage(),"错误",MB_OK);
	}

	DATABASE_EXIT_CHECK(bIsLogIn);
	return TRUE;
}
void* DBAccessConn::GetHandel(int nIndex)
{
	void *pHandle = nullptr;
	switch (nIndex)
	{
	case 0:
		pHandle = m_pConnection;
		break;
	case 1:
		pHandle = m_pRecordset;
	default:
		break;
	}
	return pHandle;
}

