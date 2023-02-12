#pragma once
#include "stdafx.h"
#include "DataType.h"

enum class EnumDBType
{
	DATA_SOURCE_UNKNOW = 0,
	DATA_SOURCE_ACCESS = 1,	//SQL Server 数据库
	DATA_SOURCE_SQLSERVER = 2,		//ACCESS 数据库 
	DATA_SOURCE_SQLITE = 3		//sqlite数据库
};

#define DB_BASE_FUNC_API __declspec(dllexport)

// 此类是从 ADOConn.dll 导出的
class DB_BASE_FUNC_API DBCommonAPI {
public:
	DBCommonAPI() = default;
	virtual ~DBCommonAPI() = default;
public:
	// TODO: 在此添加您的方法。
	virtual BOOL CheckConnecInfo(PDBLOGINFO pAdoInfo) = 0;//检查登录信息是否正确
	virtual BOOL OnInitADOConn(PDBLOGINFO pAdoInfo) = 0;//连接数据库
	virtual BOOL ExitConnect() = 0;//关闭数据库

	virtual BOOL GetDataFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;		//查询数据库
	virtual BOOL AddDataToDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;		//向数据库添加数据
	virtual BOOL DeleteAllDataFromTable(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;	//删除数据库中数据
	virtual BOOL UpdateDataFromToBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;		//更新数据库中数据

	virtual BOOL CreatDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;//创建数据库
	virtual BOOL DeleteTableFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;//删除表格
	virtual BOOL GetTableNameFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;//获取表格名称
	virtual BOOL GetColumnNameFromTable(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) = 0;//获取列名称

	virtual void* GetHandel(int nIndex = 0) = 0;
};

