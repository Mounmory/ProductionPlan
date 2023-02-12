#include "stdafx.h"
#include "DBSqliteConn.h"

#pragma comment(lib,"sqlite3.lib")
#include "DataType.h"


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

DBSqliteConn::DBSqliteConn()
{
}


DBSqliteConn::~DBSqliteConn()
{
}
BOOL DBSqliteConn::CheckConnecInfo(PDBLOGINFO pAdoInfo)//检查登录信息是否正确
{

	if (pAdoInfo->strDataSource == "")
	{
		MessageBox(NULL, _T("登录信息错误：SQLITE数据库连接信息错误！"), _T("错误"), MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL DBSqliteConn::OnInitADOConn(PDBLOGINFO pAdoInfo)//检查登录信息是否正确
{
	//不存在即创建数据库
	std::string strDBPath = pAdoInfo->strDataSource;
	//打开（没有则创建）数据库
	if (sqlite3_open(strDBPath.c_str(), &m_pHandle))
	{
		CString strError = sqlite3_errmsg(m_pHandle);
		MessageBox(NULL,strError,"错误",MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL DBSqliteConn::ExitConnect()//检查登录信息是否正确
{
	if (sqlite3_close(m_pHandle) != SQLITE_OK)
	{
		CString strError = sqlite3_errmsg(m_pHandle);
		MessageBox(NULL, strError, "错误", MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
	
}
BOOL DBSqliteConn::GetDataFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn)
{
	BOOL bResult = TRUE;
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PVALUEINFO pValueInfo = &pAdoInfo->stuValueInfo;
	DATABASE_LOGIN_CHECK(bIsLogIn, pLogInfo);

	{
		std::string strSql = pValueInfo->strInReadSQL;
		char *zErrMsg = nullptr;
		char** pValues;
		int nRow, nCol;
		//sqlite3_prepare_v2(m_pHandle, strSql.c_str(),strSql.length(), m_pstmt,0);
		int statu = sqlite3_get_table(m_pHandle, strSql.c_str(), &pValues, &nRow, &nCol, &zErrMsg);
		if (statu != SQLITE_OK)
		{
			CString strError = zErrMsg;
			MessageBox(NULL, strError, "错误", MB_ICONEXCLAMATION);
			sqlite3_free(zErrMsg);
			bResult = FALSE;
		}
		else if(nRow > 0)
		{
			int nRowCount = nRow;//行数
			int nColCount = pValueInfo->szInReadColumnName.size();//列数
			pValueInfo->szOutReadValue.reSizeArray(nRowCount, nColCount);

			DynaArray2D<STRING> temReadValue(nRow, nCol);//保存临时信息
			std::map<STRING, size_t> temMap;//列名称，列索引
			char* tmpChar;
			//获取列名称及索引
			for (int col = 0; col < nCol; ++col)
			{
				tmpChar = pValues[col];
				temMap.insert(std::make_pair(tmpChar, col));
			}
			//获取所有数据
			int nIndex = nCol;
			for (int i = 0; i < nRow; ++i)
			{
				for (int j = 0; j < nCol; ++j)
				{
					tmpChar = pValues[nIndex++];
					temReadValue[i][j] = tmpChar;
				}
			}
			sqlite3_free_table(pValues);
			//返回结果
			for (size_t colIdex = 0; colIdex < nColCount; ++colIdex)
			{
				auto iterFind = temMap.find(pValueInfo->szInReadColumnName[colIdex]);
				if (iterFind != temMap.end())
				{
					for (int i = 0; i < nRow; ++i)
					{
						pValueInfo->szOutReadValue[i][colIdex] = temReadValue[i][iterFind->second];
					}
				}
				else
				{
					pValueInfo->szOutReadValue.reSizeArray(0, 0);
					bResult = false;
					MessageBox(NULL, CString("查询列" + pValueInfo->szInReadColumnName[colIdex] + "不存在！"), "错误", MB_ICONEXCLAMATION);
					break;
				}
			}
		}
	}

	DATABASE_EXIT_CHECK(bIsLogIn, pLogInfo);
	return bResult;
}
BOOL DBSqliteConn::AddDataToDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn)
{
	BOOL bResult = TRUE;
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PVALUEINFO pValueInfo = &pAdoInfo->stuValueInfo;
	DATABASE_LOGIN_CHECK(bIsLogIn, pLogInfo);

	//构造sql语句
	std::string strSql = "INSERT INTO " + pValueInfo->strInWriteSQL + "(";
	std::string strValue = " VALUES(";
	for (auto iter: pValueInfo->szInWtriteColuName)
	{
		strSql += iter;
		strSql += ",";
		strValue += "?,";
	}
	strSql[strSql.length() - 1] = 0x20;
	strValue[strValue.length() - 1] = 0x20;
	strSql += ") ";
	strValue += ");";
	strSql += strValue;
	//const char* sql_insert = "INSERT INTO COMPANY (ID,TIME_START,DETECT_IP,NOTE) VALUES(?,?,?,?);";
	sqlite3_exec(m_pHandle, "begin;", 0, 0, 0);//开启事务
	int statu = sqlite3_prepare_v2(m_pHandle, strSql.c_str(), strSql.length(), &m_pstmt, 0);

	if (statu == SQLITE_OK)
	{
		int nWriteColumnNum = pValueInfo->szInWtriteColuName.size();//要写入的列数
		int nWriteRowNum = pValueInfo->szInWriteValue.getRowCount();//要写入的行数

		if (nWriteColumnNum > pValueInfo->szInWriteValue.getColCount())//校验要写入的列数,基本不会出错
		{
			nWriteColumnNum = pValueInfo->szInWriteValue.getColCount();
		}
		//写入数据
		std::string strTmp;
		for (int i = 0; i < nWriteRowNum; ++i)
		{
			for (int j = 0; j < nWriteColumnNum; ++j)
			{
				strTmp = pValueInfo->szInWriteValue[i][j];
				statu = sqlite3_bind_text(m_pstmt, j + 1, strTmp.c_str(), strTmp.length(), SQLITE_TRANSIENT);
			}
			statu = sqlite3_step(m_pstmt);
			statu = sqlite3_clear_bindings(m_pstmt);
			statu = sqlite3_reset(m_pstmt);

		}
		statu = sqlite3_clear_bindings(m_pstmt);
		statu = sqlite3_finalize(m_pstmt);
		
	}
	else 
	{
		bResult = FALSE;
	}
	statu = sqlite3_exec(m_pHandle, "commit;", 0, 0, 0);//提交事务
	if (statu != SQLITE_OK)
	{
		bResult = FALSE;
	}
	//关闭数据库
	DATABASE_EXIT_CHECK(bIsLogIn);
	return bResult;
}
BOOL DBSqliteConn::DeleteAllDataFromTable(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn)
{
	return true;
}
BOOL DBSqliteConn::UpdateDataFromToBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn)
{
	return true;
}
BOOL DBSqliteConn::CreatDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn)
{
	BOOL bResult = TRUE;
	PDBOPERATEINFO pCreatDb = &pAdoInfo->stuDBOperate;//创建数据库信息

	//if (pCreatDb->EnuCreatType == CREAT_DB || pCreatDb->EnuCreatType == EnumDBCreatType::CREAT_DB_AND_TABLE)//创建数据库
	//{
		std::string strSql = pCreatDb->strInDBPath + "\\" + pCreatDb->strInDBname + ".db";
		if (sqlite3_open(strSql.c_str(), &m_pHandle) != SQLITE_OK)
		{
			bResult = FALSE;
			return bResult;
		}
	//}
	if (pCreatDb->EnuCreatType == EnumDBCreatType::CREAT_TABLE || pCreatDb->EnuCreatType == EnumDBCreatType::CREAT_DB_AND_TABLE)//创建表格
	{
		int statu;
		char *zErrMsg = 0;
		std::string strSql = pCreatDb->strInCreatTableSQL;
		statu = sqlite3_exec(m_pHandle, strSql.c_str(), NULL/*callback*/, 0, &zErrMsg);//回调函数貌似没什么用
		if (statu != SQLITE_OK) 
		{
			CString strError = zErrMsg;
			MessageBox(NULL,strError,"错误",MB_ICONEXCLAMATION);
			sqlite3_free(zErrMsg);
			bResult = FALSE;
		}
		else
		{
			bResult = TRUE;
		}
	}

	if (TRUE == bIsLogIn)
	{
		sqlite3_close(m_pHandle);
	}

	return bResult;
}
BOOL DBSqliteConn::DeleteTableFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn)
{
	BOOL bResult = TRUE;
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PDBOPERATEINFO pDbInfo = &pAdoInfo->stuDBOperate;
	DATABASE_LOGIN_CHECK(bIsLogIn, pLogInfo);
	std::string strSql = "DROP TABLE IF EXISTS " + pDbInfo->strInDelTableName + ";";
	char *zErrMsg = nullptr;
	int statu = sqlite3_exec(m_pHandle, strSql.c_str(), NULL/*callback*/, 0, &zErrMsg);//回调函数貌似没什么用
	if (statu != SQLITE_OK)
	{
		CString strError = zErrMsg;
		MessageBox(NULL, strError, "错误", MB_ICONEXCLAMATION);
		sqlite3_free(zErrMsg);
		bResult = FALSE;
	}
	
	DATABASE_EXIT_CHECK(bIsLogIn);
	return bResult;
}
BOOL DBSqliteConn::GetTableNameFromDataBase(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn)
{
	BOOL bResult = TRUE;
	PDBLOGINFO pLogInfo = &pAdoInfo->stuLogInfo;
	PDBOPERATEINFO pDbInfo = &pAdoInfo->stuDBOperate;
	DATABASE_LOGIN_CHECK(bIsLogIn, pLogInfo);

	pDbInfo->strOutQueryTableResult.clear();//清空查询结果

	CString strTableType;

	std::string strSql = "SELECT name FROM sqlite_master WHERE type = 'table'";
	char *zErrMsg = nullptr;
	char** pValues;
	int nRow,nCol;
	//sqlite3_prepare_v2(m_pHandle, strSql.c_str(),strSql.length(), m_pstmt,0);
	int statu = sqlite3_get_table(m_pHandle, strSql.c_str(), &pValues, &nRow, &nCol, &zErrMsg);
	if (statu != SQLITE_OK)
	{
		CString strError = zErrMsg;
		MessageBox(NULL, strError, "错误", MB_ICONEXCLAMATION);
		sqlite3_free(zErrMsg);
		bResult = FALSE;
	}
	else 
	{
		std::string strvalue;
		for (int i = 1; i < nRow; ++i)//索引0的是系统表sqlite_sequence
		{
			strvalue = pValues[(i+1)*nCol];
			pDbInfo->strOutQueryTableResult.push_back(strvalue.c_str());
		}
		sqlite3_free_table(pValues);
	}

	DATABASE_EXIT_CHECK(bIsLogIn, pLogInfo);
	return bResult;
}
BOOL DBSqliteConn::GetColumnNameFromTable(std::shared_ptr<ADODBINFO> pAdoInfo, bool bIsLogIn)
{
	return true;
}

void* DBSqliteConn::GetHandel(int nIndex)
{
	void *pHandle = nullptr;
	switch (nIndex) 
	{
	case 0:
		pHandle = m_pHandle;
		break;
	case 1:
		pHandle = m_pstmt;
	default:
		break;
	}
	return pHandle;
}