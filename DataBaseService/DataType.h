#pragma once
#include "stdafx.h"

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include "atlstr.h"
#include <memory>
#include "DynaArray.h"


//创建数据库操作类新
enum class EnumDBCreatType
{
	CREAT_UNKNOW = 0,		//未知数据库类型
	CREAT_DB = 1,			//仅创建数据库
	CREAT_TABLE	= 2,		//仅创建表格
	CREAT_DB_AND_TABLE = 3	//创建数据库及表格
};
//记录集打开方式
enum class RecordSetOpenType
{
	AD_OPEN_DANAMIC = 0,
	AD_OPEN_STATIC = 1,
	AD_OPEN_KEYSET = 2
};

//创建表格时的类型
#define COLUMN_TYPE_BIT			"BIT"			//整型
#define COLUMN_TYPE_INTEGER		"INTEGER"		//整型
#define COLUMN_TYPE_REAL		"REAL"			//实型
#define COLUMN_TYPE_TEXT		"TEXT"			//文本
#define COLUMN_TYPE_DATE		"DATE"		//日期
#define COLUMN_TYPE_TIME		"TIME"			//时间
#define COLUMN_TYPE_DATETIME	"DATETIME"		//时间

//#ifdef USING_STD_STRING
//#define STRING std::string 
//#else
//#define STRING CString 
//#endif // WIN_CSTRING

////第二种方法
//#ifdef USING_STD_STRING
//typedef std::string STRING;
//#else
//typedef CString	STRING;
//#endif

////第三种方法
//#ifdef USING_STD_STRING
//using STRING = std::string;
//#else
//using STRING = CString;
//#endif

typedef CString	STRING;
//登录信息
typedef struct ADOLogInfo
{
	STRING strUserID ;//登用户名
	STRING strPassword ;//用户密码
	STRING	strDBName ;//数据库名称
	//STRING	strServerIP;//服务器IP
	STRING strDataSource ;//数据源信息
}DBLOGINFO,*PDBLOGINFO;

//读、写数据库的信息
typedef struct ValueInfo
{
	//读数据库信息
	STRING  strInReadSQL;//SQL语句
	std::vector<STRING> szInReadColumnName;//存储读取的列名称
	DynaArray2D<STRING> szOutReadValue;//读取信息

	//写数据库信息
	STRING  strInWriteSQL;//SQL语句
	std::vector<STRING> szInWtriteColuName;//要写的列
	DynaArray2D<STRING> szInWriteValue;

	//更新数据库信息
	STRING  strUpdateSQL;//SQL语句
	std::vector<STRING> szUpdateColuName;//更新的列
	std::vector<STRING> szUpdateValue;//更新的值
	int nUpdateColumnNum = 0;//更新列的数量

	//删除数据库中内容
	//STRING  strInDeletSQL;
	STRING  strInDelTableName;

}VALUEINFO,*PVALUEINFO;

//新建查询数据库信息
typedef struct DBOperInfo
{
	//新建数据库及表格相关信息
	STRING strInDBname;	//创建的数据库名称
	STRING strInDBPath;	//数据库路径
	STRING strInCreatTableSQL;		//创建表语句
	EnumDBCreatType EnuCreatType = EnumDBCreatType::CREAT_UNKNOW;		//创建类型
	//删除表格信息
	STRING strInDelTableName;

	//查询表格结果
	std::vector<STRING> strOutQueryTableResult;//查询数据库中表格名称

	//查询表格中列名称结果
	STRING strInQueryedColTableName;
	std::vector<STRING> strOutQueryColumnResult;//查询表格中列名称

	//清空信息
	void ClearCreatInfo()
	{
		EnuCreatType = EnumDBCreatType::CREAT_UNKNOW;
	}
}DBOPERATEINFO,*PDBOPERATEINFO;

//数据库信息结构体

typedef struct ADODatabaseInfo
{
	DBLOGINFO stuLogInfo;		//登录信息
	VALUEINFO stuValueInfo;		//操作数据库信息
	DBOPERATEINFO stuDBOperate;	//新建数据库信息

}COMMONDBINFO,ADODBINFO,*PADODBINFO,SQLITEDBINFO,*PSQLITEDBINFO;