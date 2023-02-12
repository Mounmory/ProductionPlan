#pragma once
#include "DBCommonAPI.h"
#include "sqlite3/sqlite3.h"

//#define USING_STD_STRING//结构体中使用标准库中string

#define DB_SQLITE_FUNC_API DB_BASE_FUNC_API

class DB_SQLITE_FUNC_API DBSqliteConn final : public DBCommonAPI {
public:
	DBSqliteConn();
	~DBSqliteConn();
public:
	virtual BOOL CheckConnecInfo(PDBLOGINFO pAdoInfo) override;//检查登录信息是否正确//overvide表示派生类中函数覆盖基类中函数，未覆盖则报错
	virtual BOOL OnInitADOConn(PDBLOGINFO pAdoInfo) override;//连接数据库
	virtual BOOL ExitConnect();//关闭数据库

	virtual BOOL GetDataFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;		//查询数据库
	virtual BOOL AddDataToDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;		//向数据库添加数据
	virtual BOOL DeleteAllDataFromTable(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;	//删除数据库中数据
	virtual BOOL UpdateDataFromToBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;		//更新数据库中数据

	virtual BOOL CreatDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;//创建数据库
	virtual BOOL DeleteTableFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;//删除表格
	virtual BOOL GetTableNameFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;//获取表格名称
	virtual BOOL GetColumnNameFromTable(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn = TRUE) override;//获取列名称

	virtual void* GetHandel(int nIndex = 0) override;
private:
	sqlite3* m_pHandle;//Database Connection Handle
	sqlite3_stmt *m_pstmt;//Prepared Statement Object
};

//SQLite的操作接口
//
//SQLite的2个重要结构体：
//
//sqlite3 *pdb，数据库句柄，跟文件句柄FILE很类似
//sqlite3_stmt *stmt，这个相当于ODBC的Command对象，用于保存编译好的SQL语句
//
//SQLite的5个主要的函数：
//
//sqlite3_open()， 打开数据库
//sqlite3_exec()，执行非查询的sql语句
//sqlite3_prepare()，准备sql语句，执行select语句或者要使用parameter bind时，用这个函数（封装了sqlite3_exec）.
//sqlite3_step()，在调用sqlite3_prepare后，使用这个函数在记录集中移动。
//sqlite3_close()，关闭数据库文件
//
//还有一系列的函数，用于从记录集字段中获取数据，如：
//
//sqlite3_column_text()，取text类型的数据
//sqlite3_column_blob()，取blob类型的数据
//sqlite3_column_int()，取int类型的数据