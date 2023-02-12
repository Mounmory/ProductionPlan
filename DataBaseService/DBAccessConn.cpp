// ADOConn.cpp : ���� DLL Ӧ�ó���ĵ���������
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
	//У����Ϣ�Ƿ���ȷ

	if (pAdoInfo->strDataSource == "")
	{
		MessageBox(NULL,_T("��¼��Ϣ����ACCESS���ݿ�������Ϣ����"),_T("����"),MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL DBAccessConn::OnInitADOConn(PDBLOGINFO pAdoInfo)
{
	if (m_bIsLog)//����Ѿ���¼��
	{
		//���������־��Ϣ
		return TRUE;
	}
	CString strConnec;
	//���������ַ���

#if defined _WIN64//�����64λ���� 
	strConnec.Format(_T("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=%s;"),pAdoInfo->strDataSource);
#elif defined _WIN32//�����32λ���� 
	strConnec.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;"),pAdoInfo->strDataSource);
#endif

	//��ʼ��OLE/COM�⻷��
	::CoInitialize(NULL);
	try
	{
		//����connection����
		m_pConnection.CreateInstance("ADODB.Connection");	   
		m_pConnection->Open((_bstr_t)strConnec,"","",adModeUnknown);
	}
	//��׽�쳣
	catch(_com_error e)
	{
		CString strError = "�������ݿ�"+pAdoInfo->strDataSource+"ʧ�ܣ�"+(LPCSTR)e.Description();
		MessageBox(NULL,strError,_T("����"),MB_ICONEXCLAMATION);
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
			MessageBox(NULL,_T("��¼������ָ��Ϊ�գ�"),_T("����"),MB_ICONEXCLAMATION);
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		switch(adType)
		{
		case RecordSetOpenType::AD_OPEN_DANAMIC:
			// ��̬�α�,���е��޸ĶԵ�ǰ�û��Ǽ���
			m_pRecordset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);
			break;
		case RecordSetOpenType::AD_OPEN_STATIC:
			//��̬�α� ,�����û����޸ĶԵ�ǰ�û��ǲ��ɼ���. 
			m_pRecordset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenStatic,adLockOptimistic,adCmdText);
			break;
		case RecordSetOpenType::AD_OPEN_KEYSET:
			//��ǰ���ֵ�����   ,   �ĶԵ�ǰ�û��Ǽ���.   ����������͵�.
			m_pRecordset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenKeyset,adLockOptimistic,adCmdText);
			break;
		default:
			break;
		}
	}
	catch(_com_error e)
	{
		CString strError =(LPCSTR)e.Description();
		MessageBox(NULL,strError,_T("����"),MB_ICONEXCLAMATION);
		if (m_pRecordset->State)
			m_pRecordset->Close();
	}
	return m_pRecordset;
}


BOOL DBAccessConn::ExitConnect()
{
	//�رռ�¼��������
	m_bIsLog = FALSE;//�����Ƿ�ر�ʧ�ܣ���Ӧ����Ϊfalse
	try
	{
		////����һ�ַ�����Ч��Ӧ��һ��
		//if(m_pRecordset->GetState() == adStateOpen) //�жϵ�ǰ�ļ�¼��״̬
		//	m_pRecordset->Close();     //�رռ�¼��
		if(m_pRecordset->State)
			m_pRecordset->Close();
		if(m_pConnection->State)
			m_pConnection->Close();
		//�ͷŻ���
		::CoUninitialize();
	}
	catch(_com_error e)
	{
		CString strError =(LPCSTR)e.Description();
		MessageBox(NULL,strError,_T("����"),MB_ICONEXCLAMATION);
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
	int nRowCount = m_pRecordset->GetRecordCount();//����
	int nColCount = pValueInfo->szInReadColumnName.size();//����

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

	//�ر����ݿ�
	DATABASE_EXIT_CHECK(bIsLogIn);
	return TRUE;
}

BOOL DBAccessConn::CreatDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn)
{
	PDBOPERATEINFO pCreatDb = &pAdoInfo->stuDBOperate;//�������ݿ���Ϣ

	CString strDataSource = pCreatDb->strInDBPath + "\\" + pCreatDb->strInDBname + ".mdb";		
	if (pCreatDb->EnuCreatType == EnumDBCreatType::CREAT_DB || pCreatDb->EnuCreatType == EnumDBCreatType::CREAT_DB_AND_TABLE)//�������ݿ�
	{
		try
		{
			//��ʼ��OLE/COM�⻷��
			::CoInitialize(NULL);
			if (m_pCatalog == nullptr)
			{
				m_pCatalog.CreateInstance(__uuidof(Catalog));
			}
#if defined _WIN64//�����64λ���� 
			CString strSQL ="Provider=Microsoft.ACE.OLEDB.12.0;Data Source=" + strDataSource + ";";
#elif defined _WIN32//�����32λ���� 
			CString strSQL ="Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" + strDataSource + ";";
#endif
			m_pCatalog->Create((_bstr_t)strSQL);
			//�ͷŻ���
			::CoUninitialize();
		}
		catch(_com_error e)
		{
			CString strError = "�������ݿ� "+ pCreatDb->strInDBname+" ʧ�ܣ�"+(LPCSTR)e.Description();
			MessageBox(NULL,strError,_T("��ʾ��"),MB_OK);
			return FALSE;
		}
		//m_pCatalog->Release();
	}
	if (pCreatDb->EnuCreatType == EnumDBCreatType::CREAT_TABLE || pCreatDb->EnuCreatType == EnumDBCreatType::CREAT_DB_AND_TABLE)//�������
	{
		if(bIsLogIn)
		{
			DBLOGINFO LogInfo ;//���ݿ��¼��Ϣ
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
		//if(m_pRecordset->GetState() != adStateOpen)//������ȷ��񣬶��᷵��0
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
		MessageBox(NULL,_T("�������ݿ⼰���δ֪�Ĵ������ͣ�"),_T("����"),MB_ICONEXCLAMATION);
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
	//if(m_pRecordset->adoEOF==0)//����������Ϊ�գ��޷�д������
	if(m_pRecordset->GetState() == adStateOpen)
	{
		//����У�����
		int nWriteColumnNum = pValueInfo->szInWtriteColuName.size();//Ҫд�������
		int nWriteRowNum =  pValueInfo->szInWriteValue.getRowCount();//Ҫд�������

		if (nWriteColumnNum > pValueInfo->szInWriteValue.getColCount())//У��Ҫд�������,�����������
		{
			nWriteColumnNum = pValueInfo->szInWriteValue.getColCount();
		}	
		//д������
		try
		{
			for (int i = 0 ; i < nWriteRowNum ; ++i)
			{
				m_pRecordset->AddNew();//Ĭ�ϲ嵽��β
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
			MessageBox(NULL,strError,_T("����"),MB_ICONEXCLAMATION);
			ExitConnect();
			return FALSE;
		}
		//�ر����ݿ�
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
			MessageBox(NULL,strError,_T("����"),MB_ICONEXCLAMATION);
			ExitConnect();
			return FALSE;
		}
	}
	//�ر����ݿ�
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
	//�ر����ݿ�
	DATABASE_EXIT_CHECK(bIsLogIn);
	return TRUE;
}

BOOL DBAccessConn::DeleteTableFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn)//ɾ�����
{
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PDBOPERATEINFO pDbInfo = &pAdoInfo->stuDBOperate;
	DATABASE_LOGIN_CHECK(bIsLogIn, pLogInfo);

	CString strDelTable = "drop table "+pDbInfo->strInDelTableName+";";
	m_pRecordset = GetRecordSet((_bstr_t)strDelTable);
	//if(!GetRecordSet((_bstr_t)pDeleTable->strDelSQL)->State)//������ȷ���GetRecordSet������0
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

	pDbInfo->strOutQueryTableResult.clear();//��ղ�ѯ���

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

	pDbInfo->strOutQueryColumnResult.clear();//��ղ�ѯ���

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
		MessageBox(NULL,e.ErrorMessage(),"����",MB_OK);
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


//�ο���Ϣ
//https://blog.csdn.net/jintingbo/article/details/81457245?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7Edefault-1.no_search_link&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7Edefault-1.no_search_link&utm_relevant_index=2


DB_BASE_FUNC_API int ConvertStrinToCodeBYTE(BYTE *pbDest,const BYTE *pbSrc)
{
	int nLen = strlen((char*)pbSrc);
	if (nLen <= 0)
	{
		return 0;
	}
	BYTE TransFlag = rand()%256 & 0xFF;
	BYTE byTempSrc[MAX_STR_LEN] = {0};
	memcpy(byTempSrc,pbSrc,nLen);
	byTempSrc[nLen] = TransFlag;
	for(int i = 0;i<nLen;++i)
	{
		byTempSrc[i]^= TransFlag>>(i%8);
	}
	//�����ַ���
	char sztmp[4];
	for(int i = 0;i<=nLen;++i)
	{
		sprintf_s(sztmp,"%02X",byTempSrc[i]);
		strcat_s((char*)pbDest,strlen(sztmp),sztmp);
	}
	return (2*nLen+2);
}


DB_BASE_FUNC_API int ConvertCodeToStringBYTE(BYTE *pbDest,const BYTE *pbSrc)
{
	int nLenSrc = strlen((char*)pbSrc);
	int nLenDes = nLenSrc/2;
	if (nLenDes <= 0)
	{
		return 0;
	}
	//��Strngת��ΪHEX
	BYTE h1,h2;
	BYTE s1,s2;
	for (int i=0; i<nLenDes; i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i+1];
		s1 = toupper(h1) - 0x30;
		if (s1 > 9)
			s1 -= 7;
		s2 = toupper(h2) - 0x30;
		if (s2 > 9)
			s2 -= 7;
		pbDest[i] = s1*16 + s2;
	}
	//����Ϊԭ�ַ�
	for(int i = 0;i<nLenDes-1;++i)
	{
		pbDest[i] ^= pbDest[nLenDes-1]>>(i%8);
	}
	pbDest[nLenDes-1] = 0x00;
	return nLenDes-1;
}