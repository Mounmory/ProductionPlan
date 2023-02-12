// TechnolProcePrepareDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolProcePrepareDlg.h"
#include "TechnolProceDBNameSaveDlg.h"

#include "afxdialogex.h"

static std::map<uint32_t, STRING> g_mapDealResult;//<key:cmdID,value:strCmd>根据执行结果进行下一个处理
static const STRING updateProcTable = "UpCom2";//更新列表控件2
//设置状态栏
static StatusData g_statuData;

// CTechnolProcePrepareDlg 对话框

IMPLEMENT_DYNAMIC(CTechnolProcePrepareDlg, CDialog)

CTechnolProcePrepareDlg::CTechnolProcePrepareDlg(std::shared_ptr<DBService> dbService, CWnd* pParent /*=NULL*/)
	: CDialog(CTechnolProcePrepareDlg::IDD, pParent)
	, m_dbService(getProPlanDBService())
	, m_fWorkTime(0)
{
	m_dbService->addListener(this);
}

CTechnolProcePrepareDlg::~CTechnolProcePrepareDlg()
{
	m_dbService->removeListener(this);
}

void CTechnolProcePrepareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
#ifdef PRO_USE_VAR_ARGS
	DDX_Control(pDX, IDC_PVIEWCTRL1, m_CDprivew);//Creo模型预览控件
#endif
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Control(pDX, IDC_LIST1, m_grid1);
	DDX_Control(pDX, IDC_LIST2, m_grid2);
	DDX_Control(pDX, IDC_COMBO1, m_combo1);
	DDX_Control(pDX, IDC_COMBO2, m_combo2);
	DDX_Text(pDX, IDC_EDIT6, m_fWorkTime);
}


BEGIN_MESSAGE_MAP(CTechnolProcePrepareDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SAVE_TO_BD, &CTechnolProcePrepareDlg::OnClickedBtnSaveList1ToBd)
	ON_BN_CLICKED(IDC_BTN_SHOW_DB, &CTechnolProcePrepareDlg::OnBnClickedBtnViewTableListInDB)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CTechnolProcePrepareDlg::OnDblclkList1)
	ON_BN_CLICKED(IDC_BTN_ADD_PROGR, &CTechnolProcePrepareDlg::OnBnClickedBtnAddSingleProgrToList2)
	ON_BN_CLICKED(IDC_BTN_ADD_TO_LIST1, &CTechnolProcePrepareDlg::OnBnClickedBtnAddEditProcToList1)
	ON_BN_CLICKED(IDC_BTN_ADD_PRO_TO_TABLE, &CTechnolProcePrepareDlg::OnBnClickedBtnShowProcessInfoFromDBToTable)
	ON_BN_CLICKED(IDC_BTN_DEL_PROGR, &CTechnolProcePrepareDlg::OnBnClickedBtnDelProgInList2)
	ON_BN_CLICKED(IDC_BTN_DELE_PROG_IN_LISET2, &CTechnolProcePrepareDlg::OnBnClickedBtnDeleSelectedProgInLiset2)
	ON_BN_CLICKED(IDC_BTN_UPER, &CTechnolProcePrepareDlg::OnBnClickedBtnMoveUperInList2)
	ON_BN_CLICKED(IDC_BTN_DOWN, &CTechnolProcePrepareDlg::OnBnClickedBtnMoveDownInList2)
END_MESSAGE_MAP()

// CTechnolProcePrepareDlg 消息处理程序
void CTechnolProcePrepareDlg::dealDBResult(std::queue<DBResult>& rst)
{
	LOGDEBUG("[%s][%d] begin, result size %d !", __STRING_FUNCTION__, __LINE__, rst.size());

	while (rst.size() > 0)
	{
		auto iterRst = rst.front();
		if (g_mapDealResult.find(iterRst.cmdID) != g_mapDealResult.end())
		{
			if (updateProcTable == g_mapDealResult[iterRst.cmdID]) //更新组合框控件2
			{
				ShowProcessTableListToCombo2();
				//设置状态栏
				CString strStatus;
				strStatus.Format("添加表格 %s ！数据库名称 %s 表格名称 %s 。"
					, iterRst.bResult ? "成功" : "失败"
					, iterRst.strDBName
					, iterRst.strTableName);
				m_StatusBar.SetPaneText(1, strStatus);
				//显示设置
				g_statuData.strInfo = strStatus;
				g_statuData.nTimeCount = 15;//显示15s
			}
			else
			{
			}
			g_mapDealResult.erase(iterRst.cmdID);
		}
		rst.pop();
	}
	
	
}

BOOL CTechnolProcePrepareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  在此添加额外的初始化
	////设置对话框图标
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CREO);//IDR_ICON为图标资源名
	SetIcon(m_hIcon, FALSE);
	///////////设置树控件节点图标
	m_ImageList.Create(14, 14, ILC_COLOR24|ILC_MASK, 1, 1);
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_ASM)));
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PRT)));
//	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_PRT)));
	m_Tree.SetImageList(&m_ImageList, TVSIL_NORMAL);

	//设置状态栏
	UINT array[8];
	for(int i=0;i<8;i++)
	{
		array[i] = 1001 + i;
	}
	m_StatusBar.Create(this); //创建状态栏窗口
	m_StatusBar.SetIndicators(array,sizeof(array)/sizeof(UINT)); //添加面板
	m_StatusBar.SetPaneInfo(0,array[0],0,70); //设置面板宽度
	m_StatusBar.SetPaneInfo(1,array[1],0,600); //设置面板宽度
	m_StatusBar.SetPaneInfo(2,array[2],0,150); //
	m_StatusBar.SetPaneInfo(3,array[3],0,150); 
	m_StatusBar.SetPaneInfo(4,array[4],0,150); //
	m_StatusBar.SetPaneInfo(5,array[5],0,150); 

	CTime Time = CTime::GetCurrentTime();
	m_StatusBar.SetPaneText(0,"状态：");//设置面板文本
	m_StatusBar.SetPaneText(1,"");//设置面板文本
	m_StatusBar.SetPaneText(2,"");//设置面板文本
	m_StatusBar.SetPaneText(3,"");//设置面板文本
	m_StatusBar.SetPaneText(4,Time.Format("%Y-%m-%d"));
	m_StatusBar.SetPaneText(5,Time.Format("%H:%M:%S"));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
	SetTimer(1,1000,NULL); //设置定时器	

#ifdef PRO_USE_VAR_ARGS
	//把装配件的名称作为树控件根节点
	ProName     name;
	ProSolid asm_solid;
	asm_solid=(ProSolid)GlobalFunction::GetCurrentMdl();
	ProMdlNameGet(asm_solid,name);
	m_Tree.InsertItem(CString(name).MakeLower()+".asm",0,0,TVI_ROOT);
	p_asmfeatlist.RemoveAll();	
	GlobalFunction::ShowModelTreeWhithoutFeature(asm_solid,m_Tree,m_Tree.GetRootItem(),p_asmfeatlist);
    m_Tree.Expand(m_Tree.GetRootItem(),TVE_EXPAND);
	////将当前模型显示到Preview控件
	ProMdl		mdl;
	ProMdldata	data;
	ProError	status;
	status = ProMdlCurrentGet(&mdl);
	/////获取路径
	if (ProMdlDataGet(mdl, &data) == PRO_TK_NO_ERROR)
	{
		m_strModelPath = CString(data.path);
		if (CString(data.device) != "")
		{
			m_strModelPath = CString(data.device) + ":" + m_strModelPath;
		}
	}
	//获取模型名称
	m_strRootMdlName = name;
#endif

	//设置列表控件
	m_grid1.InsertColumn(0,_T("序号"),LVCFMT_LEFT,40,0);
	m_grid1.InsertColumn(1,_T("层级"),LVCFMT_LEFT,60,0);
	m_grid1.InsertColumn(2,_T("物料编号"),LVCFMT_LEFT,100,0);
	m_grid1.InsertColumn(3,_T("物料名称"),LVCFMT_LEFT,100,0);
	m_grid1.InsertColumn(4,_T("模型名称"),LVCFMT_LEFT,100,0);
	m_grid1.InsertColumn(5,_T("类型"),LVCFMT_LEFT,40,0);
	m_grid1.InsertColumn(6,_T("数量"),LVCFMT_LEFT,40,0);
	m_grid1.InsertColumn(7,_T("父装配体"),LVCFMT_LEFT,150,0);
	m_grid1.InsertColumn(8,_T("制造流程"),LVCFMT_LEFT,150,0);
	m_grid1.InsertColumn(9,_T("工时(h)"),LVCFMT_LEFT,150,0);
	m_grid1.InsertColumn(10,_T("合计(h)"),LVCFMT_LEFT,50,0);
	m_grid1.InsertColumn(11,_T("流程日期"),LVCFMT_LEFT,75,0);
	m_grid1.InsertColumn(12,_T("备注"),LVCFMT_LEFT,150,0);
	m_grid1.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	//设置列表控件2标题
	m_grid2.InsertColumn(0,_T("序号"),LVCFMT_LEFT,40,0);
	m_grid2.InsertColumn(1,_T("工位"),LVCFMT_LEFT,85,0);
	m_grid2.InsertColumn(2,_T("工时(h)"),LVCFMT_LEFT,50,0);
	m_grid2.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);
	//显示物料明细
	ShowMdlTreeToBillOfMaterial();
	//组合框控件显示工位
	ShowWorkplaceToCombo1();
	ShowProcessTableListToCombo2();

	return TRUE; 
}

//将模型树结构显示道树控件
void CTechnolProcePrepareDlg::ShowMdlTreeToBillOfMaterial(void)
{
#ifdef PRO_USE_VAR_ARGS
	m_grid1.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_grid1.DeleteAllItems();
	m_vecStrParents.clear();
	////插入第一行（根目录）
	HTREEITEM root=m_Tree.GetRootItem();
	CString Asmname,mdlname,mdltype;
	Asmname=m_Tree.GetItemText(root);
	int pos=Asmname.Find(".");
	mdlname=Asmname.Left(pos);
	mdltype=Asmname.Right(Asmname.GetLength()-pos-1);
	m_grid1.InsertItem(0,"1",0);/////设置图标asm
	m_grid1.SetItemText(0,1,"0");
	m_grid1.SetItemText(0,4,mdlname);
	m_grid1.SetItemText(0,5,mdltype);
	m_grid1.SetItemText(0,6,"1");
//	m_grid1.SetItemText(0,7,"null");
	m_vecStrParents.push_back(Asmname);
	////插入子目录
	ShowChildTreeList(root);
#endif
}

void CTechnolProcePrepareDlg::ShowChildTreeList(HTREEITEM Hitem)
{
#ifdef PRO_USE_VAR_ARGS
	HTREEITEM hchild=m_Tree.GetChildItem(Hitem);
	while(hchild!=NULL)
	{
		HTREEITEM fuxiang=m_Tree.GetParentItem(Hitem);
		CString strParent = m_Tree.GetItemText(hchild);//记录父项存储到表格1中
		CString strFullName = strParent;//用于获取模型到根节点的全路径
		int pos1=strFullName.Find(".");
		CString mdlname=strFullName.Left(pos1);//模型名称
		CString mdltype=strFullName.Mid(pos1+1,3);//模型类型;

		/////判断层级
		HTREEITEM hRoot=m_Tree.GetRootItem();
		CString strLevel = "   ";
		int nLevel = 1;
		HTREEITEM hGetParent = Hitem;//获取父项
		strFullName = m_Tree.GetItemText(hGetParent) + strFullName;//累加各父项名称
		while (hGetParent != hRoot)
		{
			//获取累计父级
			strFullName = m_Tree.GetItemText(hGetParent) + strFullName;
			//获取层级
			hGetParent = m_Tree.GetParentItem(hGetParent);
			strLevel += "    ";
			nLevel++;
		}
		strLevel.Format("%s%d",strLevel,nLevel);
		//////判断零件是否已经在列表中
		int nVecSize = m_vecStrParents.size();
		bool bIsExist = false;//查模型时候否存在表格里了
		int nRowNum= 0 ;
		for (int i = 0 ; i < nVecSize ; ++i)
		{
			if (strFullName == m_vecStrParents.at(i))
			{
				nRowNum = i;//获取行号
				bIsExist = true;
				break;
			}
		}
		//如果表格中存在相同零件了，将表格中数量+1
		if (true == bIsExist)//
		{
			int num=atoi(m_grid1.GetItemText(nRowNum,6));
			num++;
			CString shuliang;
			shuliang.Format("%d",num);
			m_grid1.SetItemText(nRowNum,6,shuliang);
		}
		else
		{
			m_vecStrParents.push_back(strFullName);//名字存到vector

			nRowNum = m_grid1.GetItemCount();
			CString xuhao;
			xuhao.Format("%d",nRowNum+1);///序号列
			if(mdltype=="asm")/////设置图标
			{
				m_grid1.InsertItem(nRowNum,xuhao,0);	//图片索引号(第一幅图片)
			}
			else
			{
				m_grid1.InsertItem(nRowNum,xuhao,1);
			}
			m_grid1.SetItemText(nRowNum,1,strLevel);///层级
			m_grid1.SetItemText(nRowNum,4,mdlname);
			m_grid1.SetItemText(nRowNum,5,mdltype);
			m_grid1.SetItemText(nRowNum,6,"1");//数量
			m_grid1.SetItemText(nRowNum,7,strParent);//父项
		}
		ShowChildTreeList(hchild);//递归，查子
		hchild=m_Tree.GetNextItem(hchild,TVGN_NEXT);
	}
#endif
}

//将工位信息显示道列表控件2
void CTechnolProcePrepareDlg::ShowWorkplaceToCombo1(void)
{
	//获取工位信息表
	TableInfo temTable = (*m_dbService)["WorkPlaceInfo"]["WorkPlaceDef"];
	//获取行数
	size_t nRowCount = temTable.m_Values.getRowCount();
	//获取列索引
	size_t nCol = temTable.m_mapCols.at("WorkPlaceName");

	//显示道列表控件
	for (int i = nRowCount - 1; i >= 0; --i)
	{
		m_combo1.InsertString(0, temTable.m_Values[i][nCol]);
	}
	m_combo1.SetCurSel(-1);
}

//将工艺流程表显示到组合框控件2
void CTechnolProcePrepareDlg::ShowProcessTableListToCombo2(void)
{
	//清空控件内容
	m_combo2.ResetContent();
	//获取表格名称
	std::vector<STRING> temCols = (*m_dbService)["ProductionProcessInfo"].m_TableNames;

	auto iterCol = temCols.begin();
	size_t index = 0 ;
	while (iterCol != temCols.end())
	{
		m_combo2.InsertString(index++,*(iterCol++));//插入控件
	}
	m_combo2.SetCurSel(-1);
}

//查看数据库按钮
void CTechnolProcePrepareDlg::OnBnClickedBtnViewTableListInDB()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox("维护中……", "sorry", MB_OK);
}

//保存至数据库按钮
void CTechnolProcePrepareDlg::OnClickedBtnSaveList1ToBd()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_grid1.GetItemCount() == 0)
	{
		MessageBox("没有可保存的信息！", "错误", MB_ICONERROR);
		return;
	}

	//设置表格名称
	CTechnolProceDBNameSaveDlg dlg("ProductionProcessInfo");
	dlg.m_strTableName = m_strRootMdlName;
	if (dlg.DoModal() != IDOK)
	{
		return;
	}
	CString strTableName = dlg.m_strTableName;
	//获取表格数据
	int nColumuCount = 13;//列数量
	int nRowCount = m_grid1.GetItemCount();//获取写入的行数
	DynaArray2D<STRING> temValues(nRowCount, nColumuCount);
	for (int i = 0; i < nRowCount; ++i)
	{
		for (int j = 0; j < nColumuCount; ++j)
		{
			temValues[i][j] = m_grid1.GetItemText(i, j);
			//temValues = m_grid1.GetItemText(i, j + 1);
		}
	}
	//CString strSQL = "CREATE TABLE " + strTableName + "(Num INTEGER not null identity(1,1),	\
		ProcLevel TEXT, MertID TEXT, MaterialName TEXT, MdlName TEXT, MdlType TEXT, CountNum INTEGER, ParentMdl TEXT, \
		ProcInfo TEXT, WorkTime TEXT, CountTime TEXT, DateInfo TEXT, AttachInfo1 TEXT)";
	CString strSQL = "CREATE TABLE " + strTableName + "(Num TEXT ,	\
		ProcLevel TEXT,	MertID TEXT,MaterialName TEXT,MdlName TEXT,MdlType TEXT,CountNum TEXT,ParentMdl TEXT,	\
		ProcInfo TEXT,WorkTime TEXT,CountTime TEXT,DateInfo TEXT,AttachInfo1 TEXT)";

	//添加表格命令
	uint32_t cmdID = m_dbService->createTableCmd("ProductionProcessInfo", strTableName, strSQL, std::move(temValues));
	g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable));
	return;
}


void CTechnolProcePrepareDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CTime Time;
	Time = CTime::GetCurrentTime();
	m_StatusBar.SetPaneText(4,Time.Format("%Y-%m-%d"));
	m_StatusBar.SetPaneText(5,Time.Format("%H:%M:%S"));
	//操作状态
	if (g_statuData.nTimeCount > 0)
	{
		CString temStr = g_statuData.strInfo + "("+ std::to_string(g_statuData.nTimeCount--).c_str() +")";
		m_StatusBar.SetPaneText(1, temStr);
	}
	else 
	{
		m_StatusBar.SetPaneText(1, "");
	}
	CDialog::OnTimer(nIDEvent);
}

//双击列表控件进行编辑
void CTechnolProcePrepareDlg::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int i=m_grid1.GetSelectionMark();
	//如果没有选中，返回
	if(i==-1)
	{
		return;
	}
	m_grid2.DeleteAllItems();
	////获取选中行数据
	CString wuliaohao,woliaoming,moxingming,leixing,liucheng,gongshi,hejigongshi,beizhu;
	wuliaohao=m_grid1.GetItemText(i,2);///物料号
	if (wuliaohao == "")
	{
		wuliaohao.Format("%s-%03d",m_strRootMdlName,i + 1);
	}
	woliaoming=m_grid1.GetItemText(i,3);////物料名称
	moxingming=m_grid1.GetItemText(i,4);///模型名称
	leixing=m_grid1.GetItemText(i,5);////类型

	liucheng=m_grid1.GetItemText(i,8);//制造流程
	gongshi=m_grid1.GetItemText(i,9);//工时
	hejigongshi=m_grid1.GetItemText(i,10);//合计工时
	beizhu=m_grid1.GetItemText(i,12);//备注
	/////设置编辑框控件内容
	SetDlgItemText(IDC_EDIT1,wuliaohao);
	SetDlgItemText(IDC_EDIT2,woliaoming);
	SetDlgItemText(IDC_EDIT3,moxingming);
	SetDlgItemText(IDC_EDIT4,leixing);
	SetDlgItemText(IDC_EDIT5,hejigongshi);
	SetDlgItemText(IDC_EDIT7,beizhu);
	//设置工序框，工时框为空
	GetDlgItem(IDC_COMBO1)->SetWindowTextA("");
	GetDlgItem(IDC_EDIT6)->SetWindowTextA("");
	////设置列表控件二内容
	int j=0;
	liucheng=liucheng+"->";
	while(liucheng.GetLength()>4)
	{
		CString xuhao,gongxu;
		xuhao.Format("%d",j+1);
		m_grid2.InsertItem(j,xuhao);
		int posi=liucheng.Find("->");
		gongxu=liucheng.Left(posi);
		m_grid2.SetItemText(j,1,gongxu);
		liucheng=liucheng.Right(liucheng.GetLength()-posi-2);
		j++;
	}
	gongshi=gongshi+"+";
	while(j>0)//后续多注意下具体j的值
	{
		CString dangongshi;//单步工时
		int pos1=gongshi.Find("+");
		dangongshi=gongshi.Left(pos1);
		pos1++;
		gongshi=gongshi.Right(gongshi.GetLength()-pos1);
		m_grid2.SetItemText(m_grid2.GetItemCount()-j,2,dangongshi);
		j--;
	}
	///预览模型
#ifdef PRO_USE_VAR_ARGS
	CString strPath = m_strModelPath + moxingming+"."+leixing;
	m_CDprivew.ShowModelToCreoView(strPath);
#endif
	*pResult = 0;
}

//添加工序
void CTechnolProcePrepareDlg::OnBnClickedBtnAddSingleProgrToList2()
{
	UpdateData(true);
	CString gongwei,gongshi;
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(gongwei);
	GetDlgItem(IDC_EDIT6)->GetWindowTextA(gongshi);

	if (gongwei == "外购")
	{
		gongshi = "0";
		GetDlgItem(IDC_EDIT7)->SetWindowTextA(gongshi);
		m_grid2.DeleteAllItems();//清空工艺流程
	}
	else if(gongwei=="" || gongshi=="" || gongshi == "0")
	{
		MessageBox("工位或工时不能为空！","提示");
		return;
	}
	else
	{
		gongshi.Format("%5.2f", m_fWorkTime);
		int pos = gongshi.Find(".");
		CString checkvalue = gongshi.Right(gongshi.GetLength() - pos - 1);
		if (checkvalue.GetLength() > 2)
		{
			MessageBox("单个工序工时间精度为0.01小时", "提示");
			return;
		}
	}
	int i=m_grid2.GetItemCount();
	CString row(std::to_string(i + 1).c_str());
	row.Format("%d",i+1);
	m_grid2.InsertItem(i,row);
	m_grid2.SetItemText(i,1,gongwei);
	m_grid2.SetItemText(i,2,gongshi);
	////设置制造周期对话框
	float zong=0;
	for(int j=0;j<m_grid2.GetItemCount();j++)
	{
		float k=static_cast<float>(atof(m_grid2.GetItemText(j,2)));
		zong+=k;
	}
	CString total;
	total.Format("%5.2f",zong);
	GetDlgItem(IDC_EDIT5)->SetWindowTextA(total);
}

//更新编辑好的工艺流程到工艺流程表
void CTechnolProcePrepareDlg::OnBnClickedBtnAddEditProcToList1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString moxingming,moxingleixing,zonggongshi,beizhu;
	GetDlgItem(IDC_EDIT3)->GetWindowTextA(moxingming);//获取模型名
	GetDlgItem(IDC_EDIT4)->GetWindowTextA(moxingleixing);//获取模型类型

	if(moxingming=="")
	{
		MessageBox("没有可更新的内容！");
		return;
	}
	for(int i=0;i<m_grid1.GetItemCount();i++)
	{
		if(m_grid1.GetItemText(i,4)==moxingming && m_grid1.GetItemText(i,5)==moxingleixing)
		{
			CString bianghao,mingcheng,liucheng,gongshi;//物料编号、物料名称、流程和工时
			GetDlgItem(IDC_EDIT1)->GetWindowTextA(bianghao);//物料编号
			GetDlgItem(IDC_EDIT2)->GetWindowTextA(mingcheng);//物料名称
			GetDlgItem(IDC_EDIT5)->GetWindowTextA(zonggongshi);//总工时
			GetDlgItem(IDC_EDIT7)->GetWindowTextA(beizhu);//备注
			//总工时
			m_grid1.SetItemText(i,2,bianghao);//物料编号
			m_grid1.SetItemText(i,3,mingcheng);//物料名称
			m_grid1.SetItemText(i,10,zonggongshi);//总工时
			//制造流程
			for(int j=0;j<m_grid2.GetItemCount();j++)
			{
				liucheng+=m_grid2.GetItemText(j,1)+"->";
			}
			liucheng=liucheng.Left(liucheng.GetLength()-2);
			m_grid1.SetItemText(i,8,liucheng);
			//工时
			for(int k=0;k<m_grid2.GetItemCount();k++)
			{
				gongshi+=m_grid2.GetItemText(k,2)+"+";
			}
			gongshi=gongshi.Left(gongshi.GetLength()-1);
			m_grid1.SetItemText(i,9,gongshi);
			//设置日期
			CString timeStr;
			CTime day;
			day=CTime::GetCurrentTime();
			int filenameday,filenamemonth,filenameyear;
			filenameday=day.GetDay();
			filenamemonth=day.GetMonth();
			filenameyear=day.GetYear();
			timeStr.Format(_T("%04d-%02d-%02d"),filenameyear,filenamemonth,filenameday);
			m_grid1.SetItemText(i,11,timeStr);
			//设置备注
			m_grid1.SetItemText(i,12,beizhu);
		}
	}
}

//显示工艺流程表到列表控件1
void CTechnolProcePrepareDlg::OnBnClickedBtnShowProcessInfoFromDBToTable()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTableName;
	GetDlgItem(IDC_COMBO2)->GetWindowTextA(strTableName);//数据库中表格名称
	if ("" == strTableName)
	{
		return;
	}

	TableInfo temTable = (*m_dbService)["ProductionProcessInfo"][strTableName];
	int32_t nRowCount = temTable.m_Values.getRowCount();
	
#ifdef PRO_USE_VAR_ARGS
	 //构建模型索引
	std::map<CString, size_t> mapIndex;//<key:Modelname.type,value:index>
	for (size_t i = 0 ; i < nRowCount ; ++i)
	{
		CString strKey = temTable.m_Values[i][4] + "." + temTable.m_Values[i][5];//模型名称+类型
		mapIndex.insert(std::make_pair(strKey,i));
	}
	//依据列表中清单逐个查流程
	int nGridRowCount = m_grid1.GetItemCount();//需要更新的列表行数
	for (int i = 0; i < nGridRowCount; ++i)//循环读取添加到列表控件中
	{
		CString strMdlName = m_grid1.GetItemText(i, 4) + "." + m_grid1.GetItemText(i, 5);
		if (mapIndex.count(strMdlName) != 0)
		{
			size_t nIndex = mapIndex[strMdlName];
			m_grid1.SetItemText(i, 2, temTable.m_Values[nIndex][2]);
			m_grid1.SetItemText(i, 3, temTable.m_Values[nIndex][3]);
			m_grid1.SetItemText(i, 8, temTable.m_Values[nIndex][8]);
			m_grid1.SetItemText(i, 9, temTable.m_Values[nIndex][9]);
			m_grid1.SetItemText(i, 10, temTable.m_Values[nIndex][10]);
			m_grid1.SetItemText(i, 11, temTable.m_Values[nIndex][11]);
			m_grid1.SetItemText(i, 12, temTable.m_Values[nIndex][12]);
		}
	}
#else
	//将整个工艺流程表显示到列表中
	m_grid1.DeleteAllItems();
	for (int i = 0; i < nRowCount; ++i)
	{
		m_grid1.InsertItem(i, temTable.m_Values[i][0]);
		m_grid1.SetItemText(i, 1, temTable.m_Values[i][1]);
		m_grid1.SetItemText(i, 2, temTable.m_Values[i][2]);
		m_grid1.SetItemText(i, 3, temTable.m_Values[i][3]);
		m_grid1.SetItemText(i, 4, temTable.m_Values[i][4]);
		m_grid1.SetItemText(i, 5, temTable.m_Values[i][5]);
		m_grid1.SetItemText(i, 6, temTable.m_Values[i][6]);
		m_grid1.SetItemText(i, 7, temTable.m_Values[i][7]);
		m_grid1.SetItemText(i, 8, temTable.m_Values[i][8]);
		m_grid1.SetItemText(i, 9, temTable.m_Values[i][9]);
		m_grid1.SetItemText(i, 10, temTable.m_Values[i][10]);
		m_grid1.SetItemText(i, 11, temTable.m_Values[i][11]);
		m_grid1.SetItemText(i, 12, temTable.m_Values[i][12]);
	}
#endif
	//设置状态栏
	CString strStatus;
	strStatus.Format("读取工艺流程表 %s 成功", strTableName);
	m_StatusBar.SetPaneText(1, strStatus);
	g_statuData.strInfo = strStatus;
	g_statuData.nTimeCount = 15;
}

//列表控件2工艺流程编辑
void CTechnolProcePrepareDlg::OnBnClickedBtnDelProgInList2()
{
	// TODO: 在此添加控件通知处理程序代码
	m_grid2.DeleteAllItems();
	GetDlgItem(IDC_EDIT5)->SetWindowTextA("0.00");
}


void CTechnolProcePrepareDlg::OnBnClickedBtnDeleSelectedProgInLiset2()
{
	// TODO: 在此添加控件通知处理程序代码
	int nRowIndex=m_grid2.GetSelectionMark();
	//如果没有选中，返回
	if(nRowIndex == -1)
	{
		return;
	}
	m_grid2.DeleteItem(nRowIndex);
	float fTotal = 0;
	for(int j = 0; j < m_grid2.GetItemCount(); j++)
	{
		float k=static_cast<float>(atof(m_grid2.GetItemText(j,2)));
		fTotal += k;
	}
	CString total;
	total.Format("%5.2f",fTotal);
	GetDlgItem(IDC_EDIT5)->SetWindowTextA(total);
	////重新编号
	for(int k=0;k<m_grid2.GetItemCount();k++)
	{
		CString xuhao;
		xuhao.Format("%d",k+1);
		m_grid2.SetItemText(k,0,xuhao);
	}
}


void CTechnolProcePrepareDlg::OnBnClickedBtnMoveUperInList2()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!m_grid2.GetSelectedCount())
	{
		MessageBox("请选择要移动的工序！","提示",MB_ICONEXCLAMATION);
		return;
	}
	int i=m_grid2.GetSelectionMark();
	CString str;
	str.Format("%d",i);
	//MessageBox(str);
	if(i==0)
	{
		MessageBox("已经为第一行，不可上移！","提示");
		return;
	}
	CString tem1,tem2;
	tem1=m_grid2.GetItemText(i,1);
	tem2=m_grid2.GetItemText(i,2);
	m_grid2.DeleteItem(i);
	m_grid2.InsertItem(i-1,str);
	m_grid2.SetItemText(i-1,1,tem1);
	m_grid2.SetItemText(i-1,2,tem2);
	////重新编号
	for(int k=0;k<m_grid2.GetItemCount();k++)
	{
		CString xuhao;
		xuhao.Format("%d",k+1);
		m_grid2.SetItemText(k,0,xuhao);
	}
}


void CTechnolProcePrepareDlg::OnBnClickedBtnMoveDownInList2()
{
	// TODO: 在此添加控件通知处理程序代码
	if(!m_grid2.GetSelectedCount())
	{
		MessageBox("请选择要移动的工序！","提示",MB_ICONEXCLAMATION);
		return;
	}
	int i=m_grid2.GetSelectionMark();
	CString str;
	str.Format("%d",i);
	//MessageBox(str);
	if(i==m_grid2.GetItemCount()-1)
	{
		MessageBox("已经为最后一行，不可下移！","提示");
		return;
	}
	CString tem1,tem2;
	tem1=m_grid2.GetItemText(i,1);
	tem2=m_grid2.GetItemText(i,2);
	m_grid2.DeleteItem(i);
	m_grid2.InsertItem(i+1,str);
	m_grid2.SetItemText(i+1,1,tem1);
	m_grid2.SetItemText(i+1,2,tem2);
	////重新编号
	for(int k=0;k<m_grid2.GetItemCount();k++)
	{
		CString xuhao;
		xuhao.Format("%d",k+1);
		m_grid2.SetItemText(k,0,xuhao);
	}
}
