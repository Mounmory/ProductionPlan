#pragma once
#ifdef PRO_USE_VAR_ARGS
//Creo 二次开发头文件
#include <ProToolkit.h>
#include <ProUtil.h>
#include <ProArray.h>
#include <ProDimension.h>
#include <ProWindows.h>
#include <ProSolid.h>
#include <ProParameter.h>
#include <ProParamval.h>
#include <ProMessage.h>
#include <ProMenu.h>
#include <ProMenuBar.h>
#include <ProMdl.h>
#include <ProModelitem.h>
#include <ProDrawing.h>
#include <ProFeature.h>
#include <ProFeatType.h>
#include <ProDrawing.h>
#include <ProNotify.h>
#include <ProRefInfo.h>
#include <ProRelSet.h>
#include <ProSelection.h>
#include <ProUICmd.h>
#include <ProNotify.h>
#include <ProPopupmenu.h>
#include <ProSelbuffer.h>
#include <ProAsmcomp.h>
#include <ProAsmcomppath.h>
#include <ProTKRunTime.h>

#include <ProAxis.h>
#include <ProSurface.h>
#include <ProEdge.h>
#endif


namespace GlobalFunction
{
	int leap(int year);

	int days(int year, int month);

	//转换控件中日期
	CString translatDay(CString day);
	//后一天
	CString nextDay(CString day);
	//获取工作目录
	void GetDllPath(CString &strPath);
	void GetDllPath(std::string &strPath);

#ifdef PRO_USE_VAR_ARGS
	// 激活当前窗口
	ProError UsrActivateCurrentWindow();

	// 重绘窗口
	ProError UsrRepaintWindow();

	//获取当前模型
	ProMdl GetCurrentMdl();

	//把模型下面的元件添加到树节点下面（这里面用到了树控件）
	void ShowModelTreeWithFeatrue(ProSolid solid, CTreeCtrl & m_Tree//只包括零件和装配体，不包括特征
		, HTREEITEM hItem, CArray<ProFeature, ProFeature> & p_treeitem);

	void ShowModelTreeWhithoutFeature(ProSolid solid, CTreeCtrl & m_Tree//不添加特征
		, HTREEITEM hItem, CArray<ProFeature, ProFeature> & p_treeitem, bool bHasID = false);//bHasID =true 时，模型名称中带ID

	//模型特征访问动作函数
	ProError FeatureAsmVisitAcFnWithFeature(ProFeature *p_object, ProError status,//包括零件、装配体、特征
		CArray<ProFeature, ProFeature> * app_data);

	ProError FeatureAsmVisitAcFnWhithoutFeature(ProFeature *p_object, ProError status,//不添加特征
		CArray<ProFeature, ProFeature> * app_data);

	//零件特征的访问动作函数定义
	ProError  UsrPartFeatVisFn(ProFeature* p_feature
		, ProError status, CArray<ProFeature, ProFeature> * app_data);

	void AsmcompConstraintsToList(ProSolid solid, CListCtrl &grid1, CListCtrl &grid2);

//	//模型特征获取函数
//	ProError SolidFeatGet(ProSolid solid, ProFeature **p_data);
//
//	//特征访问函数
//	ProError FeatureVisitActionFn(ProFeature *p_object, ProError status, ProAppData app_data);
//
//	//特征访问过滤函数
//	ProError FeatureFilterActionFn(ProFeature *p_object, ProError status, ProAppData app_data);
	namespace ProOpenMdl
	{
		ProError UserOpenComponent(ProSolid* solid);//使用Creo对话框打开组件

		int ProUtilConfirmNameType(char *input_name, char *name, ProType *type);//模型名称检查

		ProType ProUtilGetProType(char *type_str);//

		int ProUtilStrcmp(char *s, char *t);
	}
#endif
}
