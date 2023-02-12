#pragma once
#include "stdafx.h"
#include "DataType.h"

enum class EnumDBType
{
	DATA_SOURCE_UNKNOW = 0,
	DATA_SOURCE_ACCESS = 1,	//SQL Server ���ݿ�
	DATA_SOURCE_SQLSERVER = 2,		//ACCESS ���ݿ� 
	DATA_SOURCE_SQLITE = 3		//sqlite���ݿ�
};

#define DB_BASE_FUNC_API __declspec(dllexport)

// �����Ǵ� ADOConn.dll ������
class DB_BASE_FUNC_API DBCommonAPI {
public:
	DBCommonAPI() = default;
	virtual ~DBCommonAPI() = default;
public:
	// TODO: �ڴ�������ķ�����
	virtual BOOL CheckConnecInfo(PDBLOGINFO pAdoInfo) = 0;//����¼��Ϣ�Ƿ���ȷ
	virtual BOOL OnInitADOConn(PDBLOGINFO pAdoInfo) = 0;//�������ݿ�
	virtual BOOL ExitConnect() = 0;//�ر����ݿ�

	virtual BOOL GetDataFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;		//��ѯ���ݿ�
	virtual BOOL AddDataToDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;		//�����ݿ��������
	virtual BOOL DeleteAllDataFromTable(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;	//ɾ�����ݿ�������
	virtual BOOL UpdateDataFromToBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;		//�������ݿ�������

	virtual BOOL CreatDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;//�������ݿ�
	virtual BOOL DeleteTableFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;//ɾ�����
	virtual BOOL GetTableNameFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;//��ȡ�������
	virtual BOOL GetColumnNameFromTable(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;//��ȡ������

	virtual void* GetHandel(int nIndex = 0) = 0;
};

