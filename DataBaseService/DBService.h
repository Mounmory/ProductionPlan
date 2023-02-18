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
		DATABASE_READ_DB = 0,//���
		DATABASE_CREATE_DB,
		DATABASE_UPTATE_DB,
		DATABASE_DELETE_DB,
		DATABASE_CRATE_TABLE,//���
		DATABASE_UPTATE_TABLE,
		DATABASE_DELETE_TABLE//���
	};

	struct DBCommand
	{
		uint32_t cmdID ;
		DBCommandType cmdType;
		STRING strDBName;
		STRING strTableName;
		STRING strSQL;//SQL���
		DynaArray2D<STRING> arrValue;//
	};

	struct DBResult
	{
		uint32_t cmdID;
		DBCommandType cmdType;
		STRING strDBName;
		STRING strTableName;
		bool bResult;//ִ�н��
	};
	/***********������࣬�������ݿ����ִ�н��*********/
	class DB_SERVICE_API IListenner //��Ϊ�ͻ�����ĳ�����࣬���ù۲������ģʽ�������첽������
	{
	public:
		IListenner() = default;
		virtual ~IListenner() = default;
	public:
		virtual void dealDBResult(std::queue<DBResult>& rst) = 0;

	};

	/*****************���ݿ�����Ϣ********************/
	class DB_SERVICE_API TableInfo//�����Ϣ�࣬���ڱ������ݿ��б����Ϣ
	{
	public:
		TableInfo();
		~TableInfo();
		TableInfo(const TableInfo& rhs);
		TableInfo(TableInfo&& ths) noexcept;
	public:
		TableInfo& operator=(const TableInfo& rhs);
		TableInfo& operator=(TableInfo&& rhs) noexcept;

		//������
		void setCols(const std::vector<STRING>& cols);
		void oderByCols(const STRING& colName,const bool& isIncrese = true);
	public:
		STRING m_strDBName;//�������ݿ�����
		STRING m_strTableName;
		std::vector<STRING> m_vevCols;//���е���
		std::map<STRING,int32_t> m_mapCols;//<������,������>
		DynaArray2D<STRING> m_Values;
	};

	/****************���ݿ���Ϣ*******************/
	class DB_SERVICE_API DataBaseInfo
	{
	public:
		DataBaseInfo();
		~DataBaseInfo();
		DataBaseInfo(const DataBaseInfo& rhs);
		DataBaseInfo(DataBaseInfo&& rhs) noexcept;

	public:
		DataBaseInfo& operator=(const DataBaseInfo& rhs);//���Ը�ֵ����
		DataBaseInfo& operator=(DataBaseInfo&& rhs) noexcept;
		TableInfo operator[](const STRING& strTable);

		bool eraseTable(const STRING& strTable);//ɾ�����
	public:
		STRING m_strDBName;//DBname
		STRING m_strDBfile;//DB�ļ�����·��
		std::vector<STRING> m_TableNames;
		std::map<STRING, TableInfo> m_tables;//<�������,������>
	};

	/**************************���ݿ�����������ݿ���Ϣ*******************************/
	class DB_SERVICE_API DBService
	{
	public:
		DBService(const EnumDBType& enumType);
		~DBService();

	public:
		void startService();
		void stopService();
		void setDataBasePath(const std::string& strPath);
		EnumDBType getDBType() const;//��ȡ���ݿ�����

		void addListener(IListenner* listen);
		void removeListener(IListenner* listen);

	public:
		//�첽����
		uint32_t readDadaBaseCmd(const STRING& strDBName);//���һ�����ݿ���Ϣ���Ǵ���һ�������ݿ⣩
		uint32_t createTableCmd(const STRING& strDBName, const STRING& strTableName, const STRING& strSQL,DynaArray2D<STRING> arrValue);//�����ݿ������һ������������򸲸�
		uint32_t deleteTableCmd(const STRING& strDBName,const STRING& strTableName);//ɾ�����
		uint32_t updateTableCmd(const STRING& strDBName, const STRING& strTableName, DynaArray2D<STRING> arrValue);//�����ݿ������һ������������򸲸�

		//ͬ������
		//bool readDadaBaseSyn(const STRING& strDBName);//���һ�����ݿ���Ϣ���Ǵ���һ�������ݿ⣩
		bool createTableSyn(const STRING& strDBName, const STRING& strTableName, const STRING& strSQL, DynaArray2D<STRING> arrValue);//�����ݿ������һ������������򸲸�
		bool deleteTableSyn(const STRING& strDBName, const STRING& strTableName);//ɾ�����
		bool updateTableSyn(const STRING& strDBName, const STRING& strTableName, DynaArray2D<STRING> arrValue);//�����ݿ������һ������������򸲸�

	public:
		DataBaseInfo operator[](const STRING& strDBName);

	private:
		void taskTread();
		bool readDadaBaseInner(DBCommand& cmd);	//��ȡ���ݿ���Ϣ
		bool creatTableInner(DBCommand& cmd);	//��ӱ��
		bool deleteTableInner(DBCommand& cmd);//ɾ�����
		bool updateTableInner(DBCommand& cmd);//���±��

	private:
		std::set<IListenner*> m_allListener;//�۲����б�
		std::mutex m_lockListen;
		void notifyAll(std::queue<DBResult>& rst);

	private:
		std::mutex m_mutDBTask;//�޸�m_queueCommand�߳���
		std::mutex m_mutDBRead;//�����ݿ��߳���
		std::thread m_tread;//������Ϣ�����߳�

		const EnumDBType m_enumDBType;//���캯����ָ���󲻿��޸�
		bool m_bRunTread;//�߳��Ƿ�����
		std::string m_strDBPath;

		static uint32_t m_cmdID ;
		std::queue<DBCommand> m_queueCommand;//��Ҫ���������
		std::queue<DBResult> m_queueResult;//ִ�н��
		std::map<STRING, DataBaseInfo> m_dbInfos;//���ݿ���Ϣ<���ݿ����ƣ���Ϣ>
		

		std::shared_ptr<DBCommonAPI> m_DBOperator;
		std::shared_ptr<COMMONDBINFO> m_pDBProceInfo;
		LoggerPtr m_Logger;
	};

}