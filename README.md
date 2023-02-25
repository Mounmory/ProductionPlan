# 生产计划管理系统简介

## 0.前言

**开发环境：Win10 + Visual Studio 2015 + Microsoft Access (或Microsoft Access引擎)**
> 注意：由于源码不断更新，本文档与源码不一致部分，以源码为准！若编译或启动失败，看《ACCESS引擎相关》文件夹安装引擎

**生产计划管理系统**基于通过MFC编程对Creo二次开发，读取三维模型结构，自动生成物料明细表，实现工位管理、工艺流程管理及生产计划排列，主要实现功能如下：
 
- **工位查看及管理** ：设置工位信息，包括工位显示、修改；查看已排列生产计划信息及工位的使用；
- **工艺流程表编制** ：读取Creo模型结构树，通过界面编辑工艺流程并保存到数据库，同时实现同步数据库中已存在物料的工艺流程信息（注：此版本已将程序与Creo剥离，为直接读取已保存的物料明细信息，可修改工艺流程）；
- **排列生产计划** ：根据编制好的工艺流程表信息及工位信息，设置时间后读取可用工位信息，按照一定原则，进行生产计划排列，对排列结果评价，给出最优排列。

## 1.修改记录

- 1.1完成基本功能和算法实现。
    - 演示视频：https://www.bilibili.com/video/BV1Fh411o78X/?spm_id_from=333.999.0.0
- 1.2封装数据库连接类，封装动态数组类，优化一些代码。
	- 演示视频：https://www.bilibili.com/video/BV1xR4y1A7tt/?spm_id_from=333.999.0.0
- 1.3封装数据库服务模块、使用线程池技术大幅缩短生产计划排列效率、封装生产计划排列算法，优化一些代码。


## 2.文件夹说明
>  这里介绍项目基本文件分布及主要功能，详细内容见源码介绍

- 2.1 ProductionPlan/BaseForProTookit/（基本服务模块）
	- DynaArray.h(cpp)/DynaArrayEx.h(cpp)：封装动态数组类
	- Logger.h(cpp):打印日志文件类
	- threadpool.h(cpp):线程池管理类
	
- 2.2 ProductionPlan/DataBaseService/（封装的数据库服务）
	- DataType.h:数据库访问数据类型封装
	- DBCommonAPI.h：抽象基类，数据库连接公共接口
	- DBAccessConn.h(cpp):继承抽象基类，封装Microsoft ACCESS数据库连接类 
	- DBSqliteConn.h(cpp):继承抽象基类，封装SQlite数据库连接类 
	- DBService.h(cpp):对外部提供方便使用的数据库服务（包括同步和异步两种方式），使用方法详见源码介绍中实例
	
- 2.3 ProductionPlan/ProductionPlanCalculateLib/
	- ProductionPlanCalculate.h:封装生产计划排列算法，源码暂不公开
	- ProductionPlanCalculateLib.lib(dll)：编译(运行)依赖

- 2.4 ProductionPlan/CommonFunctionFile/（可忽略）
	提炼出一些公共函数及依赖的sqlite库文件
	
- 2.5 ProductionPlan/ProductionPlan/（提供MFC对话框程序界面,也是主要操作界面）
	- TechnolProcePrepareDlg.h(cpp):工艺流程管理对话框类
	- TechnolPlanAndWorkPlaceViewDlg.h(cpp):工位管理及已排列生产计划管理对话框
	- TechnolProductionPlanDlg.h(cpp):根据工位信息及工艺流程信息排列生产计划对话框
	- TechnolProductionPlanResultDlg.h(cpp):生产计划排列的子对话框，计算并显示生产计划排列评价。
	
- 2.6 ProductionPlan/ProductionPlanMenu/（可忽略）
	- 以菜单形式提供ProductionPlan dll中对话框界面。

- 2.7 ProductionPlan/ConsoleTest/（可忽略）
	- 对库文件自测用的控制台程序
	
- 2.8 ProductionPlan/x64/RelWithDebugInfo/
	提供的应用程序ProductionPlanMenu.exe，可对照修改记录中的视频进行操作体验，若加载动态链接失败，请尝试安装ACCESS数据库引擎，见文件夹《ACCESS引擎相关》。

## 3.源码介绍
	 
### 3.1 BaseForProTookit
#### 3.1.1 DynaArray.h(cpp)/DynaArrayEx.h(cpp)
使用模板设计可改变大小的二维数组类，用于生产计划排列过程中，二维矩阵计算，定义移动构造函数及移动赋值运算符来提高性能。
DynaArray.h 头文件中类声明如下：

```c++
template<class _Ty>
class DYNARRAY_API DynaArray2D {
public:
	DynaArray2D();
	DynaArray2D(const size_t nRow, const size_t nCol);
	/*explicit */DynaArray2D(const DynaArray2D<_Ty> &rhs);	//拷贝构造函数
	DynaArray2D(DynaArray2D<_Ty>&& rhs) noexcept;	//移动构造函数
	~DynaArray2D();

	size_t getRowCount() const;	//获取数组行数
	size_t getColCount() const;	//获取数组列数
	void reSizeArray(const size_t nRow, const size_t nCol);	//动态修改数组大小
	void zeroArray();	//数组值清零

	_Ty* operator[](const size_t index);
	const _Ty* operator[] (const size_t index) const;
	
	DynaArray2D<_Ty>& operator=(const _Ty& value);
	DynaArray2D<_Ty>& operator=(const DynaArray2D<_Ty> &rhs);	//拷贝赋值
	DynaArray2D<_Ty>& operator=(DynaArray2D<_Ty>&& rhs) noexcept;	//移动赋值

	template<typename _Ty>
	friend std::ostream& operator<< (std::ostream &os, const DynaArray2D<_Ty>& rhs); //使用流输出数组中内容
	
	bool operator==(const DynaArray2D<_Ty> &rhs);
	bool operator!=(const DynaArray2D<_Ty> &rhs);
private:
	bool isMemoryCopy();
	void destroyArray();
private:
	size_t m_rows;
	size_t m_cols;
	_Ty** m_array;
};
```

使用示例如下：

```c++
	DynaArray2D<int> arrayInt1(2, 2);
	arrayInt1.zeroArray();//数值清零
	std::cout << arrayInt1 << std::endl;//打印数组
	arrayInt1.reSizeArray(3, 4);//重新分配内存，改变大小
	arrayInt1 = 2;//整个数组赋值
	
	DynaArray2D<int> arrayInt2(std::move(arrayInt1));//移动构造函数
	std::cout << arrayInt2 << std::endl;
	arrayInt2[0][0] = 1;//某个元素赋值

	arrayInt1 = std::move(arrayInt2);//移动赋值运算符
	arrayInt1 = std::move(arrayInt1);//自赋值
	//略……
```
DynaArrayEx.h为新类型，与DynaArray.h中类作用相同，私有成员m_array类型为“ _Ty* ”，也就是一维数组通过地址转换作为二维数组用，但在分配较大内存时需要较大的连续空间，分配较大内存空间可能更容易失败，还未进行充分测试，后面可考虑用unique_ptr管理内存。
```c++
template<class _Ty>
class DYNARRAY_API_EX DynaArray2DEx {
public:
	//略
private:
	//略
	_Ty* m_array;//可考虑用unique_ptr管理内存
};
```
#### 3.1.2 Logger.h(cpp)
单例模式，参考第三方库写的一个单例设计模式日志打印系统，以不同级别打印格式化字符流。

```c++
class LOGGER_API Logger
{
private:
	Logger();
	~Logger();
//略
public:
	static Logger* getLogger();//获取日志服务指针
	
	void LogForce(const char *format, ...);
	void LogFatal(const char *format, ...);
	void LogError(const char *format, ...);
	void LogWarn(const char *format, ...);
	void LogInfo(const char *format, ...);
	void LogDebug(const char *format, ...);
//略
private:
std::mutex m_mutLog;//日志文件修改线程锁
static Logger* m_LogerInstance;//静态日志服务实例指针
}
```
函数调用代码示例
```c++
Logger::getLogger()->LogInfo("[%s][%d] DB Service begin!", __STRING_FUNCTION__, __LINE__);
```
日志打印效果
```c++
[20748-16400]2023-02-12 09:10:40:480 [I][DataBase::DBService::DBService][234] DB Service begin!
```
其中20748为进程ID，16400为线程ID，多线程中方便信息检索。除此之外，还定义了可以直接使用文件流输出。

```c++
#define LOGDEBUG_BYSTREAM(logInfo)\
{\
	std::lock_guard<std::mutex> lockLog(Logger::getLogger()->getMutex());\
	if (Logger::getLogger()->getLevel() >= Logger::EnumLogLevel::LOG_DEBUG)\
		Logger::getLogger()->LogByOstream("D") << logInfo <<std::endl;\
}
```
使用
```c++
DynaArray2D<uint16_t> array1(2, 2);
array1 = 5;
LOGDEBUG_BYSTREAM(array1);
```
打印效果
```c++
[17664-7260]2023-02-12 13:49:44:256 [D]DynaArray2D Type:unsigned short	nRow:2	nCol:2
5	5	
5	5	
```
#### 3.1.3 threadpool.h(cpp)
借用了一个开源线程池库。参考：https://github.com/xf-8087/ThreadPool

### 3.2 数据库服务模块
#### 3.2.1 DataType.h
定义了数据库服务数据交互的一些基本数类型
```c++
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

//操作数据库信息（操作某一个表格，小范围）
typedef struct ValueInfo
{
	//读数据库信息
	STRING  strInReadSQL;//SQL语句
	std::vector<STRING> szInReadColumnName;//存储读取的列名称
	DynaArray2D<STRING> szOutReadValue;//读取信息

	//写数据库信息
	//略……

	//更新数据库信息
	//略……

	//删除数据库中内容
	//略……

}VALUEINFO,*PVALUEINFO;

//数据库整体操作信息
typedef struct DBOperInfo
{
	//新建数据库及表格相关信息
	//略……
	
	//删除表格信息
	//略……
	
	//查询整个表格结果
	//略……

	//查询表格中列名称结果
	//略……
}DBOPERATEINFO,*PDBOPERATEINFO;

//数据库信息结构体
typedef struct ADODatabaseInfo //用于数据库交互的所有信息
{
	DBLOGINFO stuLogInfo;		//登录信息
	VALUEINFO stuValueInfo;		//操作数据库信息
	DBOPERATEINFO stuDBOperate;	//数据库整体操作信息

}COMMONDBINFO,ADODBINFO,*PADODBINFO,SQLITEDBINFO,*PSQLITEDBINFO;
```

#### 3.2.2 DBCommonAPI.h
数据库连接抽象基类，定义数据库连接类接口

```c++
class DB_BASE_FUNC_API DBCommonAPI {
public:
	DBCommonAPI() = default;
	virtual ~DBCommonAPI() = default;
public:
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
```
#### 3.2.3 DBAccessConn.h(cpp)/DBSqliteConn.h(cpp)
目前定义两个数据库连接派生类，ACEESS及SQlite，在本项目中使用了ACCESS数据库，理论上可在数据库服务中指定数据库类型为SQlite，（SQlite还行进行适配测试、修改）。
文件DBAccessConn.h中ACCESS数据库连接类定义如下，具体实现可查看源文件

```c++
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
```
#### 3.2.4DBService.h(cpp)
在本项目中，外部对于数据库操作可直接依赖数据库服务类class DBService，支持同步、异步数据库操作，支持数据库内容缓存，提高访问速度。
异步操作创建独立线程进行数据库操作，客户端使用时调用异步接口，以命令码形式将命令传递给操作线程，操作结束后，将操作结果返回给客户端，数据库服务使用示例见后面生产计划管理模块介绍。

头文件定义示意如下：
```c++
namespace DataBase
{
	enum class DBCommandType	//异步操作命令码
	{
		//详见DBService.h文件……
	};

	struct DBCommand	//异步命令数据结构
	{
		//详见DBService.h文件……
	};

	struct DBResult		//异步命令执行结果数据结构
	{
		//详见DBService.h文件……
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
	class DB_SERVICE_API TableInfo	//表格信息类，用于保存数据库中表格信息
	{
	public:
		TableInfo();
		~TableInfo();
		TableInfo(const TableInfo& rhs);
		TableInfo(TableInfo&& ths) noexcept;//定义移动构造函数和移动赋值运算符，提升拷贝效率
	public:
		TableInfo& operator=(const TableInfo& rhs);
		TableInfo& operator=(TableInfo&& rhs) noexcept;

		//其他略，详见DBService.h文件……
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
		TableInfo operator[](const STRING& strTable);//重载操作符，可直接通过表格名称读取表格信息
		bool eraseTable(const STRING& strTable);//删除表格
		
		//略，详见DBService.h文件……
	};

	/**************************数据库服务，所有数据库信息*******************************/
	class DB_SERVICE_API DBService
	{
	public:
		DBService(const EnumDBType& enumType);
		~DBService();

	public:
		void startService();	//启动数据库服务
		void stopService();		//停止数据库服务
		void setDataBasePath(const std::string& strPath);	//设置数据库文件路径或连接字符串
		EnumDBType getDBType() const;	//获取数据库类型

		void addListener(IListenner* listen);	//添加异步模式观察者
		void removeListener(IListenner* listen);	//移除异步模式观察者

	public://提供给外部接口
	
		//异步操作，目前只实现了此项目中用到的操作
		uint32_t readDadaBaseCmd(const STRING& strDBName);//添加一个数据库信息（非创建一个新数据库）
		uint32_t createTableCmd(const STRING& strDBName, const STRING& strTableName, const STRING& strSQL,DynaArray2D<STRING> arrValue);//在数据库中添加一个表格，若包含则覆盖
		uint32_t deleteTableCmd(const STRING& strDBName,const STRING& strTableName);//删除表格
		uint32_t updateTableCmd(const STRING& strDBName, const STRING& strTableName, DynaArray2D<STRING> arrValue);//在数据库中添加一个表格，若包含则覆盖

		//同步操作
		bool readDadaBaseSyn(const STRING& strDBName);//添加一个数据库信息（非创建一个新数据库）
		bool createTableSyn(const STRING& strDBName, const STRING& strTableName, const STRING& strSQL, DynaArray2D<STRING> arrValue);//在数据库中添加一个表格，若包含则覆盖
		bool deleteTableSyn(const STRING& strDBName, const STRING& strTableName);//删除表格
		bool updateTableSyn(const STRING& strDBName, const STRING& strTableName, DynaArray2D<STRING> arrValue);//在数据库中添加一个表格，若包含则覆盖

	public:
		DataBaseInfo operator[](const STRING& strDBName);//重载操作符[]，通过数据库名称获取整个数据库数据

	private:
		void taskTread();	//异步数据库操作线程
		
		//内部调用的直接操作数据库接口
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
		

		std::shared_ptr<DBCommonAPI> m_DBOperator;//数据库连接指针
		std::shared_ptr<COMMONDBINFO> m_pDBProceInfo;//数据库操作数据指针
		LoggerPtr m_Logger;
	};

}
```

### 3.3 生产计划管理模块
#### 3.3.1 数据库服务的使用
- 3.3.1.1 服务获取及启动
在生产计划管理模块，所有对话框使用同一个数据库服务实例，定义获取数据库服务实例接口如下：

```c++
std::shared_ptr<DBService>& getProPlanDBService()
{
	static std::shared_ptr<DBService> prodPlanDBServicePtr = nullptr;
	if (nullptr == prodPlanDBServicePtr)//单线程调用
	{
		prodPlanDBServicePtr = std::make_shared<DBService>(EnumDBType::DATA_SOURCE_ACCESS);//数据库连接类型为ACCESS
		//设置数据库服务信息
		std::string strDBFolderPath;
		GlobalFunction::GetDllPath(strDBFolderPath);
		strDBFolderPath += "\\DataBase\\Access";//保存文件路径用
		prodPlanDBServicePtr->setDataBasePath(strDBFolderPath);//设置数据库路径
	}
	return prodPlanDBServicePtr;
}
```
启动数据库服务，读取所有数据库文件，加到内存，提高数据库科访问速度

```c++
bool initProdPlanDBService() 
{
	if (!InitDataBaseFiles())//初始化数据库文件
	{
		return false;
	}
	auto DBPtr = getProPlanDBService();	//获取服务指针
	DBPtr->startService();	//启动服务
	
	DBPtr->readDadaBaseCmd("WorkPlaceInfo");			//导入工位信息数据库信息
	DBPtr->readDadaBaseCmd("ProductionProcessInfo");	//导入工艺流程数据库信息
	DBPtr->readDadaBaseCmd("ProductionPlanedInfo");		//导入已排列的生产计划数据库信息
	DBPtr->readDadaBaseCmd("WorkPlaceUsedInfo");		//导入已排列的生产计划的工位使用数据库信息
	
	//启动线程池服务
	getThreadPoolService()->start();
}
```
- 3.3.1.2 数据库服务使用
以工艺流程表管理类（文件TechnolProcePrepareDlg.h）实例数据库操作示例：
为了收取异步信息，对话框类继承IListenner接口并实现 void dealDBResult(std::queue<DBResult>& rst)接口，如下：
```c++
class CTechnolProcePrepareDlg : public CDialog, public IListenner//继承监听类
{
	//略……
	virtual void dealDBResult(std::queue<DBResult>& rst) override;//实现具体监听接口
	//略……
	std::shared_ptr<DBService> m_dbService;//数据库服务对象指针
}
```
在构造函数中将对话框添加到数据库服务监听列表
```c++
CTechnolProcePrepareDlg::CTechnolProcePrepareDlg(std::shared_ptr<DBService> dbService, CWnd* pParent /*=NULL*/)
	: CDialog(CTechnolProcePrepareDlg::IDD, pParent)
	, m_dbService(getProPlanDBService())//获取数据库服务
	, m_fWorkTime(0)
{
	m_dbService->addListener(this);//添加到数据库服务监听列表
}

//析构函数
CTechnolProcePrepareDlg::~CTechnolProcePrepareDlg()
{
	m_dbService->removeListener(this);//移除数据库服务监听列表
}
```
在源文件中实现接口dealDBResult，将异步操作结果显示到对话框界面
```c++
void CTechnolProcePrepareDlg::dealDBResult(std::queue<DBResult>& rst)
{
	LOGDEBUG("[%s][%d] begin, result size %d !", __STRING_FUNCTION__, __LINE__, rst.size());

	while (rst.size() > 0)
	{
		auto iterRst = rst.front();
		if (g_mapDealResult.find(iterRst.cmdID) != g_mapDealResult.end())//是本对话框的请求
		{
			//处理显示结果，略……
		}
		rst.pop();
	}
}
```
读取数据库中表格信息变得非常方便
例如读取数据库中所有工位信息
```c++
void CTechnolProcePrepareDlg::ShowWorkplaceToCombo1(void)
{
	//获取工位信息表
	TableInfo temTable = (*m_dbService)["WorkPlaceInfo"]["WorkPlaceDef"];//数据库名称“WorkPlaceInfo”，表格名称“WorkPlaceDef”
	size_t nRowCount = temTable.m_Values.getRowCount();//获取行数（工位数量）
	size_t nCol = temTable.m_mapCols.at("WorkPlaceName");//找工位名称列索引
	//显示道列表控件
	for (int i = nRowCount - 1; i >= 0; --i)
	{
		m_combo1.InsertString(0, temTable.m_Values[i][nCol]);
	}
	m_combo1.SetCurSel(-1);
}
```
在TableInfo类中，包含如下数据成员
```c++
STRING m_strDBName;					//所属数据库名称
STRING m_strTableName;				//表格名称
std::vector<STRING> m_vevCols;		//所有的列名称
std::map<STRING,int32_t> m_mapCols;	//<列名称,列索引>
DynaArray2D<STRING> m_Values;		//表格中数据
```
创建新的表格到数据库
```c++
//略……
DynaArray2D<STRING> temValues(nRowCount, nColumuCount);构造表格大小
for (int i = 0; i < nRowCount; ++i)
{
	for (int j = 0; j < nColumuCount; ++j)
	{
		temValues[i][j] = m_grid1.GetItemText(i, j);//获取表格信息
	}
}
//构造SQL语句，不同数据库类型会有不同
CString strSQL = "CREATE TABLE " + strTableName + "(Num TEXT ,	\
	ProcLevel TEXT,	MertID TEXT,MaterialName TEXT,MdlName TEXT,MdlType TEXT,CountNum TEXT,ParentMdl TEXT,	\
	ProcInfo TEXT,WorkTime TEXT,CountTime TEXT,DateInfo TEXT,AttachInfo1 TEXT)";

//添加表格命令
uint32_t cmdID = m_dbService->createTableCmd("ProductionProcessInfo", strTableName, strSQL, std::move(temValues));//表格信息使用std::move提升效率，返回命令码
g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable));//保存命令码，用于识别返回命令中是否为自己的操作
```
//删除表格
```c++
m_dbService->deleteTableCmd(strDataBaseName, strTableName);
```
#### 3.3.2 生产计划排列算法简介

**排列生产计划符合 下列基本原则：**
> 1、当天物料生产所占用工位时间不能超过任一工位空余时间；
2、零部件需要在其子零部件全部生产完成后才能开始生产；

**排列方法评价准则**
>1、同一种物料使用相同材料和工具，因此尽量集中生产。（物料集中生产为优）
2、从开始排产到所有成品交付，成品平均交付时间越短越好。（平均交付时间短为优）

此上述2条准则为负相关，举个简单例子，要生产10个A产品，每个A产品各由1个A1、1个A2组成，两个极端方案如下：
方案1：先生产1个A1和1个A2,再生产1个A，循环10次。此方案可以让10个A的平均交付时间较短，但一些共用工位需要在不同种物料之间
转换生产，实际效率会降低；
方案2：先生产10个A1，再生产10个A2，最后一起生产10个A。此方案10个A的平均交付时间较长，但同一物料集中生产，非常方便高效。

生产计划排列在TechnolProductionPlanDlg.h(cpp)文件，生产计划排列按钮关联void CTechnolProductionPlanDlg::OnBnClickedBtnProduPlan()函数，生产计划排列对话框提供按指定数量排列和自动排列两种方式，自动排列根据工位占用情况、物料生产集中度及成品生产周期三个方面进行评价，推荐最优排列方法。
```c++
void CTechnolProductionPlanDlg::OnBnClickedBtnProduPlan()
{
	//略
	
	//获取排列用的信息
	if (radio->GetCheck() == 1)//按指定数量排列生产计划
	{
		//略……
	}
	else//智能排列生产计划
	{
		//如果结果对话框存在，销毁窗口
		RESULT_WINDOW_CHECK_CLOSE

		//创建线程排列生产计划
		std::thread(&CTechnolProductionPlanDlg::GenerateProductionPlanAuto,this,
			m_nTotalProdNum,
			mapWorkPlaceIndex,
			arrWorkPlaceFree,
			vecMapWorkPlaceUse,
			vecTimeToRoot,
			vecMertID,
			vecMertNumPer).detach();
	}
}
```
调用的自动排列函数如下
```c++
void CTechnolProductionPlanDlg::GenerateProductionPlanAuto(
	const uint16_t nTotalNum	//总排列数目
	, const std::map<CString, uint16_t>& mapWorkPlIndex	//workplace info, <key:workplaceName ,value:workplace index>
	, const DynaArray2D<float>& arrWorkFree	//workplace info, free information
	, const std::vector<std::map<uint16_t, float>>& vecMapWorkPlaceUse	// material information ,map <key:workplace Index,value:workplace cost time>
	, const std::vector<float>& vecTimeToRoot	// material information ,cumulative time to root node);	
	, const std::vector<CString>& vecMertID	// material id
	, const std::vector<uint16_t>& vecMertNumPer)	 // material num per product
{
	//略……
	
	std::vector<std::future<stuPlanRet>> vecFetResult;//保存线程提交返回值
	vecFetResult.reserve(m_nTotalProdNum);
	for (int numPer = 0; numPer <m_nTotalProdNum; ++numPer)//穷举法排列，任务添加到线程池
	{
		vecFetResult.emplace_back(m_threadPool->submit(
			GenerateProductionPlan, //调用排列函数，封装到ProductionPlanCalculateLib/ProductionPlanCalculate.h中了
			m_nTotalProdNum,
			numPer + 1,
			mapWorkPlIndex,
			arrWorkFree,
			vecMapWorkPlaceUse,
			vecTimeToRoot,
			vecMertID,
			vecMertNumPer));
	}
	
	//等待排列结果
	std::vector<stuPlanRet> vecResult;//保存所有的结果
	vecResult.reserve(m_nTotalProdNum);
	//略……
	
	do{
		for (int k = 0; k <m_nTotalProdNum; ++k)/////从高往低排，任务添加到线程池
		{
			//略……
			
			拿到排列结果
			//stuPlanRet temRet = vecFetResult[k].get();
			
			//略……

			//保存所有结果
			vecResult.emplace_back(std::move(temRet));
		
			//略……
		}
		
		if(true == isFailed)
		{
			break;
		}

		//计算最佳排列结果
		nBestNum = m_ResultDlg.CalculatBestSolution(fVecTimeLong, fVecTotalDayMer, fVecTotalWorkPlace);
		if (nBestNum == 0)
		{
			isFailed = true;
			break;
		}
	} while (false);
	
	//是否失败
	//略……

	//获取最佳排列结果
	//略……
	
	//显示排列结果到列表控件1中
	//略……
}
```
## 4. 程序演示
见文档《程序演示》或本文档”修改记录“中视频
