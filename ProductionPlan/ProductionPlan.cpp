// ProductionPlan.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "ProductionPlan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



//对话框头文件
#include "TechnolProcePrepareDlg.h"//工艺流程表编制对话框头文件
#include "TechnolProductionPlanDlg.h"//生产计划排列对话框
#include "TechnolPlanAndWorkPlaceViewDlg.h"//查看工位信息和生产计划对话框


//
//TODO: 如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CProductionPlanApp

BEGIN_MESSAGE_MAP(CProductionPlanApp, CWinApp)
END_MESSAGE_MAP()


// CProductionPlanApp 构造

CProductionPlanApp::CProductionPlanApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中

}


// 唯一的一个 CProductionPlanApp 对象

CProductionPlanApp theApp;



// CProductionPlanApp 初始化

BOOL CProductionPlanApp::InitInstance()
{
	CWinApp::InitInstance();
	return TRUE;
}
bool InitDataBaseFiles();//初始化数据库文件夹

bool initProdPlanDBService();//初始化数据库服务

void terminateProdPlanDBService();//结束数据库服务

void TecnProcessPrepareDlg();//打开工艺流程表编制对话框

void TecnProcessPlanDlg();//打开编制生产计划表对话框

void TecnProcessPlanAndWorkPlaceViewDlg();//打开工位信息查看、管理对话框



//在注册Creo注册dll
#ifdef PRO_USE_VAR_ARGS

//**************访问控制函数*********************/////
/////*************一直有效****************/////
static uiCmdAccessState AccessDefault(uiCmdAccessMode access_mode)
{
	return ACCESS_AVAILABLE;
}
//************当前模型是零件时有效************//////
static uiCmdAccessState AccessPart(uiCmdAccessMode access_mode)
{
	//如果当前存在模型且为零件模型则有效
	ProMdlType   p_type; 
	ProMdl       mdl;
	ProError     status;
	status=ProMdlCurrentGet(&mdl);
	if (status==PRO_TK_NO_ERROR)
	{
		ProMdlTypeGet(mdl,&p_type);
		if (p_type==PRO_MDL_PART)
		{
			return ACCESS_AVAILABLE;
		}		
	}
	return ACCESS_UNAVAILABLE;

}
//************当前模型是装配件时有效************//////
static uiCmdAccessState AccessAssemble(uiCmdAccessMode access_mode)
{
	//如果当前存在模型且为组装模型则有效
	ProMdlType   p_type; 
	ProMdl       mdl;
	ProError     status;
	status=ProMdlCurrentGet(&mdl);
	if (status==PRO_TK_NO_ERROR)
	{
		ProMdlTypeGet(mdl,&p_type);
		if (p_type==PRO_MDL_ASSEMBLY)
		{
			return ACCESS_AVAILABLE;
		}		
	}
	return ACCESS_UNAVAILABLE;
}
//************当前存在模型有效************//////
static uiCmdAccessState AccessSolid(uiCmdAccessMode access_mode)
{
	////如果当前存在模型且为零件模型则有效
	//ProSolid	sld;
	//sld=(ProSolid)GetCurrentMdl();
	//if (sld!=NULL)
	//{
	//	return ACCESS_AVAILABLE;
	//}
	//return ACCESS_UNAVAILABLE;
}


//程序初始化
extern "C" int user_initialize(int argc,char *argv[])
{
	if (!initProdPlanDBService())
	{
		return PRO_TK_GENERAL_ERROR;
	}
	getProPlanDBService()->startService();//启动数据库服务
	AfxEnableControlContainer();//Creo能够正常使用模型预览ActiveX控件
	ProError         status;
	ProFileName      Msg;
	ProStringToWstring(Msg,"ProductionPlanMenu.txt");//信息文件名称
//菜单动作函数按钮
	uiCmdCmdId     
//生产计划管理系统
					TecnProcessPrepare_id,//生成工艺流程表
					ProducPlanMana_id,//生产计划安排
					PlanAndStationView_id;///工位设置及生产计划查看
	status=ProMenubarMenuAdd("ProductionPlanSystem","ProductionPlanSystem","help",PRO_B_TRUE,Msg);
/////*********工艺流程表编制
	status=ProCmdActionAdd("TecnProcessPrepareNew",(uiCmdCmdActFn)TecnProcessPrepareDlg,
			uiCmdPrioDefault,AccessAssemble,PRO_B_TRUE,PRO_B_TRUE,&TecnProcessPrepare_id);	
	status=ProMenubarmenuPushbuttonAdd(
		"ProductionPlanSystem",//父菜单
		"ProcessTablePrepare",//按钮名称
		"ProcessTablePrepare",//菜单标签——信息文件中显示的菜单名
		"ProcessTablePrepareInfo",//信息文件中鼠标停留显示的帮助信息
		NULL,PRO_B_TRUE,TecnProcessPrepare_id,Msg);
	/////*********生产计划表编制
	status=ProCmdActionAdd("TecnProcessPlanNew",(uiCmdCmdActFn)TecnProcessPlanDlg,
			uiCmdPrioDefault,AccessDefault,PRO_B_TRUE,PRO_B_TRUE,&ProducPlanMana_id);	
	status=ProMenubarmenuPushbuttonAdd(
		"ProductionPlanSystem",//父菜单
		"ProcessPlan",//按钮名称
		"ProcessPlan",//菜单标签——信息文件中显示的菜单名
		"ProcessPlanInfo",//信息文件中鼠标停留显示的帮助信息
		NULL,PRO_B_TRUE,ProducPlanMana_id,Msg);
	/////*********查看工位信息及生产计划信息
	status=ProCmdActionAdd("TecnProcessPlanViewNew",(uiCmdCmdActFn)TecnProcessPlanAndWorkPlaceViewDlg,
			uiCmdPrioDefault,AccessDefault,PRO_B_TRUE,PRO_B_TRUE,&PlanAndStationView_id);	
	status=ProMenubarmenuPushbuttonAdd(
		"ProductionPlanSystem",//父菜单
		"ProcessPlanAndWorkPlaceView",//按钮名称
		"ProcessPlanAndWorkPlaceView",//菜单标签——信息文件中显示的菜单名
		"ProcessPlanAndWorkPlaceViewInfo",//信息文件中鼠标停留显示的帮助信息
		NULL,PRO_B_TRUE,PlanAndStationView_id,Msg);
	return 0;
}

//程序结束
extern "C" void user_terminate()
{	
	getProPlanDBService()->stopService();
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

}

#endif
bool initProdPlanDBService() 
{
	//数据库服务
	if (!InitDataBaseFiles())
	{
		return false;
	}
	auto DBPtr = getProPlanDBService();
	DBPtr->startService();
	DBPtr->readDadaBaseCmd("WorkPlaceInfo");			//导入工位信息数据库信息
	DBPtr->readDadaBaseCmd("ProductionProcessInfo");	//导入工艺流程数据库信息
	DBPtr->readDadaBaseCmd("ProductionPlanedInfo");		//导入已排列的生产计划数据库信息
	DBPtr->readDadaBaseCmd("WorkPlaceUsedInfo");		//导入已排列的生产计划的工位使用数据库信息
	
	//线程池服务
	ThreadPool::getThreadPool()->start();

	return true;
}

void terminateProdPlanDBService() 
{
	//数据库服务
	auto& DBPtr = getProPlanDBService();
	DBPtr->stopService();
	DBPtr = nullptr;//析构

	//线程池
	ThreadPool::getThreadPool()->stop();
}

void TecnProcessPrepareDlg()
{
	//std::shared_ptr<DBService> intance1 = getProPlanDBService();
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CTechnolProcePrepareDlg TecProDlg(getProPlanDBService());
	TecProDlg.DoModal();
}

void TecnProcessPlanDlg()//编制生产计划表
{
	std::shared_ptr<DBService> intance1 = getProPlanDBService();
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CTechnolProductionPlanDlg TecProPlanDlg;
	TecProPlanDlg.DoModal();
}

void TecnProcessPlanAndWorkPlaceViewDlg()//工位信息查看、管理
{
	//std::shared_ptr<DBService> intance1 = getProPlanDBService();
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CTechnolPlanAndWorkPlaceViewDlg TecPlacAndPlanViewDlg;
	TecPlacAndPlanViewDlg.DoModal();
}

bool InitDataBaseFiles()
{
	//检查数据库文件夹是否存在
	std::string strDBFolderPath;
	GlobalFunction::GetDllPath(strDBFolderPath);
	strDBFolderPath += "\\DataBase";//保存文件路径用
	if (_access(strDBFolderPath.c_str(), 0) == -1)//如果数据库文件夹不存在，则创建
	{
		BOOL NewFile1 = CreateDirectory(strDBFolderPath.c_str(), 0);//则创建
		if (NewFile1 == 0)
		{
			return false;
		}
	}
	strDBFolderPath += "\\Access";
	if (_access(strDBFolderPath.c_str(), 0) == -1)//如果数据库文件夹不存在，则创建
	{
		BOOL NewFile1 = CreateDirectory(strDBFolderPath.c_str(), 0);//则创建
		if (NewFile1 == 0)
		{
			return false;
		}
	}
	//如果数据库不存在，则创建
	std::unique_ptr<DBCommonAPI> DBConnFunc(new DBAccessConn);
	std::shared_ptr<COMMONDBINFO> DBOperaInfo(new ADODBINFO);

	DBOperaInfo->stuDBOperate.ClearCreatInfo();
	DBOperaInfo->stuDBOperate.EnuCreatType = EnumDBCreatType::CREAT_DB;//创建数据库
	DBOperaInfo->stuDBOperate.strInDBPath = strDBFolderPath.c_str();//路径
	std::string strFilePath;//工艺流程表数据库文件
	
	//检查数据库是否存在
	strFilePath = strDBFolderPath + "\\ProductionPlanedInfo.mdb";//生产计划表
	if (_access(strFilePath.c_str(), 0) == -1)
	{
		DBOperaInfo->stuDBOperate.strInDBname = "ProductionPlanedInfo";
		if (!DBConnFunc->CreatDataBase(DBOperaInfo))
		{
			return false;
		}
	}

	strFilePath = strDBFolderPath + "\\ProductionProcessInfo.mdb";//工艺流程表
	if (_access(strFilePath.c_str(), 0) == -1)
	{
		DBOperaInfo->stuDBOperate.strInDBname = "ProductionProcessInfo";
		if (!DBConnFunc->CreatDataBase(DBOperaInfo))
		{
			return false;
		}
	}

	strFilePath = strDBFolderPath + "\\WorkPlaceUsedInfo.mdb";//工位使用信息
	if (_access(strFilePath.c_str(), 0) == -1)
	{
		DBOperaInfo->stuDBOperate.strInDBname = "WorkPlaceUsedInfo";
		if (!DBConnFunc->CreatDataBase(DBOperaInfo))
		{
			return false;
		}
	}

	strFilePath = strDBFolderPath + "\\WorkPlaceInfo.mdb";//工位信息
	if (_access(strFilePath.c_str(), 0) == -1)
	{
		DBOperaInfo->stuDBOperate.EnuCreatType = EnumDBCreatType::CREAT_DB_AND_TABLE;//创建数据库
		DBOperaInfo->stuDBOperate.strInDBname = "WorkPlaceInfo";//数据库名称
		DBOperaInfo->stuDBOperate.strInCreatTableSQL = "CREATE TABLE WorkPlaceDef (Num INTEGER not null identity(1,1),	WorkPlaceName TEXT,	WorkPlaceNum TEXT)";
		if (!DBConnFunc->CreatDataBase(DBOperaInfo))
		{
			return false;
		}
	}

	strFilePath = strDBFolderPath + "\\生产计划相关数据库说明.txt";//数据库说明文档
	if (_access(strFilePath.c_str(), 0) == -1)
	{
		std::fstream fileWrite;
		fileWrite.open(strFilePath, std::fstream::app);
		if (fileWrite.is_open())
		{
			fileWrite << "WorkPlaceInfo.mdb			工位信息" << std::endl;
			fileWrite << "ProductionProcessInfo.mdb	保存编制好的工艺流程表" << std::endl;
			fileWrite << "ProductionPlanedInfo.mdb	保存排列完成的生产计划" << std::endl;
			fileWrite << "WorkPlaceUsedInfo.mdb		保存排列完的生产计划使用的工位信息" << std::endl;
			fileWrite.close();
		}
	}
	return true;
}

std::shared_ptr<DBService>& getProPlanDBService()
{
	static std::shared_ptr<DBService> prodPlanDBServicePtr = nullptr;
	if (nullptr == prodPlanDBServicePtr)
	{
		prodPlanDBServicePtr = std::make_shared<DBService>(EnumDBType::DATA_SOURCE_ACCESS);
		//设置数据库服务信息
		std::string strDBFolderPath;
		GlobalFunction::GetDllPath(strDBFolderPath);
		strDBFolderPath += "\\DataBase\\Access";//保存文件路径用
		prodPlanDBServicePtr->setDataBasePath(strDBFolderPath);//设置数据库路径
	}
	return prodPlanDBServicePtr;
}