#pragma once
#ifdef PRO_USE_VAR_ARGS
//Creo ���ο���ͷ�ļ�
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

	//ת���ؼ�������
	CString translatDay(CString day);
	//��һ��
	CString nextDay(CString day);
	//��ȡ����Ŀ¼
	void GetDllPath(CString &strPath);
	void GetDllPath(std::string &strPath);

#ifdef PRO_USE_VAR_ARGS
	// ���ǰ����
	ProError UsrActivateCurrentWindow();

	// �ػ洰��
	ProError UsrRepaintWindow();

	//��ȡ��ǰģ��
	ProMdl GetCurrentMdl();

	//��ģ�������Ԫ����ӵ����ڵ����棨�������õ������ؼ���
	void ShowModelTreeWithFeatrue(ProSolid solid, CTreeCtrl & m_Tree//ֻ���������װ���壬����������
		, HTREEITEM hItem, CArray<ProFeature, ProFeature> & p_treeitem);

	void ShowModelTreeWhithoutFeature(ProSolid solid, CTreeCtrl & m_Tree//���������
		, HTREEITEM hItem, CArray<ProFeature, ProFeature> & p_treeitem, bool bHasID = false);//bHasID =true ʱ��ģ�������д�ID

	//ģ���������ʶ�������
	ProError FeatureAsmVisitAcFnWithFeature(ProFeature *p_object, ProError status,//���������װ���塢����
		CArray<ProFeature, ProFeature> * app_data);

	ProError FeatureAsmVisitAcFnWhithoutFeature(ProFeature *p_object, ProError status,//���������
		CArray<ProFeature, ProFeature> * app_data);

	//��������ķ��ʶ�����������
	ProError  UsrPartFeatVisFn(ProFeature* p_feature
		, ProError status, CArray<ProFeature, ProFeature> * app_data);

	void AsmcompConstraintsToList(ProSolid solid, CListCtrl &grid1, CListCtrl &grid2);

//	//ģ��������ȡ����
//	ProError SolidFeatGet(ProSolid solid, ProFeature **p_data);
//
//	//�������ʺ���
//	ProError FeatureVisitActionFn(ProFeature *p_object, ProError status, ProAppData app_data);
//
//	//�������ʹ��˺���
//	ProError FeatureFilterActionFn(ProFeature *p_object, ProError status, ProAppData app_data);
	namespace ProOpenMdl
	{
		ProError UserOpenComponent(ProSolid* solid);//ʹ��Creo�Ի�������

		int ProUtilConfirmNameType(char *input_name, char *name, ProType *type);//ģ�����Ƽ��

		ProType ProUtilGetProType(char *type_str);//

		int ProUtilStrcmp(char *s, char *t);
	}
#endif
}
