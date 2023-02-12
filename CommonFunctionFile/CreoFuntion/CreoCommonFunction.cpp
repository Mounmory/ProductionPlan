#include "stdafx.h"
#include <iostream>
#include <vector>
#include "CreoCommonFunction.h"


namespace GlobalFunction
{
	int leap(int year)
	{
		if (year % 4 == 0 && year % 100 != 0 || year % 100 == 0)
			return 1;
		else return 0;
	}

	int days(int year, int month)
	{
		if (leap(year) && month == 2)
			return 29;
		if (month == 2)
			return 28;
		if (month == 4 || month == 6 || month == 9 || month == 11)
			return 30;
		return 31;
	}

	CString translatDay(CString day)
	{
		int nian, yue, ri;
		nian = atoi(day.Left(4));
		day = day.Right(day.GetLength() - 6);
		int pos = day.Find("��");
		yue = atoi(day.Left(pos));
		day = day.Right(day.GetLength() - pos - 2);
		pos = day.Find("��");
		ri = atoi(day.Left(pos));
		CString after;
		after.Format("%04d-%02d-%02d", nian, yue, ri);
		return after;
	}

	CString nextDay(CString day)
	{
		int nian, yue, ri;
		nian = atoi(day.Left(4));
		yue = atoi(day.Mid(5, 2));
		ri = atoi(day.Right(2));
		if (ri == days(nian, yue))
		{
			ri = 1;
			if (yue == 12)
			{
				yue = 1;
			}
			else
			{
				yue = (yue)+1;
			}
			if (yue == 1) nian++;
		}
		else ri++;
		CString xiayitian;
		xiayitian.Format("%04d-%02d-%02d", nian, yue, ri);
		return xiayitian;
	}

	//��ȡ����Ŀ¼
	void GetDllPath(CString &strPath)
	{
		char path[MAX_STR_LEN] = { 0 };
		HMODULE hModuleInstance = _AtlBaseModule.GetModuleInstance();
		int nlen = GetModuleFileName(hModuleInstance, path, MAX_STR_LEN);
		while (path[nlen - 1] != '\\')
		{
			nlen--;
		}
		path[nlen - 1] = 0;
		strPath = path;
	}

	void GetDllPath(std::string &strPath)
	{
		char path[MAX_STR_LEN] = { 0 };
		HMODULE hModuleInstance = _AtlBaseModule.GetModuleInstance();
		int nlen = GetModuleFileName(hModuleInstance, path, MAX_STR_LEN);
		while (path[nlen - 1] != '\\')
		{
			nlen--;
		}
		path[nlen - 1] = 0;
		strPath = path;
	}

#ifdef PRO_USE_VAR_ARGS
	/************************************************************************/
	/* ���ǰ����                                                         */
	/************************************************************************/
	ProError UsrActivateCurrentWindow()
	{
		int window_id;
		ProError status;
		status = ProWindowCurrentGet(&window_id);
		status = ProWindowActivate(window_id);
		return status;
	}
	/************************************************************************/
	/* �ػ洰��                                                         */
	/************************************************************************/
	ProError UsrRepaintWindow()
	{
		int window_id;
		ProError status;
		status = ProWindowCurrentGet(&window_id);
		status = ProWindowRefresh(window_id);
		return status;
	}

	/************************************************************************/
	/* ��ȡ��ǰģ��                                                */
	/************************************************************************/
	ProMdl GetCurrentMdl()
	{
		ProMdl     mdl;
		ProError   status;
		status = ProMdlCurrentGet(&mdl);
		if (status == PRO_TK_NO_ERROR)
			return mdl;
		return NULL;
	}

	/************************************************************************/
	/* ��ģ�������Ԫ����ӵ����ڵ�����,���������������õ������ؼ���              */
	/************************************************************************/
	void ShowModelTreeWithFeatrue(ProSolid solid, CTreeCtrl & m_Tree
		, HTREEITEM hItem, CArray<ProFeature, ProFeature> & p_treeitem)
	{
		ProError				status;
		CString					csName;
		ProName					wName;
		HTREEITEM				tempItem;
		ProMdlType				type;
		ProFeattype				ftype;
		CArray<ProFeature, ProFeature> feat_list;

		//����ʵ��ģ��solid����������
		status = ProSolidFeatVisit(solid, (ProFeatureVisitAction)FeatureAsmVisitAcFnWithFeature
			, NULL, (CArray<ProFeature, ProFeature>*)&feat_list);
		int num = (int)feat_list.GetCount();

		//ѭ�����ÿ�����������ͣ����ж������Ƿ�ΪԪ��
		for (int i = 0; i < num; i++)
		{
			status = ProFeatureTypeGet(&feat_list[i], &ftype);

			//�������������ΪԪ�������Ԫ�����ļ�����Ӳ��뵽���ڵ�����
			if (ftype == PRO_FEAT_COMPONENT) {
				ProSolid              mdl;
				status = ProAsmcompMdlGet((ProAsmcomp *)(&feat_list[i]), (ProMdl *)&mdl);
				status = ProMdlNameGet(mdl, wName);
				csName = CString(wName);
				status = ProMdlTypeGet(mdl, &type);
				//���Ԫ�������ͣ�װ�������������������ƺ�������ļ���չ��
				switch (type)
				{
				case PRO_MDL_ASSEMBLY:
				{
					csName.Format("%s.ASM(%d)", csName, feat_list[i].id);
					tempItem = m_Tree.InsertItem(TVIF_TEXT | TVIF_PARAM, csName
						, 0, 0, 0, 0, (int)p_treeitem.Add(feat_list[i]), hItem, NULL);
				}
				break;
				case PRO_MDL_PART:
				{
					csName.Format("%s.PRT(%d)", csName, feat_list[i].id);
					tempItem = m_Tree.InsertItem(TVIF_TEXT | TVIF_PARAM, csName
						, 1, 1, 0, 0, (int)p_treeitem.Add(feat_list[i]), hItem, NULL);
				}
				break;
				default:
					break;
				}
				//��װ�������,������һ���ݹ��������
				ShowModelTreeWithFeatrue(mdl, m_Tree, tempItem, p_treeitem);
			}
			//����Ĺ��̿��԰�Ԫ����������ڵ���������������ӵ�����ڵ�����
			else
			{
				status = ProModelitemNameGet(&feat_list[i], wName);
				csName = CString(wName);
				if (csName == "")
				{
					status = ProFeatureTypenameGet(&feat_list[i], wName);
					csName = CString(wName);
				}
				csName.Format("%s(%d)", csName, feat_list[i].id);
				tempItem = m_Tree.InsertItem(TVIF_TEXT | TVIF_PARAM, csName
					, 2, 2, 0, 0,
					//���������ӵ������У����Ѽ�¼����Ϊ�������������
					(int)p_treeitem.Add(feat_list[i]),
					//��Ԫ���ڽ�һ���жϣ�������Ԫ�����������û��Ԫ����������
					hItem, NULL);
			}
		}
	}

	/************************************************************************/
	/* ��ģ�������Ԫ����ӵ����ڵ����棨�������õ������ؼ���              */
	/************************************************************************/
	void ShowModelTreeWhithoutFeature(ProSolid solid, CTreeCtrl & m_Tree
		, HTREEITEM hItem, CArray<ProFeature, ProFeature> & p_treeitem, bool bHasID)
	{
		ProError				status;
		CString					csName;
		ProName					wName;
		HTREEITEM				tempItem;
		ProMdlType				type;
		ProFeattype				ftype;
		CArray<ProFeature, ProFeature> feat_list;

		//����ʵ��ģ��solid����������
		status = ProSolidFeatVisit(solid, (ProFeatureVisitAction)FeatureAsmVisitAcFnWhithoutFeature
			, NULL, (CArray<ProFeature, ProFeature>*)&feat_list);
		int num = (int)feat_list.GetCount();
		//ѭ�����ÿ�����������ͣ����ж������Ƿ�ΪԪ��
		for (int i = 0; i < num; i++)
		{
			status = ProFeatureTypeGet(&feat_list[i], &ftype);
			//�������������ΪԪ�������Ԫ�����ļ�����Ӳ��뵽���ڵ�����
			if (ftype == PRO_FEAT_COMPONENT) {
				ProSolid              mdl;
				status = ProAsmcompMdlGet((ProAsmcomp *)(&feat_list[i])
					, (ProMdl *)&mdl);
				status = ProMdlNameGet(mdl, wName);
				csName = CString(wName);
				status = ProMdlTypeGet(mdl, &type);
				if (type == PRO_MDL_ASSEMBLY)
				{
					bHasID == true ? csName.Format("%s.asm (%d)", csName.MakeLower(), feat_list[i].id) : csName.Format("%s.asm", csName.MakeLower());
					tempItem = m_Tree.InsertItem(csName, 0, 0, hItem);
					//��װ�������,������һ���ݹ��������
					ShowModelTreeWhithoutFeature(mdl, m_Tree, tempItem, p_treeitem, bHasID);
				}
				else if (type == PRO_MDL_PART)
				{
					bHasID == true ? csName.Format("%s.prt (%d)", csName.MakeLower(), feat_list[i].id) : csName.Format("%s.prt", csName.MakeLower());
					m_Tree.InsertItem(csName, 1, 1, hItem);
				}

			}
		}
	}
	/************************************************************************/
	/* ģ���������ʶ�������                                                 */
	/************************************************************************/
	ProError FeatureAsmVisitAcFnWithFeature(ProFeature *p_object, ProError status,
		CArray<ProFeature, ProFeature> * app_data)

	{
		ProBoolean     isVisible;
		int         *  r_failed_feat_ids;
		int            num = 0;
		ProBoolean     p_is_incomplete;
		ProFeatStatus  p_status;
		//��������   
		//����ʧ��
		status = ProArrayAlloc(0, sizeof(int), 1, (ProArray*)&r_failed_feat_ids);
		status = ProSolidFailedFeatsList((ProSolid)(p_object->owner)
			, &r_failed_feat_ids);
		status = ProArraySizeGet((ProArray)r_failed_feat_ids, &num);
		for (int i = 0; i < num; i++)
		{
			if (p_object->id == r_failed_feat_ids[i])
			{
				ProArrayFree((ProArray*)&r_failed_feat_ids);
				return PRO_TK_NO_ERROR;
			}
		}
		ProArrayFree((ProArray*)&r_failed_feat_ids);
		//������������
		status = ProFeatureIsIncomplete(p_object, &p_is_incomplete);
		if (p_is_incomplete != PRO_B_FALSE) return PRO_TK_NO_ERROR;
		//����û�б����������������
		status = ProFeatureStatusGet(p_object, &p_status);
		if (p_status == PRO_FEAT_SUPPRESSED) return PRO_TK_NO_ERROR;
		if (p_status != PRO_FEAT_ACTIVE)    return PRO_TK_NO_ERROR;
		//�����ɼ�
		status = ProFeatureVisibilityGet(p_object, &isVisible);
		if (isVisible == PRO_B_TRUE) {
			app_data->Add(*p_object);
		}

		return PRO_TK_NO_ERROR;
	}

	/************************************************************************/
	/* ģ���������ʶ�������                                                 */
	/************************************************************************/
	ProError FeatureAsmVisitAcFnWhithoutFeature(ProFeature *p_object, ProError status,
		CArray<ProFeature, ProFeature> * app_data)

	{
		ProBoolean     isVisible;
		int         *  r_failed_feat_ids;
		int            num = 0;
		ProBoolean     p_is_incomplete;
		ProFeatStatus  p_status;
		//��������   
		//����ʧ��
		status = ProArrayAlloc(0, sizeof(int), 1, (ProArray*)&r_failed_feat_ids);
		status = ProSolidFailedFeatsList((ProSolid)(p_object->owner)
			, &r_failed_feat_ids);
		status = ProArraySizeGet((ProArray)r_failed_feat_ids, &num);
		for (int i = 0; i < num; i++)
		{
			if (p_object->id == r_failed_feat_ids[i])
			{
				ProArrayFree((ProArray*)&r_failed_feat_ids);
				return PRO_TK_NO_ERROR;
			}
		}
		ProArrayFree((ProArray*)&r_failed_feat_ids);
		//������������
		status = ProFeatureIsIncomplete(p_object, &p_is_incomplete);
		if (p_is_incomplete != PRO_B_FALSE) return PRO_TK_NO_ERROR;
		//����û�б����������������
		status = ProFeatureStatusGet(p_object, &p_status);
		if (p_status == PRO_FEAT_SUPPRESSED) return PRO_TK_NO_ERROR;
		if (p_status != PRO_FEAT_ACTIVE)    return PRO_TK_NO_ERROR;
		//�����ɼ�
		status = ProFeatureVisibilityGet(p_object, &isVisible);
		if (isVisible == PRO_B_TRUE) {
			app_data->Add(*p_object);
		}

		return PRO_TK_NO_ERROR;
	}

	/************************************************************************/
	/* ʵ���������ʶ�������                                                 */
	/************************************************************************/
	ProError UsrPartFeatVisFn(ProFeature* p_feature, ProError status,
		CArray<ProFeature, ProFeature> * app_data)
	{
		ProBoolean     isVisible;
		int         *  r_failed_feat_ids;
		int            num = 0;
		ProBoolean     p_is_incomplete;
		ProFeatStatus  p_status;
		//��������   
		//1.����ʧ��
		status = ProArrayAlloc(0, sizeof(int), 1, (ProArray*)&r_failed_feat_ids);
		status = ProSolidFailedFeatsList((ProSolid)(p_feature->owner), &r_failed_feat_ids);
		status = ProArraySizeGet((ProArray)r_failed_feat_ids, &num);
		for (int i = 0; i < num; i++)
		{
			if (p_feature->id == r_failed_feat_ids[i])
			{
				ProArrayFree((ProArray*)&r_failed_feat_ids);
				return PRO_TK_NO_ERROR;
			}
		}
		//2.������������
		status = ProFeatureIsIncomplete(p_feature, &p_is_incomplete);
		if (p_is_incomplete != PRO_B_FALSE) return PRO_TK_NO_ERROR;
		//3.����û�б����������������
		status = ProFeatureStatusGet(p_feature, &p_status);
		if (p_status == PRO_FEAT_SUPPRESSED) return PRO_TK_NO_ERROR;
		if (p_status != PRO_FEAT_ACTIVE)    return PRO_TK_NO_ERROR;
		//4.�����ɼ�
		status = ProFeatureVisibilityGet(p_feature, &isVisible);
		if (isVisible == PRO_B_TRUE) {
			app_data->Add(*p_feature);
		}
		return PRO_TK_NO_ERROR;
	}

	static struct str_to_Feattype//��������
	{
		char *str;
		ProType type;
	}str_type_feat_table[] =
	{
		{ "��", PRO_AXIS },
		{ "��", PRO_SURFACE },
		{ "��", PRO_EDGE },
		{ "��", PRO_POINT },
		{ "����", PRO_CSYS },
		{ "�����", PRO_EDGE_START },
		{ "���յ�", PRO_EDGE_END },
		{ "???", PRO_TYPE_INVALID }
	};

	CString FeatureNameGet(pro_obj_types enmType)
	{
		CString str;
		int nSize = sizeof(str_type_feat_table) / sizeof(str_to_Feattype);
		for (int i = 0; i < nSize; ++i)
		{
			if (str_type_feat_table[i].type == enmType)
			{
				str = str_type_feat_table[i].str;
				break;
			}
		}
		return str;
	}

	static struct str_to_ConstraintType
	{
		char *str;
		ProAsmcompConstrType type;

	}str_cons_type_table[]
	{
		{ "ƥ��", PRO_ASM_MATE },
		{ "ƥ��ƫ��", PRO_ASM_MATE_OFF },
		{ "����", PRO_ASM_ALIGN },
		{ "����ƫ��", PRO_ASM_ALIGN_OFF },
		{ "����", PRO_ASM_INSERT },
		{ "����", PRO_ASM_ORIENT },
		{ "����ϵ", PRO_ASM_CSYS },
		{ "����", PRO_ASM_TANGENT },
		{ "�����ϵ�", PRO_ASM_PNT_ON_SRF },
		{ "�����ϱ�", PRO_ASM_EDGE_ON_SRF },
		{ "Ĭ��", PRO_ASM_DEF_PLACEMENT },
		{ "�滻", PRO_ASM_SUBSTITUTE },
		{ "���ϵ�", PRO_ASM_PNT_ON_LINE },
		{ "�̶�", PRO_ASM_FIX },
		{ "�Զ�", PRO_ASM_AUTO },
	};

	CString ConstraintsNameGet(ProAsmcompConstrType enmType)
	{
		CString str;
		int nSize = sizeof(str_cons_type_table) / sizeof(str_to_ConstraintType);
		for (int i = 0; i < nSize; ++i)
		{
			if (str_cons_type_table[i].type == enmType)
			{
				str = str_cons_type_table[i].str;
				break;
			}
		}
		return str;
	}

	void AsmcompConstraintsToList(ProSolid solid, CListCtrl &grid1, CListCtrl &grid2)//����һ�ֹ��������ķ���
	{
		ProError				status;
		CArray<ProFeature, ProFeature> feat_list;

		//����ʵ��ģ��solid����������
		status = ProSolidFeatVisit(solid, (ProFeatureVisitAction)FeatureAsmVisitAcFnWithFeature
			, NULL, (CArray<ProFeature, ProFeature>*)&feat_list);
		int num = (int)feat_list.GetCount();

		//ѭ�����ÿ�����������ͣ����ж������Ƿ�ΪԪ��
		for (int i = 0; i < num; i++)
		{
			ProFeattype				featType;
			status = ProFeatureTypeGet(&feat_list[i], &featType);
			if (featType == PRO_FEAT_COMPONENT) //ֻ��ȡ�����װ���ϵ
			{
				CString strMdlName,strMdlNameWithID;//ģ������
				CString strConstrType, strConstrFeatInfo, strConstrState, strMdlPath;//Լ����Ϣ
				CString strMatrix;//λ�˾���

				ProSolid		mdl;
				ProName			mdlName;
				ProMdlType		mdlType;

				ProAsmcomp		asmcomp(*(ProAsmcomp *)(&feat_list[i]));
				status = ProAsmcompMdlGet(&asmcomp, (ProMdl *)&mdl);
				status = ProMdlNameGet(mdl, mdlName);
				status = ProMdlTypeGet(mdl, &mdlType);
				//��ȡģ������
				strMdlName.Format("%s.%s", CString(mdlName), mdlType == PRO_MDL_ASSEMBLY ?"ASM":"PRT");
				strMdlNameWithID.Format("%s.%s(%d)", CString(mdlName), mdlType == PRO_MDL_ASSEMBLY ? "ASM" : "PRT",asmcomp.id);
				//��ȡģ��Լ��
				ProAsmcompconstraint* constr_array;

				ProSelection asmConstr, compConstr;
				ProDatumside dtmside;
				ProModelitem asmfeat, comfeat;
				ProAsmcomppath asmCompPath,compCompPath;//��������װ����Ϣ·��

				int nSize = 0;
				status = ProAsmcompConstraintsGet(&asmcomp, &constr_array);
				if (status != PRO_TK_NO_ERROR)
					return;
				ProArraySizeGet(constr_array, &nSize);
				for (int j = 0; j < nSize; j++)
				{

					////��ȡԼ�����ͺ�Լ��ֵ
					//ProAsmcompConstrType asmConsYype;
					//CString strTemConsValue, strTemConsType;
					//double fOffset(0);
					//status = ProAsmcompconstraintOffsetGet(constr_array[i], &fOffset);//���װ��Ԫ��Լ����ƫ��ֵ
					//status == PRO_TK_NO_ERROR ? strTemConsValue.Format("ƫ��ֵ = %6.3lf", fOffset) : strTemConsValue = "";
					//status = ProAsmcompconstraintTypeGet(constr_array[i], &asmConsYype);//���װ��Ԫ��Լ��������
					//ConstraintsNameGet(asmConsYype);//����UserConstraintTypeToString����
					//strConstrType.Format("%s(%s,%s)", strConstrType, ConstraintsNameGet(asmConsYype), strTemConsValue);

					//��ȡԼ������
					CString strAsmConsInfo, strCompConsInfo;
					ProGeomitem goitem;
					ProName featName;//Լ�����ƣ���δ��ȡ
					int nId;
					status = ProAsmcompconstraintAsmreferenceGet(constr_array[j], &asmConstr, &dtmside);
					status = ProSelectionAsmcomppathGet(asmConstr, &asmCompPath);///�����ѡ����������װ�����·��
					status = ProSelectionModelitemGet(asmConstr, &asmfeat);
					switch (asmfeat.type)
					{
					case PRO_AXIS:
					{
						ProAxis axis;
						status = ProAxisInit((ProSolid)asmfeat.owner, asmfeat.id, &axis); //!<��ȡ����Ϣ
						status = ProAxisToGeomitem((ProSolid)asmfeat.owner, axis, &goitem);//!<��ü�����Ϣ
						status = ProAxisIdGet(axis, &nId);
					}
					break;
					case PRO_SURFACE:
					{
						ProSurface surface;
						status = ProSurfaceInit((ProSolid)asmfeat.owner, asmfeat.id, &surface);//!<��ȡƽ��
						status = ProSurfaceToGeomitem((ProSolid)asmfeat.owner, surface, &goitem);//!<��ü�����Ϣ
						status = ProSurfaceIdGet(surface, &nId);
					}
					break;
					case PRO_EDGE:
					case PRO_POINT:
					case PRO_CSYS:
					case PRO_EDGE_START:
					case PRO_EDGE_END:
					{
						ProEdge edge;
						status = ProEdgeInit((ProSolid)asmfeat.owner, asmfeat.id, &edge);//!<��ȡ��
						status = ProEdgeToGeomitem((ProSolid)asmfeat.owner, edge, &goitem);//!<��ü�����Ϣ
						status = ProEdgeIdGet(edge, &nId);
					}
					break;
					default:
						break;
					}
					strAsmConsInfo.Format("%s %d", FeatureNameGet(asmfeat.type), nId);


					//��ȡ���װ��������Ϣ
					status = ProAsmcompconstraintCompreferenceGet(constr_array[j], &compConstr, &dtmside);
					status = ProSelectionAsmcomppathGet(compConstr, &compCompPath);///�����ѡ����������װ�����·��
					status = ProSelectionModelitemGet(compConstr, &comfeat);
					switch (comfeat.type)
					{
					case PRO_AXIS:
					{
						ProAxis axis;
						status = ProAxisInit((ProSolid)comfeat.owner, comfeat.id, &axis); //!<��ȡ����Ϣ
						status = ProAxisToGeomitem((ProSolid)comfeat.owner, axis, &goitem);//!<��ü�����Ϣ
						status = ProAxisIdGet(axis, &nId);
					}
					break;
					case PRO_SURFACE:
					{
						ProSurface surface;
						status = ProSurfaceInit((ProSolid)comfeat.owner, comfeat.id, &surface);//!<��ȡƽ��
						status = ProSurfaceToGeomitem((ProSolid)comfeat.owner, surface, &goitem);//!<��ü�����Ϣ
						status = ProSurfaceIdGet(surface, &nId);
					}
					break;
					case PRO_EDGE:
					case PRO_POINT:
					case PRO_CSYS:
					case PRO_EDGE_START:
					case PRO_EDGE_END:
					{
						ProEdge edge;
						status = ProEdgeInit((ProSolid)comfeat.owner, comfeat.id, &edge);//!<��ȡ��
						status = ProEdgeToGeomitem((ProSolid)comfeat.owner, edge, &goitem);//!<��ü�����Ϣ
						status = ProEdgeIdGet(edge, &nId);
					}
					break;
					default:
						break;
					}
					strCompConsInfo.Format("%s %d", FeatureNameGet(comfeat.type), nId);
					
					strConstrFeatInfo.Format("%s(%s,%s)", strConstrFeatInfo, strAsmConsInfo, strCompConsInfo);

				}
				ProArrayFree((ProArray*)&constr_array);
				//��ʾ���ؼ���
				CString strIndex;
				int nRowCount = grid1.GetItemCount();
				strIndex.Format("%d", nRowCount + 1);
				//�б�ؼ�1��ʾ
				grid1.InsertItem(nRowCount, strIndex, mdlType == PRO_MDL_ASSEMBLY ? 0 : 1);
				grid1.SetItemText(nRowCount, 1, strMdlNameWithID);//ģ������
				grid1.SetItemText(nRowCount, 2, strConstrType);//Լ����Ϣ
				grid1.SetItemText(nRowCount, 3, strConstrFeatInfo);//Լ������
			}
		}
	}


	namespace ProOpenMdl
	{
		static struct str_to_type
		{
			char *str;
			ProType type;
		};
		str_to_type	str_type_table[] =
		{
			{ "sec", PRO_2DSECTION },
			{ "prt", PRO_PART },
			{ "asm", PRO_ASSEMBLY },
			{ "drw", PRO_DRAWING },
			{ "mfg", PRO_MFG },
			{ "lay", PRO_LAYOUT },
			{ "frm", PRO_DWGFORM },
			{ "rep", PRO_REPORT },
			{ "mrk", PRO_MARKUP },
			{ "dgm", PRO_DIAGRAM },
			{ "???", PRO_TYPE_INVALID }
		};

		ProError UserOpenComponent(ProSolid* solid)///�����
		{
			ProSolid comp;
			ProError err, er1;
			int version;
			ProCharLine line, str;
			ProPath *path_arr, sel_path, def_path, cur_path;
			ProName *path_lab_arr, w_name, w_type;
			ProType type;

			ProStringToWstring(def_path, ".");

			err = ProArrayAlloc(0, sizeof(ProPath), 1, (ProArray*)&path_arr);
			err = ProArrayAlloc(0, sizeof(ProPath), 1, (ProArray*)&path_lab_arr);

			err = ProFileOpen(NULL, L"*.prt,*.asm,*.cem", path_arr, path_lab_arr, def_path, NULL, sel_path);////���ļ��Ի��� 
			if (err == PRO_TK_NO_ERROR)
			{
				err = ProFilenameParse(sel_path, def_path, w_name, w_type, &version);////�����ļ���      
																					 //ProUtilWstrcpy(sel_path, def_path);
				ProWstringCopy(sel_path, def_path, PRO_VALUE_UNUSED);
				err = ProPathCreate(NULL, w_name, w_type, PRO_VALUE_UNUSED, def_path);////����·��       
				ProWstringToString(str, def_path);////��Wstring��·��ת��ΪString��
				if (ProUtilConfirmNameType(str, line, &type) != PRO_TK_BAD_INPUTS)
				{
					err = ProDirectoryCurrentGet(cur_path); ////��õ�ǰ·�����       
					err = ProDirectoryChange(sel_path); ////�ı�·��      
					err = ProMdlRetrieve(w_name, (ProMdlType)type, (ProMdl*)&comp);////�������
					err = ProDirectoryChange(cur_path); ////��ԭ·��           
					if (err != PRO_TK_NO_ERROR)
					{

					}
				}
			}
			er1 = ProArrayFree((ProArray*)&path_arr);
			er1 = ProArrayFree((ProArray*)&path_lab_arr);
			if (err != PRO_TK_NO_ERROR)
				return (PRO_TK_BAD_CONTEXT);
			*solid = comp;///////�������

			return PRO_TK_NO_ERROR;
		}

		int ProUtilConfirmNameType(char *input_name, char *name, ProType *type)
		{
			char *dot_occur,
				*slash_occur,
				type_str[PRO_EXTENSION_SIZE];
			int name_length,
				type_length;

			if ((slash_occur = strrchr(input_name, '/')) != NULL)
				strcpy(input_name, &slash_occur[1]);

			if ((dot_occur = strchr(input_name, '.')) != NULL)
			{
				if ((name_length = strlen(input_name) - strlen(dot_occur)) > 0)
				{
					strncpy(name, input_name, name_length);
					name[name_length] = '\0';
				}
				else
					return(PRO_TK_BAD_INPUTS);

				if ((type_length = strlen(&dot_occur[1])) == 3)
				{
					strncpy(type_str, &dot_occur[1], type_length);
					type_str[3] = '\0';

					return((*type = ProUtilGetProType(type_str)));
				}
				else
					return(PRO_TK_BAD_INPUTS);
			}
			else
				return(PRO_TK_BAD_INPUTS);
		}

		ProType ProUtilGetProType(char *type_str)
		{
			int table_size;
			int i;
			table_size = sizeof(str_type_table) / sizeof(struct str_to_type);
			for (i = 0; i < table_size - 1; i++)
			{
				if (!ProUtilStrcmp(type_str, str_type_table[i].str))
					return(str_type_table[i].type);
			}
			return(str_type_table[table_size - 1].type);
		}

		int ProUtilStrcmp(char *s, char *t)
		{
			int i = 0;
			while (toupper(s[i]) == toupper(t[i]))
			{
				if (s[i++] == '\0')
					return(0);
			}
			return(s[i] - t[i]);
		}
	}
#endif
}
