#include "stdafx.h"
#include "DBService.h"

//���ݿ��¼����
#ifndef DATABASE_LOG_CONTROL
#define DATABASE_LOG_CONTROL(bFlag,connInfo,dataInfo)					\
if (!bFlag)																\
{																		\
	if (!connInfo->CheckConnecInfo(&dataInfo))							\
	{																	\
		m_Logger->LogError("[%s][%d] CheckConnecInfo error!",			\
			__STRING_FUNCTION__,__FUNCTION__);							\
		return false;															\
	}																	\
	if (!connInfo->OnInitADOConn(&dataInfo))							\
	{																	\
		m_Logger->LogError("[%s][%d] OnInitADOConn error!",				\
			__STRING_FUNCTION__,__FUNCTION__);							\
		return false;															\
	}																	\
}																	
#endif // !DATABASE_LOG_CONTROL

//���ݿ��˳�����
#ifndef DATABASE_EXIT_CONTROL
#define DATABASE_EXIT_CONTROL(bFlag,connInfo)	\
if (!bFlag)										\
{												\
	connInfo->ExitConnect();					\
}
#endif

namespace DataBase {
	//��ȡ���ݿ�����
	std::shared_ptr<DBCommonAPI>  getDataBaseConnect(const EnumDBType& dbType)
	{
		std::shared_ptr<DBCommonAPI> dbPtr = nullptr;
		switch (dbType)
		{
		case EnumDBType::DATA_SOURCE_ACCESS:
			dbPtr = (std::make_shared<DBAccessConn>());
			break;
		case EnumDBType::DATA_SOURCE_SQLITE:
			dbPtr = std::make_shared<DBSqliteConn>();
		default:
			break;
		}
		return dbPtr;
	}

	//��ȡDB�ļ���չ��
	inline STRING getDBExternName(const EnumDBType& dbType)
	{
		STRING strExter;
		switch (dbType)
		{
		case EnumDBType::DATA_SOURCE_ACCESS:
			strExter = ".mdb";
			break;
		case EnumDBType::DATA_SOURCE_SQLITE:
			strExter = ".db";
			break;
		default:
			strExter = "";
			break;
		}
		return strExter;
	}

	/*****************���ݿ�����Ϣ********************/
	TableInfo::TableInfo() 
		: m_strDBName("")
		, m_strTableName("")
	{
	}


	TableInfo::~TableInfo()
	{
	}


	TableInfo::TableInfo(const TableInfo& rhs)
	{
		*this = rhs;
	}

	TableInfo::TableInfo(TableInfo&& rhs) noexcept
		:m_strDBName(std::exchange(rhs.m_strDBName, ""))
		, m_strTableName(std::exchange(rhs.m_strTableName, ""))
		, m_vevCols(std::move(rhs.m_vevCols))
		, m_mapCols(std::move(rhs.m_mapCols))
		, m_Values(std::move(rhs.m_Values))
	{
	}

	TableInfo& TableInfo::operator=(const TableInfo& rhs)
	{
		m_strDBName = rhs.m_strDBName;
		m_strTableName = rhs.m_strTableName;
		m_vevCols = rhs.m_vevCols;
		m_mapCols = rhs.m_mapCols;
		m_Values = rhs.m_Values;

		return *this;
	}

	TableInfo& TableInfo::operator=(TableInfo&& rhs) noexcept
	{
		m_strDBName = std::exchange(rhs.m_strDBName, "");
		m_strTableName = std::exchange(rhs.m_strTableName, "");
		m_vevCols = std::move(rhs.m_vevCols);
		m_mapCols = std::move(rhs.m_mapCols);
		m_Values = std::move(rhs.m_Values);
		return *this;
	}

	void TableInfo::setCols(const std::vector<STRING>& cols) 
	{
		m_vevCols = cols;
		for (size_t index = 0 ;index < m_vevCols.size(); ++index)
		{
			m_mapCols.insert(std::make_pair(m_vevCols[index], index));
		}
	}

	//����ĳ������
	void TableInfo::oderByCols(const STRING& colName, const bool& isIncrese)
	{
		auto index = m_mapCols.find(colName);
		if (index == m_mapCols.end())
		{
			LOGERROR("[%s][%d] column name (%s) is not exist in table %s db %s",__STRING_FUNCTION__,__LINE__, colName,m_strTableName,m_strDBName);
			throw std::out_of_range("column Name:"+ colName +" is not exist!");
		}
	}

	/****************���ݿ���Ϣ*******************/
	DataBaseInfo::DataBaseInfo()
		: m_strDBName(""),
		m_strDBfile("")
	{
	}

	DataBaseInfo::~DataBaseInfo()
	{
	}

	DataBaseInfo::DataBaseInfo(const DataBaseInfo& rhs)
		: m_strDBName("")
		, m_strDBfile("")
	{
		*this = rhs;
	}


	DataBaseInfo::DataBaseInfo(DataBaseInfo&& rhs) noexcept
		: m_strDBName(std::exchange(rhs.m_strDBName, ""))
		, m_strDBfile(std::exchange(rhs.m_strDBfile, ""))
		, m_TableNames(std::move(rhs.m_TableNames))
		, m_tables(std::move(rhs.m_tables))
	{
	}

	DataBaseInfo& DataBaseInfo::operator = (const DataBaseInfo& rhs)
	{
		m_strDBName = rhs.m_strDBName;
		m_strDBfile = rhs.m_strDBfile;
		m_TableNames = rhs.m_TableNames;
		m_tables = rhs.m_tables;
		return *this;
	}

	DataBaseInfo& DataBaseInfo::operator = (DataBaseInfo&& rhs) noexcept
	{
		m_strDBName = std::exchange(rhs.m_strDBName, "");
		m_strDBfile = std::exchange(rhs.m_strDBfile, "");
		m_TableNames = std::move(rhs.m_TableNames);
		m_tables = std::move(rhs.m_tables);
		return *this;
	}

	TableInfo DataBaseInfo::operator[](const STRING& strTable)
	{
		auto iterTable = m_tables.find(strTable);
		if (iterTable == m_tables.end())
		{
			return TableInfo();//���ؿյ�
		}
		else
		{
			return iterTable->second;
		}
	}

	bool DataBaseInfo::eraseTable(const STRING& strTable) 
	{
		auto iterTable = m_tables.find(strTable);
		if (iterTable != m_tables.end())
		{
			m_tables.erase(iterTable);//ɾ�����
			auto iterTableName = m_TableNames.begin();
			while (iterTableName != m_TableNames.end())
			{
				if (strTable == *iterTableName)
				{
					m_TableNames.erase(iterTableName);
					break;
				}
				else
				{
					++iterTableName;
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	/**************************���ݿ�����������ݿ���Ϣ*******************************/
	uint32_t DBService::m_cmdID = 0;

	DBService::DBService(const EnumDBType& enumType) :
		m_Logger(Logger::getLogger()),
		m_enumDBType(enumType),
		m_bRunTread(false),
		m_DBOperator(getDataBaseConnect(enumType)),
		m_pDBProceInfo(std::make_shared<ADODBINFO>())
	{
		if (nullptr == m_DBOperator)
		{
			m_Logger->LogError("m_DBOperator is nullptr!");
			throw std::runtime_error("nullptr in DBService construct!");
		}
		m_Logger->LogInfo("[%s][%d] DB Service begin!", __STRING_FUNCTION__, __LINE__);
		//startService();
	}

	DBService::~DBService()
	{
		//stopService();
		m_Logger->LogInfo("[%s][%d] DB Service end!", __STRING_FUNCTION__, __LINE__);
	}

	void DBService::startService()
	{
		if (false == m_bRunTread)
		{
			m_bRunTread = true;
			//�����߳�
			m_tread = std::thread(&DBService::taskTread, this);//�����߳�
		}
		return;
	}

	void DBService::stopService()
	{
		if (true == m_bRunTread)
		{
			m_bRunTread = false;//�˳��߳�
			m_tread.join();//�ȴ��߳̽���
			m_dbInfos.clear();
		}
	}

	void DBService::setDataBasePath(const std::string& strPath)
	{
		m_strDBPath = strPath;
	}

	EnumDBType DBService::getDBType() const
	{
		return m_enumDBType;
	}

	void DBService::addListener(IListenner* listen)
	{
		std::lock_guard<std::mutex> lock(m_lockListen);
		m_allListener.insert(listen);
	}

	void DBService::removeListener(IListenner* listen)
	{
		std::lock_guard<std::mutex> lock(m_lockListen);
		m_allListener.erase(listen);
	}

	uint32_t DBService::readDadaBaseCmd(const STRING& strDBName)
	{
		DBCommand temCmd; 
		temCmd.cmdID = ++m_cmdID;
		temCmd.cmdType = DBCommandType::DATABASE_READ_DB;
		temCmd.strDBName = strDBName;

		std::lock_guard<std::mutex> lockTread(m_mutDBTask);
		m_queueCommand.push(std::move(temCmd));
		return temCmd.cmdID;
	}

	uint32_t DBService::createTableCmd(const STRING& strDBName, const STRING& strTableName, const STRING& strSQL, DynaArray2D<STRING> arrValue)
	{
		DBCommand temCmd;
		temCmd.cmdID = ++m_cmdID;
		temCmd.cmdType = DBCommandType::DATABASE_CRATE_TABLE;
		temCmd.strDBName = strDBName;
		temCmd.strTableName = strTableName;
		temCmd.strSQL = strSQL;
		temCmd.arrValue = std::move(arrValue);

		std::lock_guard<std::mutex> lockTread(m_mutDBTask);
		m_queueCommand.push(std::move(temCmd));
		return temCmd.cmdID;
	}

	uint32_t DBService::deleteTableCmd(const STRING& strDBName, const STRING& strTableName) 
	{
		DBCommand temCmd;
		temCmd.cmdID = ++m_cmdID;
		temCmd.cmdType = DBCommandType::DATABASE_DELETE_TABLE;
		temCmd.strDBName = strDBName;
		temCmd.strTableName = strTableName;

		std::lock_guard<std::mutex> lockTread(m_mutDBTask);
		m_queueCommand.push(std::move(temCmd));
		return temCmd.cmdID;
	}

	uint32_t DBService::updateTableCmd(const STRING& strDBName, const STRING& strTableName, DynaArray2D<STRING> arrValue)
	{
		DBCommand temCmd;
		temCmd.cmdID = ++m_cmdID;
		temCmd.cmdType = DBCommandType::DATABASE_UPTATE_TABLE;
		temCmd.strDBName = strDBName;
		temCmd.strTableName = strTableName;
		temCmd.arrValue = std::move(arrValue);

		std::lock_guard<std::mutex> lockTread(m_mutDBTask);
		m_queueCommand.push(std::move(temCmd));
		return temCmd.cmdID;
	}

	//ͬ������
	//bool DBService::readDadaBaseSyn(const STRING& strDBName)	
	//{
	//	return true;
	//}

	bool DBService::createTableSyn(const STRING& strDBName, const STRING& strTableName, const STRING& strSQL, DynaArray2D<STRING> arrValue) 
	{
		DBCommand temCmd;
		//temCmd.cmdID = ++m_cmdID;
		//temCmd.cmdType = DBCommandType::DATABASE_CRATE_TABLE;
		temCmd.strDBName = strDBName;
		temCmd.strTableName = strTableName;
		temCmd.strSQL = strSQL;
		temCmd.arrValue = std::move(arrValue);

		std::lock_guard<std::mutex> lockTread(m_mutDBRead);
		bool ret = creatTableInner(temCmd);
		return ret;
	}

	bool DBService::deleteTableSyn(const STRING& strDBName, const STRING& strTableName) 
	{
		DBCommand temCmd;
		//temCmd.cmdID = ++m_cmdID;
		//temCmd.cmdType = DBCommandType::DATABASE_DELETE_TABLE;
		temCmd.strDBName = strDBName;
		temCmd.strTableName = strTableName;

		std::lock_guard<std::mutex> lockTread(m_mutDBRead);
		bool ret = deleteTableInner(temCmd);
		return ret;
	}

	bool DBService::updateTableSyn(const STRING& strDBName, const STRING& strTableName, DynaArray2D<STRING> arrValue) 
	{
		DBCommand temCmd;
		//temCmd.cmdID = ++m_cmdID;
		//temCmd.cmdType = DBCommandType::DATABASE_UPTATE_TABLE;
		temCmd.strDBName = strDBName;
		temCmd.strTableName = strTableName;
		temCmd.arrValue = std::move(arrValue);
		
		std::lock_guard<std::mutex> lockTread(m_mutDBRead);
		bool ret = updateTableInner(temCmd);
		return ret;
	}

	void DBService::taskTread()
	{
		std::queue<DBCommand> temDealCommand;//��Ҫ���������
		m_Logger->LogInfo("[%s][%d] begin!", __STRING_FUNCTION__, __LINE__);
		while (m_bRunTread)
		{
			//m_Logger->LogDebug("[%s][%d][%d] is running!", __STRING_FUNCTION__, __LINE__, m_tread.get_id());
			{
				std::lock_guard<std::mutex> lockTread(m_mutDBTask);
				temDealCommand.swap(m_queueCommand);
			}
			if (0 == temDealCommand.size())
			{
				Sleep(10);
				continue;
			}
			//else
			std::queue<DBResult> allRst;
			{
				m_Logger->LogInfo("[%s][%d] receive task,task number is %d!", __STRING_FUNCTION__, __LINE__, temDealCommand.size());
				//m_mutDBRead.lock();
				
				std::lock_guard<std::mutex> lockTread(m_mutDBRead);
				while (temDealCommand.size() > 0)
				{
					bool bRst = false;
					auto iterCmd = temDealCommand.front();
					switch (iterCmd.cmdType)
					{
					case DBCommandType::DATABASE_READ_DB:
						bRst = readDadaBaseInner(iterCmd);
						break;
					case DBCommandType::DATABASE_CRATE_TABLE:
						bRst = creatTableInner(iterCmd);
						break;
					case DBCommandType::DATABASE_DELETE_TABLE:
						bRst = deleteTableInner(iterCmd);
						break;
					case DBCommandType::DATABASE_UPTATE_TABLE:
						bRst = updateTableInner(iterCmd);
						break;
					default:
						break;
					}
					m_Logger->LogDebug("[%s][%d] deal with command cmdID(%ld) end,type(%d),DBname(%s),TableName(%s)!", __STRING_FUNCTION__, __LINE__,
						iterCmd.cmdID, iterCmd.cmdType, iterCmd.strDBName, iterCmd.strTableName);
					
					DBResult tmpRst = { iterCmd.cmdID ,iterCmd.cmdType , iterCmd.strDBName,iterCmd.strTableName, bRst };
					allRst.push(tmpRst);//��ȡִ�н��

					temDealCommand.pop();
				}
				//m_mutDBRead.unlock();
			}
			notifyAll(allRst);
		}
		m_Logger->LogInfo("[%s][%d] end!", __STRING_FUNCTION__, __LINE__);
		return;
	}



	DataBaseInfo DBService::operator[](const STRING& strDBName)
	{
		std::lock_guard<std::mutex> lockTread(m_mutDBRead);
		auto iterDB = m_dbInfos.find(strDBName);
		if (iterDB != m_dbInfos.end())
		{
			return iterDB->second;
		}
		else
		{
			return DataBaseInfo();
		}
	}

	bool DBService::readDadaBaseInner(DBCommand& cmd)
	{
		m_Logger->LogDebug("[%s][%d] read DataBase(%s) begin!", __STRING_FUNCTION__, __LINE__, cmd.strDBName);
		DataBaseInfo temDBInfo;//DB�����е�һ��DB��Ϣ
		temDBInfo.m_strDBName = cmd.strDBName;
		temDBInfo.m_strDBfile = STRING(m_strDBPath.c_str()) + "\\" + cmd.strDBName + getDBExternName(m_enumDBType);

		bool bLogFlag = FALSE;//�ӿں���ִ�й����в��Զ���¼���˳�,trueÿ�β�����ִ�е�¼���˳���false�ֶ����Ƶ�¼���˳�
		m_pDBProceInfo->stuLogInfo.strDataSource = temDBInfo.m_strDBfile;//���ݿ��ļ�

		DATABASE_LOG_CONTROL(bLogFlag, m_DBOperator, m_pDBProceInfo->stuLogInfo);
		//��ȡ��ѯ�����Ϣ
		m_DBOperator->GetTableNameFromDataBase(m_pDBProceInfo, bLogFlag);
		temDBInfo.m_TableNames = m_pDBProceInfo->stuDBOperate.strOutQueryTableResult;//���б������

		for (auto iterSetTables : temDBInfo.m_TableNames)//�������������Ϣ����ѯ���������
		{
			//��ȡ����������
			m_pDBProceInfo->stuDBOperate.strInQueryedColTableName = iterSetTables;
			m_DBOperator->GetColumnNameFromTable(m_pDBProceInfo, bLogFlag);//��ȡ������
			
			//��ѯ���������Ϣ
			m_pDBProceInfo->stuValueInfo.strInReadSQL = "select* from " + iterSetTables;
			m_pDBProceInfo->stuValueInfo.szInReadColumnName = m_pDBProceInfo->stuDBOperate.strOutQueryColumnResult;
			m_DBOperator->GetDataFromDataBase(m_pDBProceInfo, bLogFlag);
			//��ȡTableInfoֵ
			TableInfo temTableInfo;
			temTableInfo.m_strDBName = temDBInfo.m_strDBName;
			temTableInfo.m_strTableName = iterSetTables;//�������
			temTableInfo.m_Values = std::move(m_pDBProceInfo->stuValueInfo.szOutReadValue);
			temTableInfo.m_vevCols = std::move(m_pDBProceInfo->stuDBOperate.strOutQueryColumnResult);//����������
			temTableInfo.setCols(temTableInfo.m_vevCols);

			temDBInfo.m_tables.insert(std::make_pair(iterSetTables, std::move(temTableInfo)));//
		}
		DATABASE_EXIT_CONTROL(bLogFlag, m_DBOperator);
		m_dbInfos.insert(std::make_pair(cmd.strDBName, temDBInfo));
		m_Logger->LogDebug("[%s][%d] read DataBase(%s) end!", __STRING_FUNCTION__, __LINE__, cmd.strDBName);
		return true;
	}

	bool DBService::creatTableInner(DBCommand& cmd) 
	{
		m_Logger->LogDebug("[%s][%d] creat table(%s) from DataBase(%s) begin!", __STRING_FUNCTION__, __LINE__, cmd.strTableName, cmd.strDBName);
		LOGDEBUG_BYSTREAM(cmd.arrValue)
		bool bLogFlag = FALSE;//�ӿں���ִ�й����в��Զ���¼���˳�,trueÿ�β�����ִ�е�¼���˳���false�ֶ����Ƶ�¼���˳�
		m_pDBProceInfo->stuLogInfo.strDataSource = STRING(m_strDBPath.c_str()) + "\\" + cmd.strDBName + getDBExternName(m_enumDBType);//���ݿ��ļ�

		//��¼���ݿ�
		DATABASE_LOG_CONTROL(bLogFlag, m_DBOperator, m_pDBProceInfo->stuLogInfo);

		//������Ƿ���ڣ�������ɾ��
		auto iterDB = m_dbInfos.find(cmd.strDBName);
		if (iterDB != m_dbInfos.end())//�Ѿ���ȡ��������ݿ�
		{
			if (iterDB->second.m_tables.find(cmd.strTableName) != iterDB->second.m_tables.end())//����Ѵ���,ֱ��ɾ��
			{
				//ɾ�����ݿ�
				m_pDBProceInfo->stuDBOperate.strInDelTableName = cmd.strTableName;
				if (!m_DBOperator->DeleteTableFromDataBase(m_pDBProceInfo, bLogFlag))
				{
					m_Logger->LogError("[%s][%d] Delete table(%s) from DataBase(%s) failed!", __STRING_FUNCTION__, __LINE__, cmd.strTableName, cmd.strDBName);
					return false;
				}
				//ɾ���ڴ�
				iterDB->second.eraseTable(cmd.strTableName);//����ֵ�⣬���������Ϣ��Ӧ�øı�
			}
		}
		else//ֱ�Ӳ�ѯ���ݿ⣬�ݲ�ʵ��
		{
			return false;
		}
		//�����Ϣ

		//�����±��
		m_pDBProceInfo->stuDBOperate.ClearCreatInfo();
		m_pDBProceInfo->stuDBOperate.EnuCreatType = EnumDBCreatType::CREAT_TABLE;//�������
		m_pDBProceInfo->stuDBOperate.strInCreatTableSQL = cmd.strSQL;
		m_DBOperator->CreatDataBase(m_pDBProceInfo, bLogFlag);
		//��ȡ������
		m_pDBProceInfo->stuDBOperate.strInQueryedColTableName = cmd.strTableName;
		m_DBOperator->GetColumnNameFromTable(m_pDBProceInfo, bLogFlag);//��ȡ������
		//д���
		m_pDBProceInfo->stuValueInfo.szInWtriteColuName = std::move(m_pDBProceInfo->stuDBOperate.strOutQueryColumnResult);//����Ϣ
		m_pDBProceInfo->stuValueInfo.strInWriteSQL = "select* from "+ cmd.strTableName;
		m_pDBProceInfo->stuValueInfo.szInWriteValue = std::move(cmd.arrValue);
		if (!m_DBOperator->AddDataToDataBase(m_pDBProceInfo, bLogFlag))
		{
			m_Logger->LogError("[%s][%d] add values to DataBase failed!",__STRING_FUNCTION__,__LINE__);
			return false;
		}
		//�˳���¼
		DATABASE_EXIT_CONTROL(bLogFlag, m_DBOperator);
		//�����ڴ�����
		TableInfo temTableInfo;
		temTableInfo.m_strDBName = cmd.strDBName;
		temTableInfo.m_strTableName = cmd.strTableName;//�������
		temTableInfo.m_vevCols = std::move(m_pDBProceInfo->stuValueInfo.szInWtriteColuName);//����������
		temTableInfo.setCols(temTableInfo.m_vevCols);
		//temTableInfo.m_Values = std::move(cmd.arrValue);
		temTableInfo.m_Values = std::move(m_pDBProceInfo->stuValueInfo.szInWriteValue);
		m_dbInfos[cmd.strDBName].m_tables.insert(std::make_pair(cmd.strTableName, std::move(temTableInfo)));
		m_dbInfos[cmd.strDBName].m_TableNames.emplace_back(cmd.strTableName);
		m_Logger->LogDebug("[%s][%d] creat table(%s) from DataBase(%s) end!", __STRING_FUNCTION__, __LINE__, cmd.strTableName, cmd.strDBName);
		return true;
	}

	bool DBService::deleteTableInner(DBCommand& cmd) 
	{
		m_Logger->LogDebug("[%s][%d] Delete table(%s) from DataBase(%s) begin!", __STRING_FUNCTION__, __LINE__, cmd.strTableName, cmd.strDBName);
		m_pDBProceInfo->stuLogInfo.strDataSource = STRING(m_strDBPath.c_str()) + "\\" + cmd.strDBName + getDBExternName(m_enumDBType);;//���ݿ��ļ�
		//ɾ�����ݿ�
		m_pDBProceInfo->stuDBOperate.strInDelTableName = cmd.strTableName;
		if (!m_DBOperator->DeleteTableFromDataBase(m_pDBProceInfo))
		{
			m_Logger->LogError("[%s][%d] Delete table(%s) from DataBase(%s) failed!", __STRING_FUNCTION__, __LINE__, cmd.strTableName, cmd.strDBName);
			return false;
		}
		m_Logger->LogDebug("[%s][%d] Delete table(%s) from DataBase(%s) end!", __STRING_FUNCTION__, __LINE__, cmd.strTableName, cmd.strDBName);
		//ɾ���ڴ�
		bool ret = m_dbInfos[cmd.strDBName].eraseTable(cmd.strTableName);//ɾ�����
		return ret;
	}

	bool DBService::updateTableInner(DBCommand& cmd)
	{
		m_Logger->LogDebug("[%s][%d] Update table(%s) from DataBase(%s) begin!", __STRING_FUNCTION__, __LINE__, cmd.strTableName, cmd.strDBName);
		LOGDEBUG_BYSTREAM(cmd.arrValue)
		bool bLogFlag = FALSE;//�ӿں���ִ�й����в��Զ���¼���˳�,trueÿ�β�����ִ�е�¼���˳���false�ֶ����Ƶ�¼���˳�
		m_pDBProceInfo->stuLogInfo.strDataSource = STRING(m_strDBPath.c_str()) + "\\" + cmd.strDBName + getDBExternName(m_enumDBType);//���ݿ��ļ�

		//��¼���ݿ�
		DATABASE_LOG_CONTROL(bLogFlag, m_DBOperator, m_pDBProceInfo->stuLogInfo);
		//��ձ�����
		m_pDBProceInfo->stuValueInfo.strInDelTableName = cmd.strTableName;
		if (!m_DBOperator->DeleteAllDataFromTable(m_pDBProceInfo, bLogFlag))
		{
			m_Logger->LogError("[%s][%d] Delete data from table (%s) DataBase(%s) failed!", __STRING_FUNCTION__, __LINE__, cmd.strTableName, cmd.strDBName);
			return false;
		}
		//��ӱ�����
		m_pDBProceInfo->stuValueInfo.szInWtriteColuName = m_dbInfos[cmd.strDBName][cmd.strTableName].m_vevCols;//����Ϣ
		m_pDBProceInfo->stuValueInfo.strInWriteSQL = "select* from " + cmd.strTableName;
		m_pDBProceInfo->stuValueInfo.szInWriteValue = std::move(cmd.arrValue);
		if (!m_DBOperator->AddDataToDataBase(m_pDBProceInfo, bLogFlag))
		{
			m_Logger->LogError("[%s][%d] add values to DataBase failed!", __STRING_FUNCTION__, __LINE__);
			return false;
		}
		//�رյ�¼
		DATABASE_EXIT_CONTROL(bLogFlag, m_DBOperator);

		//���±��ֵ
		m_dbInfos[cmd.strDBName][cmd.strTableName].m_Values = std::move(m_pDBProceInfo->stuValueInfo.szInWriteValue);
		m_Logger->LogDebug("[%s][%d] Update table(%s) from DataBase(%s) end!", __STRING_FUNCTION__, __LINE__, cmd.strTableName, cmd.strDBName);
		return true;
	}

	void DBService::notifyAll(std::queue<DBResult>& rst)
	{
		std::lock_guard<std::mutex> lock(m_lockListen);
		for (auto iterListen : m_allListener)
		{
			iterListen->dealDBResult(rst);
		}
	}
}