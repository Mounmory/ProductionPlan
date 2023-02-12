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
		int pos = day.Find("月");
		yue = atoi(day.Left(pos));
		day = day.Right(day.GetLength() - pos - 2);
		pos = day.Find("日");
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

	//获取工作目录
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
	/* 激活当前窗口                                                         */
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
	/* 重绘窗口                                                         */
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
	/* 获取当前模型                                                */
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
	/* 把模型下面的元件添加到树节点下面,带特征（这里面用到了树控件）              */
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

		//访问实体模型solid的所有特征
		status = ProSolidFeatVisit(solid, (ProFeatureVisitAction)FeatureAsmVisitAcFnWithFeature
			, NULL, (CArray<ProFeature, ProFeature>*)&feat_list);
		int num = (int)feat_list.GetCount();

		//循环获得每个特征的类型，并判断类型是否为元件
		for (int i = 0; i < num; i++)
		{
			status = ProFeatureTypeGet(&feat_list[i], &ftype);

			//如果特征的类型为元件，则把元件的文件名添加插入到树节点下面
			if (ftype == PRO_FEAT_COMPONENT) {
				ProSolid              mdl;
				status = ProAsmcompMdlGet((ProAsmcomp *)(&feat_list[i]), (ProMdl *)&mdl);
				status = ProMdlNameGet(mdl, wName);
				csName = CString(wName);
				status = ProMdlTypeGet(mdl, &type);
				//获得元件的类型（装配件或零件），并在名称后面添加文件扩展符
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
				//子装配件下面,这里是一个递归运算过程
				ShowModelTreeWithFeatrue(mdl, m_Tree, tempItem, p_treeitem);
			}
			//下面的过程可以把元件（零件）内的其它类型特征添加到零件节点下面
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
					//把这个项添加到数组中，并把记录好作为树项的隐藏数字
					(int)p_treeitem.Add(feat_list[i]),
					//对元件内进一步判断，如果这个元件是零件，即没有元件类型特征
					hItem, NULL);
			}
		}
	}

	/************************************************************************/
	/* 把模型下面的元件添加到树节点下面（这里面用到了树控件）              */
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

		//访问实体模型solid的所有特征
		status = ProSolidFeatVisit(solid, (ProFeatureVisitAction)FeatureAsmVisitAcFnWhithoutFeature
			, NULL, (CArray<ProFeature, ProFeature>*)&feat_list);
		int num = (int)feat_list.GetCount();
		//循环获得每个特征的类型，并判断类型是否为元件
		for (int i = 0; i < num; i++)
		{
			status = ProFeatureTypeGet(&feat_list[i], &ftype);
			//如果特征的类型为元件，则把元件的文件名添加插入到树节点下面
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
					//子装配件下面,这里是一个递归运算过程
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
	/* 模型特征访问动作函数                                                 */
	/************************************************************************/
	ProError FeatureAsmVisitAcFnWithFeature(ProFeature *p_object, ProError status,
		CArray<ProFeature, ProFeature> * app_data)

	{
		ProBoolean     isVisible;
		int         *  r_failed_feat_ids;
		int            num = 0;
		ProBoolean     p_is_incomplete;
		ProFeatStatus  p_status;
		//特征过滤   
		//特征失败
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
		//特征不完整的
		status = ProFeatureIsIncomplete(p_object, &p_is_incomplete);
		if (p_is_incomplete != PRO_B_FALSE) return PRO_TK_NO_ERROR;
		//特征没有被激活或特征被隐含
		status = ProFeatureStatusGet(p_object, &p_status);
		if (p_status == PRO_FEAT_SUPPRESSED) return PRO_TK_NO_ERROR;
		if (p_status != PRO_FEAT_ACTIVE)    return PRO_TK_NO_ERROR;
		//特征可见
		status = ProFeatureVisibilityGet(p_object, &isVisible);
		if (isVisible == PRO_B_TRUE) {
			app_data->Add(*p_object);
		}

		return PRO_TK_NO_ERROR;
	}

	/************************************************************************/
	/* 模型特征访问动作函数                                                 */
	/************************************************************************/
	ProError FeatureAsmVisitAcFnWhithoutFeature(ProFeature *p_object, ProError status,
		CArray<ProFeature, ProFeature> * app_data)

	{
		ProBoolean     isVisible;
		int         *  r_failed_feat_ids;
		int            num = 0;
		ProBoolean     p_is_incomplete;
		ProFeatStatus  p_status;
		//特征过滤   
		//特征失败
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
		//特征不完整的
		status = ProFeatureIsIncomplete(p_object, &p_is_incomplete);
		if (p_is_incomplete != PRO_B_FALSE) return PRO_TK_NO_ERROR;
		//特征没有被激活或特征被隐含
		status = ProFeatureStatusGet(p_object, &p_status);
		if (p_status == PRO_FEAT_SUPPRESSED) return PRO_TK_NO_ERROR;
		if (p_status != PRO_FEAT_ACTIVE)    return PRO_TK_NO_ERROR;
		//特征可见
		status = ProFeatureVisibilityGet(p_object, &isVisible);
		if (isVisible == PRO_B_TRUE) {
			app_data->Add(*p_object);
		}

		return PRO_TK_NO_ERROR;
	}

	/************************************************************************/
	/* 实体特征访问动作函数                                                 */
	/************************************************************************/
	ProError UsrPartFeatVisFn(ProFeature* p_feature, ProError status,
		CArray<ProFeature, ProFeature> * app_data)
	{
		ProBoolean     isVisible;
		int         *  r_failed_feat_ids;
		int            num = 0;
		ProBoolean     p_is_incomplete;
		ProFeatStatus  p_status;
		//特征过滤   
		//1.特征失败
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
		//2.特征不完整的
		status = ProFeatureIsIncomplete(p_feature, &p_is_incomplete);
		if (p_is_incomplete != PRO_B_FALSE) return PRO_TK_NO_ERROR;
		//3.特征没有被激活或特征被隐含
		status = ProFeatureStatusGet(p_feature, &p_status);
		if (p_status == PRO_FEAT_SUPPRESSED) return PRO_TK_NO_ERROR;
		if (p_status != PRO_FEAT_ACTIVE)    return PRO_TK_NO_ERROR;
		//4.特征可见
		status = ProFeatureVisibilityGet(p_feature, &isVisible);
		if (isVisible == PRO_B_TRUE) {
			app_data->Add(*p_feature);
		}
		return PRO_TK_NO_ERROR;
	}

	static struct str_to_Feattype//特征类型
	{
		char *str;
		ProType type;
	}str_type_feat_table[] =
	{
		{ "轴", PRO_AXIS },
		{ "面", PRO_SURFACE },
		{ "边", PRO_EDGE },
		{ "点", PRO_POINT },
		{ "坐标", PRO_CSYS },
		{ "边起点", PRO_EDGE_START },
		{ "边终点", PRO_EDGE_END },
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
		{ "匹配", PRO_ASM_MATE },
		{ "匹配偏距", PRO_ASM_MATE_OFF },
		{ "对齐", PRO_ASM_ALIGN },
		{ "对齐偏距", PRO_ASM_ALIGN_OFF },
		{ "插入", PRO_ASM_INSERT },
		{ "定向", PRO_ASM_ORIENT },
		{ "坐标系", PRO_ASM_CSYS },
		{ "相切", PRO_ASM_TANGENT },
		{ "曲面上点", PRO_ASM_PNT_ON_SRF },
		{ "曲面上边", PRO_ASM_EDGE_ON_SRF },
		{ "默认", PRO_ASM_DEF_PLACEMENT },
		{ "替换", PRO_ASM_SUBSTITUTE },
		{ "线上点", PRO_ASM_PNT_ON_LINE },
		{ "固定", PRO_ASM_FIX },
		{ "自动", PRO_ASM_AUTO },
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

	void AsmcompConstraintsToList(ProSolid solid, CListCtrl &grid1, CListCtrl &grid2)//另外一种过滤特征的方法
	{
		ProError				status;
		CArray<ProFeature, ProFeature> feat_list;

		//访问实体模型solid的所有特征
		status = ProSolidFeatVisit(solid, (ProFeatureVisitAction)FeatureAsmVisitAcFnWithFeature
			, NULL, (CArray<ProFeature, ProFeature>*)&feat_list);
		int num = (int)feat_list.GetCount();

		//循环获得每个特征的类型，并判断类型是否为元件
		for (int i = 0; i < num; i++)
		{
			ProFeattype				featType;
			status = ProFeatureTypeGet(&feat_list[i], &featType);
			if (featType == PRO_FEAT_COMPONENT) //只获取组件的装配关系
			{
				CString strMdlName,strMdlNameWithID;//模型名称
				CString strConstrType, strConstrFeatInfo, strConstrState, strMdlPath;//约束信息
				CString strMatrix;//位姿矩阵

				ProSolid		mdl;
				ProName			mdlName;
				ProMdlType		mdlType;

				ProAsmcomp		asmcomp(*(ProAsmcomp *)(&feat_list[i]));
				status = ProAsmcompMdlGet(&asmcomp, (ProMdl *)&mdl);
				status = ProMdlNameGet(mdl, mdlName);
				status = ProMdlTypeGet(mdl, &mdlType);
				//获取模型名称
				strMdlName.Format("%s.%s", CString(mdlName), mdlType == PRO_MDL_ASSEMBLY ?"ASM":"PRT");
				strMdlNameWithID.Format("%s.%s(%d)", CString(mdlName), mdlType == PRO_MDL_ASSEMBLY ? "ASM" : "PRT",asmcomp.id);
				//获取模型约束
				ProAsmcompconstraint* constr_array;

				ProSelection asmConstr, compConstr;
				ProDatumside dtmside;
				ProModelitem asmfeat, comfeat;
				ProAsmcomppath asmCompPath,compCompPath;//带有完整装配信息路径

				int nSize = 0;
				status = ProAsmcompConstraintsGet(&asmcomp, &constr_array);
				if (status != PRO_TK_NO_ERROR)
					return;
				ProArraySizeGet(constr_array, &nSize);
				for (int j = 0; j < nSize; j++)
				{

					////获取约束类型和约束值
					//ProAsmcompConstrType asmConsYype;
					//CString strTemConsValue, strTemConsType;
					//double fOffset(0);
					//status = ProAsmcompconstraintOffsetGet(constr_array[i], &fOffset);//获得装配元件约束的偏置值
					//status == PRO_TK_NO_ERROR ? strTemConsValue.Format("偏距值 = %6.3lf", fOffset) : strTemConsValue = "";
					//status = ProAsmcompconstraintTypeGet(constr_array[i], &asmConsYype);//获得装配元件约束的类型
					//ConstraintsNameGet(asmConsYype);//调用UserConstraintTypeToString函数
					//strConstrType.Format("%s(%s,%s)", strConstrType, ConstraintsNameGet(asmConsYype), strTemConsValue);

					//获取约束特征
					CString strAsmConsInfo, strCompConsInfo;
					ProGeomitem goitem;
					ProName featName;//约束名称，暂未获取
					int nId;
					status = ProAsmcompconstraintAsmreferenceGet(constr_array[j], &asmConstr, &dtmside);
					status = ProSelectionAsmcomppathGet(asmConstr, &asmCompPath);///获得所选择的零件或子装配体的路径
					status = ProSelectionModelitemGet(asmConstr, &asmfeat);
					switch (asmfeat.type)
					{
					case PRO_AXIS:
					{
						ProAxis axis;
						status = ProAxisInit((ProSolid)asmfeat.owner, asmfeat.id, &axis); //!<获取轴信息
						status = ProAxisToGeomitem((ProSolid)asmfeat.owner, axis, &goitem);//!<获得几何信息
						status = ProAxisIdGet(axis, &nId);
					}
					break;
					case PRO_SURFACE:
					{
						ProSurface surface;
						status = ProSurfaceInit((ProSolid)asmfeat.owner, asmfeat.id, &surface);//!<获取平面
						status = ProSurfaceToGeomitem((ProSolid)asmfeat.owner, surface, &goitem);//!<获得几何信息
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
						status = ProEdgeInit((ProSolid)asmfeat.owner, asmfeat.id, &edge);//!<获取边
						status = ProEdgeToGeomitem((ProSolid)asmfeat.owner, edge, &goitem);//!<获得几何信息
						status = ProEdgeIdGet(edge, &nId);
					}
					break;
					default:
						break;
					}
					strAsmConsInfo.Format("%s %d", FeatureNameGet(asmfeat.type), nId);


					//获取组件装配特征信息
					status = ProAsmcompconstraintCompreferenceGet(constr_array[j], &compConstr, &dtmside);
					status = ProSelectionAsmcomppathGet(compConstr, &compCompPath);///获得所选择的零件或子装配体的路径
					status = ProSelectionModelitemGet(compConstr, &comfeat);
					switch (comfeat.type)
					{
					case PRO_AXIS:
					{
						ProAxis axis;
						status = ProAxisInit((ProSolid)comfeat.owner, comfeat.id, &axis); //!<获取轴信息
						status = ProAxisToGeomitem((ProSolid)comfeat.owner, axis, &goitem);//!<获得几何信息
						status = ProAxisIdGet(axis, &nId);
					}
					break;
					case PRO_SURFACE:
					{
						ProSurface surface;
						status = ProSurfaceInit((ProSolid)comfeat.owner, comfeat.id, &surface);//!<获取平面
						status = ProSurfaceToGeomitem((ProSolid)comfeat.owner, surface, &goitem);//!<获得几何信息
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
						status = ProEdgeInit((ProSolid)comfeat.owner, comfeat.id, &edge);//!<获取边
						status = ProEdgeToGeomitem((ProSolid)comfeat.owner, edge, &goitem);//!<获得几何信息
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
				//显示到控件中
				CString strIndex;
				int nRowCount = grid1.GetItemCount();
				strIndex.Format("%d", nRowCount + 1);
				//列表控件1显示
				grid1.InsertItem(nRowCount, strIndex, mdlType == PRO_MDL_ASSEMBLY ? 0 : 1);
				grid1.SetItemText(nRowCount, 1, strMdlNameWithID);//模型名称
				grid1.SetItemText(nRowCount, 2, strConstrType);//约束信息
				grid1.SetItemText(nRowCount, 3, strConstrFeatInfo);//约束特征
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

		ProError UserOpenComponent(ProSolid* solid)///打开组件
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

			err = ProFileOpen(NULL, L"*.prt,*.asm,*.cem", path_arr, path_lab_arr, def_path, NULL, sel_path);////打开文件对话框 
			if (err == PRO_TK_NO_ERROR)
			{
				err = ProFilenameParse(sel_path, def_path, w_name, w_type, &version);////解析文件名      
																					 //ProUtilWstrcpy(sel_path, def_path);
				ProWstringCopy(sel_path, def_path, PRO_VALUE_UNUSED);
				err = ProPathCreate(NULL, w_name, w_type, PRO_VALUE_UNUSED, def_path);////生成路径       
				ProWstringToString(str, def_path);////将Wstring型路径转换为String型
				if (ProUtilConfirmNameType(str, line, &type) != PRO_TK_BAD_INPUTS)
				{
					err = ProDirectoryCurrentGet(cur_path); ////获得当前路径零件       
					err = ProDirectoryChange(sel_path); ////改变路径      
					err = ProMdlRetrieve(w_name, (ProMdlType)type, (ProMdl*)&comp);////检索零件
					err = ProDirectoryChange(cur_path); ////还原路径           
					if (err != PRO_TK_NO_ERROR)
					{

					}
				}
			}
			er1 = ProArrayFree((ProArray*)&path_arr);
			er1 = ProArrayFree((ProArray*)&path_lab_arr);
			if (err != PRO_TK_NO_ERROR)
				return (PRO_TK_BAD_CONTEXT);
			*solid = comp;///////返回零件

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
