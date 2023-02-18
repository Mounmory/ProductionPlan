#pragma once
#include <mutex>
#include "DataType.h"
#include "DynaArray.h"
#include "DBAccessConn.h"
#include "DBSqliteConn.h"
#include "Logger.h"
#include <queue>
#include <set>

#define DB_SERVICE_API DB_BASE_FUNC_API

namespace DataBase
{
	enum class DBCommandType
	{
		DATABASE_NONE = -1,
		DATABASE_READ_DB = 0,//完成
		DATABASE_CREATE_DB,
		DATABASE_UPTATE_DB,
		DATABASE_DELETE_DB,
		DATABASE_CRATE_TABLE,//完成
		DATABASE_UPTATE_TABLE,
		DATABASE_DELETE_TABLE//完成
	};

	struct DBCommand
	{
		uint32_t cmdID ;
		DBCommandType cmdType;
		STRING strDBName;
		STRING strTableName;
		STRING strSQL;//SQL语句
		DynaArray2D<STRING> arrValue;//
	};

	struct DBResult
	{
		uint32_t cmdID;
		DBCommandType cmdType;
		STRING strDBName;
		STRING strTableName;
		bool bResult;//执行结果
	};
	/***********抽象基类，接收数据库操作执行结果*********/
	class DB_SERVICE_API IListenner //作为客户端类的抽象基类，采用观察者设计模式，处理异步命令结果
	{
	public:
		IListenner() = default;
		virtual ~IListenner() = default;
	public:
		virtual void dealDBResult(std::queue<DBResult>& rst) = 0;

	};

	/*****************数据库表格信息********************/
	class DB_SERVICE_API TableInfo//表格信息类，用于保存数据库中表格信息
	{
	public:
		TableInfo();
		~TableInfo();
		TableInfo(const TableInfo& rhs);
		TableInfo(TableInfo&& ths) noexcept;
	public:
		TableInfo& operator=(const TableInfo& rhs);
		TableInfo& operator=(TableInfo&& rhs) noexcept;

		//设置列
		void setCols(const std::vector<STRING>& cols);
		void oderByCols(const STRING& colName,const bool& isIncrese = true);
	public:
		STRING m_strDBName;//所属数据库名称
		STRING m_strTableName;
		std::vector<STRING> m_vevCols;//所有的列
		std::map<STRING,int32_t> m_mapCols;//<列名称,列索引>
		DynaArray2D<STRING> m_Values;
	};

	/****************数据库信息*******************/
	class DB_SERVICE_API DataBaseInfo
	{
	public:
		DataBaseInfo();
		~DataBaseInfo();
		DataBaseInfo(const DataBaseInfo& rhs);
		DataBaseInfo(DataBaseInfo&& rhs) noexcept;

	public:
		DataBaseInfo& operator=(const DataBaseInfo& rhs);//对自赋值测试
		DataBaseInfo& operator=(DataBaseInfo&& rhs) noexcept;
		TableInfo operator[](const STRING& strTable);

		bool eraseTable(const STRING& strTable);//删除表格
	public:
		STRING m_strDBName;//DBname
		STRING m_strDBfile;//DB文件完整路径
		std::vector<STRING> m_TableNames;
		std::map<STRING, TableInfo> m_tables;//<表格名称,表内容>
	};

	/**************************数据库服务，所有数据库信息*******************************/
	class DB_SERVICE_API DBService
	{
	public:
		DBService(const EnumDBType& enumType);
		~DBService();

	public:
		void startService();
		void stopService();
		void setDataBasePath(const std::string& strPath);
		EnumDBType getDBType() const;//获取数据库类型

		void addListener(IListenner* listen);
		void removeListener(IListenner* listen);

	public:
		//异步操作
		uint32_t readDadaBaseCmd(const STRING& strDBName);//添加一个数据库信息（非创建一个新数据库）
		uint32_t createTableCmd(const STRING& strDBName, const STRING& strTableName, const STRING& strSQL,DynaArray2D<STRING> arrValue);//在数据库中添加一个表格，若包含则覆盖
		uint32_t deleteTableCmd(const STRING& strDBName,const STRING& strTableName);//删除表格
		uint32_t updateTableCmd(const STRING& strDBName, const STRING& strTableName, DynaArray2D<STRING> arrValue);//在数据库中添加一个表格，若包含则覆盖

		//同步操作
		//bool readDadaBaseSyn(const STRING& strDBName);//添加一个数据库信息（非创建一个新数据库）
		bool createTableSyn(const STRING& strDBName, const STRING& strTableName, const STRING& strSQL, DynaArray2D<STRING> arrValue);//在数据库中添加一个表格，若包含则覆盖
		bool deleteTableSyn(const STRING& strDBName, const STRING& strTableName);//删除表格
		bool updateTableSyn(const STRING& strDBName, const STRING& strTableName, DynaArray2D<STRING> arrValue);//在数据库中添加一个表格，若包含则覆盖

	public:
		DataBaseInfo operator[](const STRING& strDBName);

	private:
		void taskTread();
		bool readDadaBaseInner(DBCommand& cmd);	//读取数据库信息
		bool creatTableInner(DBCommand& cmd);	//添加表格
		bool deleteTableInner(DBCommand& cmd);//删除表格
		bool updateTableInner(DBCommand& cmd);//更新表格

	private:
		std::set<IListenner*> m_allListener;//观察者列表
		std::mutex m_lockListen;
		void notifyAll(std::queue<DBResult>& rst);

	private:
		std::mutex m_mutDBTask;//修改m_queueCommand线程锁
		std::mutex m_mutDBRead;//读数据库线程锁
		std::thread m_tread;//数据信息操作线程

		const EnumDBType m_enumDBType;//构造函数中指定后不可修改
		bool m_bRunTread;//线程是否运行
		std::string m_strDBPath;

		static uint32_t m_cmdID ;
		std::queue<DBCommand> m_queueCommand;//需要处理的命令
		std::queue<DBResult> m_queueResult;//执行结果
		std::map<STRING, DataBaseInfo> m_dbInfos;//数据库信息<数据库名称，信息>
		

		std::shared_ptr<DBCommonAPI> m_DBOperator;
		std::shared_ptr<COMMONDBINFO> m_pDBProceInfo;
		LoggerPtr m_Logger;
	};

}