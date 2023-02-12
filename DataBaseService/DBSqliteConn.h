#pragma once
#include "DBCommonAPI.h"
#include "sqlite3/sqlite3.h"

//#define USING_STD_STRING//�ṹ����ʹ�ñ�׼����string

#define DB_SQLITE_FUNC_API DB_BASE_FUNC_API

class DB_SQLITE_FUNC_API DBSqliteConn final : public DBCommonAPI {
public:
	DBSqliteConn();
	~DBSqliteConn();
public:
	virtual BOOL CheckConnecInfo(PDBLOGINFO pAdoInfo) override;//����¼��Ϣ�Ƿ���ȷ//overvide��ʾ�������к������ǻ����к�����δ�����򱨴�
	virtual BOOL OnInitADOConn(PDBLOGINFO pAdoInfo) override;//�������ݿ�
	virtual BOOL ExitConnect();//�ر����ݿ�

	virtual BOOL GetDataFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;		//��ѯ���ݿ�
	virtual BOOL AddDataToDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;		//�����ݿ��������
	virtual BOOL DeleteAllDataFromTable(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;	//ɾ�����ݿ�������
	virtual BOOL UpdateDataFromToBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;		//�������ݿ�������

	virtual BOOL CreatDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;//�������ݿ�
	virtual BOOL DeleteTableFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;//ɾ�����
	virtual BOOL GetTableNameFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;//��ȡ�������
	virtual BOOL GetColumnNameFromTable(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;//��ȡ������

	virtual void* GetHandel(int nIndex = 0) override;
private:
	sqlite3* m_pHandle;//Database Connection Handle
	sqlite3_stmt *m_pstmt;//Prepared Statement Object
};

//SQLite�Ĳ����ӿ�
//
//SQLite��2����Ҫ�ṹ�壺
//
//sqlite3 *pdb�����ݿ��������ļ����FILE������
//sqlite3_stmt *stmt������൱��ODBC��Command�������ڱ������õ�SQL���
//
//SQLite��5����Ҫ�ĺ�����
//
//sqlite3_open()�� �����ݿ�
//sqlite3_exec()��ִ�зǲ�ѯ��sql���
//sqlite3_prepare()��׼��sql��䣬ִ��select������Ҫʹ��parameter bindʱ���������������װ��sqlite3_exec��.
//sqlite3_step()���ڵ���sqlite3_prepare��ʹ����������ڼ�¼�����ƶ���
//sqlite3_close()���ر����ݿ��ļ�
//
//����һϵ�еĺ��������ڴӼ�¼���ֶ��л�ȡ���ݣ��磺
//
//sqlite3_column_text()��ȡtext���͵�����
//sqlite3_column_blob()��ȡblob���͵�����
//sqlite3_column_int()��ȡint���͵�����