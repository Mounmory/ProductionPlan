// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 BASE_FUNC_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// BASE_FUNC_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

#pragma once
#include "DBCommonAPI.h"
//#include "DataType.h"
//#include <atlstr.h>

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")rename("BOF","adoBOF")
#import "C:\Program Files\Common Files\System\ado\Msadox.dll"  rename_namespace("ADOCG") rename("EOF", "adoXEOF") rename("DataTypeEnum","adoXDataTypeEnum") 

using namespace ADOCG;

#define DB_ACCESS_FUNC_API DB_BASE_FUNC_API

// 此类是从 ADOConn.dll 导出的
class DB_ACCESS_FUNC_API DBAccessConn final: public DBCommonAPI {//final关键字，不能作为基类被继承
public:
	DBAccessConn(void);
	~DBAccessConn();

public:
	virtual BOOL CheckConnecInfo(PDBLOGINFO pAdoInfo) override;//检查登录信息是否正确
	virtual BOOL OnInitADOConn(PDBLOGINFO pAdoInfo) override;//连接数据库
	virtual BOOL ExitConnect();//关闭数据库

	virtual BOOL GetDataFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;		//查询数据库
	virtual BOOL AddDataToDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;		//向数据库添加数据
	virtual BOOL DeleteAllDataFromTable(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;	//删除数据库中数据
	virtual BOOL UpdateDataFromToBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;		//更新数据库中数据

	virtual BOOL CreatDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;//创建数据库
	virtual BOOL DeleteTableFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;//删除表格
	virtual BOOL GetTableNameFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;//获取表格名称
	virtual BOOL GetColumnNameFromTable(std::shared_ptr<ADODBINFO> pAdoInfo,bool bIsLogIn = TRUE) override;//获取列名称

	virtual void* GetHandel(int nIndex = 0) override;

private:
	_RecordsetPtr& GetRecordSet(_bstr_t bstrSQL, RecordSetOpenType adType = RecordSetOpenType::AD_OPEN_DANAMIC);//执行SQL语句
	_ConnectionPtr m_pConnection;//添加一个指向Connection对象的指针
	_CommandPtr m_pCommand;//添加一个指向CommandPtr指针
	_RecordsetPtr m_pRecordset;//添加一个指向Recordset对象的指针
	_CatalogPtr m_pCatalog;//添加一个指向Catalog对象指针
	bool m_bIsLog;
};

extern DB_BASE_FUNC_API int nADOConn;

DB_BASE_FUNC_API int fnADOConn(void);

DB_BASE_FUNC_API int ConvertStrinToCodeBYTE(BYTE *pbDest,const BYTE *pbSrc);
DB_BASE_FUNC_API int ConvertCodeToStringBYTE(BYTE *pbDest,const BYTE *pbSrc);