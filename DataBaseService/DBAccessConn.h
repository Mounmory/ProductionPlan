// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� BASE_FUNC_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// BASE_FUNC_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

#pragma once
#include "DBCommonAPI.h"
//#include "DataType.h"
//#include <atlstr.h>

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")rename("BOF","adoBOF")
#import "C:\Program Files\Common Files\System\ado\Msadox.dll"  rename_namespace("ADOCG") rename("EOF", "adoXEOF") rename("DataTypeEnum","adoXDataTypeEnum") 

using namespace ADOCG;

#define DB_ACCESS_FUNC_API DB_BASE_FUNC_API

// �����Ǵ� ADOConn.dll ������
class DB_ACCESS_FUNC_API DBAccessConn final: public DBCommonAPI {//final�ؼ��֣�������Ϊ���౻�̳�
public:
	DBAccessConn(void);
	~DBAccessConn();

public:
	virtual BOOL CheckConnecInfo(PDBLOGINFO pAdoInfo) override;//����¼��Ϣ�Ƿ���ȷ
	virtual BOOL OnInitADOConn(PDBLOGINFO pAdoInfo) override;//�������ݿ�
	virtual BOOL ExitConnect();//�ر����ݿ�

	virtual BOOL GetDataFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;		//��ѯ���ݿ�
	virtual BOOL AddDataToDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;		//�����ݿ��������
	virtual BOOL DeleteAllDataFromTable(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;	//ɾ�����ݿ�������
	virtual BOOL UpdateDataFromToBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;		//�������ݿ�������

	virtual BOOL CreatDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;//�������ݿ�
	virtual BOOL DeleteTableFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;//ɾ�����
	virtual BOOL GetTableNameFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;//��ȡ�������
	virtual BOOL GetColumnNameFromTable(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;//��ȡ������

	virtual void* GetHandel(int nIndex = 0) override;

private:
	_RecordsetPtr& GetRecordSet(_bstr_t bstrSQL, RecordSetOpenType adType = RecordSetOpenType::AD_OPEN_DANAMIC);//ִ��SQL���
	_ConnectionPtr m_pConnection;//���һ��ָ��Connection�����ָ��
	_CommandPtr m_pCommand;//���һ��ָ��CommandPtrָ��
	_RecordsetPtr m_pRecordset;//���һ��ָ��Recordset�����ָ��
	_CatalogPtr m_pCatalog;//���һ��ָ��Catalog����ָ��
	bool m_bIsLog;
};

extern DB_BASE_FUNC_API int nADOConn;

DB_BASE_FUNC_API int fnADOConn(void);

DB_BASE_FUNC_API int ConvertStrinToCodeBYTE(BYTE *pbDest,const BYTE *pbSrc);
DB_BASE_FUNC_API int ConvertCodeToStringBYTE(BYTE *pbDest,const BYTE *pbSrc);