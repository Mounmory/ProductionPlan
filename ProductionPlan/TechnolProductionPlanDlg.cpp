// TechnolProductionPlanDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolProductionPlanDlg.h"
#include "TechnolProceDBNameSaveDlg.h"
#include "afxdialogex.h"

#include "ProductionPlanCalculate.h"//计算生产计划专用

static std::map<uint32_t, STRING> g_mapDealResult;//<key:cmdID,value:strCmd>根据执行结果进行下一个处理
static const STRING updateProcTable ;//显示到状态栏
																//设置状态栏
static StatusData g_statuData;

//检查并关闭显示结果对话框
#define RESULT_WINDOW_CHECK_CLOSE \
if (IsWindow(m_ResultDlg.m_hWnd))\
	m_ResultDlg.DestroyWindow();\



// CTechnolProductionPlanDlg 对话框

IMPLEMENT_DYNAMIC(CTechnolProductionPlanDlg, CDialog)

CTechnolProductionPlanDlg::CTechnolProductionPlanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTechnolProductionPlanDlg::IDD, pParent)
	, m_dbService(getProPlanDBService())
	, m_threadPool(getThreadPoolService())
	,m_bTreadIsRunning(false)
	,m_nMaxDayPlan(60)
{
	m_dbService->addListener(this);
}

CTechnolProductionPlanDlg::~CTechnolProductionPlanDlg()
{
	m_dbService->removeListener(this);
}

void CTechnolProductionPlanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo1);
	DDX_Control(pDX, IDC_LIST1, m_grid1);
	DDX_Control(pDX, IDC_LIST2, m_grid2);
}


BEGIN_MESSAGE_MAP(CTechnolProductionPlanDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CTechnolProductionPlanDlg::OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BTN_SET_PROC_INFO, &CTechnolProductionPlanDlg::OnBnClickedBtnSetProcAndWorkPlaceInfoToList1AndList2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_GET_WORKPLACE_FROM_DB, &CTechnolProductionPlanDlg::OnBnClickedBtnGetWorkplaceInfoFromDbToList2)
	ON_BN_CLICKED(IDC_BTN_PRODU_PLAN, &CTechnolProductionPlanDlg::OnBnClickedBtnProduPlan)
	ON_BN_CLICKED(IDC_BTN_SHOW_WORK_PLACE_USED, &CTechnolProductionPlanDlg::OnBnClickedBtnShowWorkPlaceUsedToList2)
	ON_BN_CLICKED(IDC_BTN_PLAN_AND_WORK_TO_DB, &CTechnolProductionPlanDlg::OnBnClickedBtnSavePlanAndWorkplaceUsedInfoToDb)
	ON_BN_CLICKED(IDC_BTN_SHOW_RESULT, &CTechnolProductionPlanDlg::OnBnClickedBtnShowResult)
END_MESSAGE_MAP()


// CTechnolProductionPlanDlg 消息处理程序
void CTechnolProductionPlanDlg::dealDBResult(std::queue<DBResult>& rst)
{
	LOGDEBUG("[%s][%d] begin, result size %d !", __STRING_FUNCTION__, __LINE__, rst.size());

	while (rst.size() > 0)
	{
		auto iterRst = rst.front();
		if (g_mapDealResult.find(iterRst.cmdID) != g_mapDealResult.end())
		{
			//设置状态栏
			CString strStatus;
			if (updateProcTable == g_mapDealResult[iterRst.cmdID])
			{
				strStatus.Format("添加表格 %s ！数据库名称 %s 表格名称 %s 。"
					, iterRst.bResult ? "成功" : "失败"
					, iterRst.strDBName
					, iterRst.strTableName);
			}
			else
			{
			}
			m_StatusBar.SetPaneText(1, strStatus);
			//显示设置
			g_statuData.strInfo = strStatus;
			g_statuData.nTimeCount = 15;//显示15s
			g_mapDealResult.erase(iterRst.cmdID);
		}
		rst.pop();
	}
	LOGDEBUG("[%s][%d] CTechnolPlanAndWorkPlaceViewDlg::dealDBResult end!", __STRING_FUNCTION__, __LINE__);
}

BOOL CTechnolProductionPlanDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	////设置对话框图标
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CREO);//IDR_ICON为图标资源名
	SetIcon(m_hIcon, FALSE);
	
	//设置图标
	m_ImageList.Create(14, 14, ILC_COLOR24 | ILC_MASK, 1, 1);
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_ASM)));
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PRT)));
	
	//设置状态栏
	UINT array[8];
	for(int i=0;i<8;i++)
	{
		array[i] = 1001 + i;
	}
	m_StatusBar.Create(this); //创建状态栏窗口
	m_StatusBar.SetIndicators(array,sizeof(array)/sizeof(UINT)); //添加面板
	m_StatusBar.SetPaneInfo(0,array[0],0,70); //设置面板宽度
	m_StatusBar.SetPaneInfo(1,array[1],0,700); //设置面板宽度
	m_StatusBar.SetPaneInfo(2,array[2],0,100); //
	m_StatusBar.SetPaneInfo(3,array[3],0,100); 
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

	//设置列表控件1标题
	m_grid1.InsertColumn(0,_T("序号"),LVCFMT_LEFT,40,0);
	m_grid1.InsertColumn(1,_T("层级"),LVCFMT_LEFT,75,0);
	m_grid1.InsertColumn(2,_T("标识号"),LVCFMT_LEFT,85,0);
	m_grid1.InsertColumn(3,_T("物料编号"),LVCFMT_LEFT,80,0);
	m_grid1.InsertColumn(4,_T("物料名称"),LVCFMT_LEFT,100,0);
	m_grid1.InsertColumn(5,_T("数量(PC)"),LVCFMT_LEFT,65,0);
	m_grid1.InsertColumn(6,_T("制造流程"),LVCFMT_LEFT,150,0);
	m_grid1.InsertColumn(7,_T("工时(h/PC)"),LVCFMT_LEFT,150,0);
	m_grid1.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	//设置列表控件2标题，并设置第一列内容
	m_grid2.InsertColumn(0,_T("序号"),LVCFMT_LEFT,40,0);
	m_grid2.InsertColumn(1,_T("工位"),LVCFMT_LEFT,60,0);
	m_grid2.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	
	//设置单选按钮图标
	CButton* radio=(CButton*)GetDlgItem(IDC_RADIO2);
	radio->SetCheck(1);
	
	//设置组合框控件2
	CComboBox* combo=(CComboBox*)GetDlgItem(IDC_COMBO2);
	combo->ResetContent();
	combo->InsertString(0, "30");
	combo->InsertString(1, "60");
	combo->InsertString(2, "90");
	combo->InsertString(3, "120");
	combo->SetCurSel(1); //默认最大排列周期

	//将数据库中表格显示到组合框控件1
	ShowProcessTableListToCombo1();//将工艺流程表信息显示到组合框控件1中
	ShowWorkplaceToList2();//将工位信息显示到列表控件2中

	GetDlgItem(IDC_BTN_SET_PROC_INFO)->EnableWindow(FALSE);//设置完成排产按钮不可用
	GetDlgItem(IDC_BTN_PRODU_PLAN)->EnableWindow(FALSE);//排列生产计划按钮不可用
	GetDlgItem(IDC_BTN_SHOW_RESULT)->EnableWindow(FALSE);//显示查看结果按钮
	GetDlgItem(IDC_BTN_SHOW_WORK_PLACE_USED)->EnableWindow(FALSE);//显示查看使用工位情况按钮
	GetDlgItem(IDC_BTN_PLAN_AND_WORK_TO_DB)->EnableWindow(FALSE);//保存工位使用到数据库按钮

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CTechnolProductionPlanDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CTime Time;
	Time = CTime::GetCurrentTime();

	m_StatusBar.SetPaneText(4,Time.Format("%Y-%m-%d"));
	m_StatusBar.SetPaneText(5,Time.Format("%H:%M:%S"));

	//操作状态
	if (g_statuData.nTimeCount > 0)
	{
		CString temStr = g_statuData.strInfo + "(" + std::to_string(g_statuData.nTimeCount--).c_str() + ")";
		m_StatusBar.SetPaneText(1, temStr);
	}
	else
	{
		m_StatusBar.SetPaneText(1, "");
	}
	CDialog::OnTimer(nIDEvent);
}

//在组合框控件中显示数据库中表格
void CTechnolProductionPlanDlg::ShowProcessTableListToCombo1(void)
{
	//将表格显示到组合框控件
	m_combo1.ResetContent();
	std::vector<STRING> temCols = (*m_dbService)["ProductionProcessInfo"].m_TableNames;
	auto iterCol = temCols.begin();
	size_t index = 0;
	while (iterCol != temCols.end())
	{
		m_combo1.InsertString(index++, *(iterCol++));//插入控件
	}
	//m_combo1.SetCurSel(0);
	m_combo1.SetCurSel(-1);
}

//组合框控件修改完，显示生产计划
void CTechnolProductionPlanDlg::OnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	if(true == m_bTreadIsRunning)	//检查线程是否正在运行
	{
		MessageBox("正在排列生产计划！","提示",MB_ICONSTOP);
		return;
	}

	//如果结果对话框存在，销毁窗口
	RESULT_WINDOW_CHECK_CLOSE

	m_grid1.SetImageList(&m_ImageList,LVSIL_SMALL);//设置图标
	m_grid1.DeleteAllItems();

	CString strTableName;
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(strTableName);//获取表格名称
	TableInfo temTable = (*m_dbService)["ProductionProcessInfo"][strTableName];
	size_t nValueRowCount = temTable.m_Values.getRowCount();
	size_t nValueColCount = temTable.m_Values.getColCount();
	//按序号排序

	//显示到表格中
	int nImageIndex = 0;//图标索引
	int nMerCount = 0;//需要排列的物料数量，外购物料不排列
	bool hasEmptyProc = false;//是否有空缺工艺流程
	for (int i = 0; i < nValueRowCount; ++i)
	{
		CString strProcess = temTable.m_Values[i][8];
		if (strProcess.Find("外购") != -1)//外购件不排计划
		{
			continue;
		}
		if (strProcess == "")
		{
			hasEmptyProc = true;
		}

		//设置模型索引图片
		temTable.m_Values[i][5] == "asm" ? nImageIndex = 0 : nImageIndex = 1;/////模型类型
		//m_grid1.InsertItem(nMerCount,temTable.m_Values[i][0],nImageIndex);	//图片索引号(第一幅图片)
		m_grid1.InsertItem(nMerCount, std::to_string(nMerCount + 1).c_str(), nImageIndex);	//图片索引号(第一幅图片)
		m_grid1.SetItemText(nMerCount, 1, temTable.m_Values[i][1]);//层级
		m_grid1.SetItemText(nMerCount, 2, "00");//物料标识，初始2字符均为"00"
		m_grid1.SetItemText(nMerCount, 3, temTable.m_Values[i][2]);//模型ID
		m_grid1.SetItemText(nMerCount, 4, temTable.m_Values[i][3]);//中文名称
		m_grid1.SetItemText(nMerCount, 5, temTable.m_Values[i][6]);//数量
		m_grid1.SetItemText(nMerCount, 6, temTable.m_Values[i][8]);//工艺流程
		m_grid1.SetItemText(nMerCount, 7, temTable.m_Values[i][9]);//流程时间
		++nMerCount;
	}

	GetDlgItem(IDC_BTN_SET_PROC_INFO)->EnableWindow(TRUE);//设置完成排产按钮可用
	GetDlgItem(IDC_BTN_PRODU_PLAN)->EnableWindow(FALSE);//排列生产计划按钮不可用
	GetDlgItem(IDC_BTN_SHOW_RESULT)->EnableWindow(FALSE);//显示查看结果按钮
	GetDlgItem(IDC_BTN_SHOW_WORK_PLACE_USED)->EnableWindow(FALSE);//显示查看使用工位情况按钮
	GetDlgItem(IDC_BTN_PLAN_AND_WORK_TO_DB)->EnableWindow(FALSE);//保存工位使用到数据库按钮
	//检查工序中工艺流程表是否有欠缺
	if (true == hasEmptyProc)
	{
		MessageBox("工艺流程表中工艺流程有欠缺", "提示", MB_ICONERROR);
		GetDlgItem(IDC_BTN_SET_PROC_INFO)->EnableWindow(FALSE);
		return;
	}

	//生成物料标识
	int nLevel = 4;
	GenerateLevelID(nLevel);
}


void CTechnolProductionPlanDlg::GenerateLevelID(const int nLevel)
{
	std::vector<CString> strID(m_grid1.GetItemCount());
	int j=1;//记录第j个子项序号值
	//int dengyu=0,dayu=0;///统计等于层级项及大于层级项，如果都为零，说明层级超过最高了，退出循环
	bool isequal = false;
	bool ishigh = false;
	CString strtemp;
	for(int i=0;i<m_grid1.GetItemCount();i++)
	{
		strtemp=m_grid1.GetItemText(i,1);
		if(strtemp.GetLength() < nLevel)//小于检查层级，表示与上一层级没有连续，设置子项计数序号值为1，进行下一循环
		{
			j=1;
			continue;
		}
		else if(strtemp.GetLength() == nLevel)//等于检查层级
		{
			strID[i].Format("%s%02d",m_grid1.GetItemText(i,2),j);
			m_grid1.SetItemText(i,2,strID[i]);
			j++;
			isequal =true;
		}
		else//大于检查层级，内容与上层一致
		{
			strID[i]=m_grid1.GetItemText(i-1,2);
			m_grid1.SetItemText(i,2,strID[i]);
			ishigh =true;
		}
	}
	if(isequal==true || ishigh ==true)	////循环过程有编号操作，递归到下一层级
		return GenerateLevelID(nLevel+4);
	else	////循环时无编号操作，终止循环	
		return;
}

//完成生产设置按钮
void CTechnolProductionPlanDlg::OnBnClickedBtnSetProcAndWorkPlaceInfoToList1AndList2()//参数设置处保证参数正确性
{
	// TODO: 在此添加控件通知处理程序代码

	if(true == m_bTreadIsRunning)	//检查线程是否正在运行
	{
		MessageBox("正在排列生产计划！","提示",MB_ICONSTOP);
		return;
	}
	CString strTotalNum, strDate, strPerNum, strMaxDay;
	GetDlgItem(IDC_EDIT1)->GetWindowTextA(strTotalNum);//获取数量
	GetDlgItem(IDC_DATETIMEPICKER1)->GetWindowTextA(strDate);//获取日期
	GetDlgItem(IDC_COMBO2)->GetWindowTextA(strMaxDay);//获取最大生产日期
	GetDlgItem(IDC_EDIT2)->GetWindowTextA(strPerNum);//获取每次排产数量
	if(strTotalNum == "")
	{
		MessageBox("所有信息均不能为空！","提示",MB_ICONSTOP);
		return;
	}
	m_nTotalProdNum = atoi(strTotalNum);
	m_nMaxDayPlan = atoi(strMaxDay);
	if(m_nTotalProdNum <= 0)
	{
		MessageBox("生产数量应为大于0的整数！","提示");
		return;
	}
	///////////////////******************根据按钮控件，设置排列方式*************////
	CButton* radio=(CButton*)GetDlgItem(IDC_RADIO1);
	if(radio->GetCheck()==1)//按照每组固定数量生产
	{
		GetDlgItem(IDC_EDIT3)->SetWindowText("");//设置时间权重设置为空
		m_nNumPerTime = atoi(strPerNum);//每次生产的数量
		if(m_nNumPerTime <= 0 || m_nNumPerTime > m_nTotalProdNum)
		{
			MessageBox("生产设置值错误！","提示");
			return;
		}
	}
	else//智能排列生产计划
	{
		GetDlgItem(IDC_EDIT2)->SetWindowText("");////生产设置第一项设置为空
		/////时间权重设置为自动设置，故此行注释掉了
		CString strTimeWeight;
		GetDlgItem(IDC_EDIT3)->GetWindowText(strTimeWeight);
		if ("" != strTimeWeight)
		{
			float fTimeWeight = static_cast<float>(atof(strTimeWeight));//将double强制转换为float格式
			if (fTimeWeight < 0 || fTimeWeight > 1)
			{
				MessageBox("时间权重不合法，请重新输入！");
				return;
			}
			else 
			{
				m_ResultDlg.setTimeWeight(fTimeWeight);//设置时间权重
			}
		}
	}

	//设置列表控件1中的排产数量
	int nOrigNum = atoi(m_grid1.GetItemText(0,5));//每个工艺流程表中初始数量，一般情况为1，二次点按钮可能会为别的值
	int nRowCount1 = m_grid1.GetItemCount();//物料清单最大数量
	int nTempNum = 0;//用于循环中保存临时变量
	CString strTemp;//用于循环中保存临时变量
	for(int i = 0; i < nRowCount1; ++i)
	{
		nTempNum = atoi(m_grid1.GetItemText(i,5));
		m_grid1.SetItemText(i,5,std::to_string(nTempNum * m_nTotalProdNum / nOrigNum).c_str());
	}
	//删除旧的日期列（如果有）
	CHeaderCtrl* pHeaderCtrl=m_grid1.GetHeaderCtrl();
	int nColumn=pHeaderCtrl-> GetItemCount();
	if(nColumn > 8)
	{
		for(int i = nColumn; i > 7; --i)
		{
			m_grid1.DeleteColumn(i);
			m_grid2.DeleteColumn(i-6);//表格2比表格1少6列
		}
	}
	//添加新的日期列
	strDate = GlobalFunction::translatDay(strDate);
	m_VecDataName.clear();
	for(int i = 0; i < m_nMaxDayPlan; ++i)
	{
		m_VecDataName.push_back(strDate);
		m_grid1.InsertColumn(i+8,strDate,LVCFMT_LEFT,70,0);
		m_grid2.InsertColumn(i+2,strDate,LVCFMT_LEFT,70,0);
		strDate = GlobalFunction::nextDay(strDate);
	}
	//将可用工位信息显示到列表2
	OnBnClickedBtnGetWorkplaceInfoFromDbToList2();
	//修改工位信息组合框标题
	//GetDlgItem(IDC_GW_INFO)->SetWindowText("工位信息（8h/工位）：");
	//设置排列结果为空
	GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA("");
	//隐藏“查看排列计划按钮”
	GetDlgItem(IDC_BTN_SHOW_RESULT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_PRODU_PLAN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SHOW_WORK_PLACE_USED)->EnableWindow(FALSE);//查看工位使用情况按钮
	GetDlgItem(IDC_BTN_PLAN_AND_WORK_TO_DB)->EnableWindow(FALSE);//保存工位使用情况到数据库按钮

}

void CTechnolProductionPlanDlg::ShowWorkplaceToList2(void)
{
	TableInfo temTable = (*m_dbService)["WorkPlaceInfo"]["WorkPlaceDef"];
	int32_t nRowCount = temTable.m_Values.getRowCount();
	m_nVecWorkPInfo.clear();
	m_nVecWorkPInfo.reserve(nRowCount);
	for (int i = 0; i < nRowCount; ++i)
	{
		m_grid2.InsertItem(i, temTable.m_Values[i][0]);
		m_grid2.SetItemText(i, 1, temTable.m_Values[i][1]);
		//m_grid2.SetItemText(i, 2, temTable.m_Values[i][2]);
		m_nVecWorkPInfo.emplace_back(_ttoi(temTable.m_Values[i][2]));//记录工位数量
	}
}


void CTechnolProductionPlanDlg::OnBnClickedBtnGetWorkplaceInfoFromDbToList2()
{
	// TODO: 在此添加控件通知处理程序代码	if(true==m_bTreadIsRunning)	//检查线程是否正在运行
	if(true == m_bTreadIsRunning)	//检查线程是否正在运行
	{
		MessageBox("正在排列生产计划！","提示",MB_ICONSTOP);
		return;
	}
	//删除旧的日期列（如果有）
	CHeaderCtrl* pHeaderCtrl = m_grid2.GetHeaderCtrl();
	int nColumnCount = pHeaderCtrl-> GetItemCount();//总列数
	int nRowCount = m_grid2.GetItemCount();//总行数
	if(nColumnCount <=2)//检查列表控件2中列数
	{
		MessageBox("请先设置开工日期！","提示",MB_ICONSTOP);
		return;
	}
	if( nRowCount != m_nVecWorkPInfo.size())
	{
		MessageBox("工位信息获取错误！","提示",MB_ICONSTOP);
		return;
	}
	//获取生产计划开始时间
	CString strStartTime;
	{
		HDITEM pHeadItem;  
		pHeadItem.mask = HDI_TEXT;
		TCHAR  lpBuffer[64];
		pHeadItem.pszText = lpBuffer;
		pHeadItem.cchTextMax = 64;
		pHeaderCtrl->GetItem(2,&pHeadItem);
		strStartTime = pHeadItem.pszText;//获取生产计划开始时间
	}
	//从数据库中读取工位信息
	std::map<CString,int> mapDataIndex;//存放日期索引<key:data,value:index>
	std::map<CString,int> mapWorkPlaceIndex;//工位索引<key:workplacename,value:index>
	{
		//建立日期索引
		CString strTempDate = strStartTime;
		for (int i = 0; i < nColumnCount - 2; ++i)
		{
			mapDataIndex[strTempDate] = i;
			strTempDate = GlobalFunction::nextDay(strTempDate);
		}
		//建立工位索引
		for (int i = 0; i < nRowCount; ++i)
		{
			mapWorkPlaceIndex[m_grid2.GetItemText(i,1)] = i;
		}
	}
	//保存数据库中工位使用的值
	DynaArray2D<float> fArrUsedWorkPlaceInfo(nRowCount,nColumnCount-2);//保存已使用的工位
	fArrUsedWorkPlaceInfo.zeroArray();//清零

	DataBaseInfo temDatabase = (*m_dbService)["WorkPlaceUsedInfo"];
	for (auto iterTabel : temDatabase.m_tables)//遍历表格
	{
		int nValueRowCount = iterTabel.second.m_Values.getRowCount();
		int nValueColCount = iterTabel.second.m_Values.getColCount();

		for (int i = nValueRowCount - 1; i >= 0; --i)//从最后一行遍历,日期由大到小
		{
			if (iterTabel.second.m_Values[i][1] < strStartTime)//日期早于排列日期
			{
				break;
			}
			for (int j = 2; j < nValueColCount; ++j)//第一列是序号，第二列是日期，第三列开始是工位数据
			{
				auto iterDataIndex = mapDataIndex.find(iterTabel.second.m_Values[i][1]);//获取日期索引
				auto iterWorkPlacIndex = mapWorkPlaceIndex.find(iterTabel.second.m_vevCols[j]);//获取工位索引
				if (iterDataIndex != mapDataIndex.end() && iterWorkPlacIndex != mapWorkPlaceIndex.end())
				{
					float fTempData = atof(iterTabel.second.m_Values[i][j]);//获取工位使用值
					fArrUsedWorkPlaceInfo[iterWorkPlacIndex->second][iterDataIndex->second] += fTempData;
				}
			}
		}
	}

	//将可用工位信息显示到数据库
	CString tmpWork;//临时字符串
	float fWorkPlaceFree(0);//空余的工位
	for (int i = 2; i<nColumnCount; ++i)
	{
		for (int j = 0; j < nRowCount; ++j)
		{
			fWorkPlaceFree = static_cast<float>(m_nVecWorkPInfo[j]) - fArrUsedWorkPlaceInfo[j][i-2];
			if (fWorkPlaceFree > 0)
			{
				tmpWork.Format("%.2f",fWorkPlaceFree);//有空余的工位
				m_grid2.SetItemText(j,i,tmpWork);
			}
			else
			{
				m_grid2.SetItemText(j,i,"");
			}
		}
	}
	GetDlgItem(IDC_GW_INFO)->SetWindowText("空余工位信息（8h/工位）：");
}

#define IN_VALUE
void CTechnolProductionPlanDlg::OnBnClickedBtnProduPlan()
{
	// TODO: 在此添加控件通知处理程序代码
	if (true == m_bTreadIsRunning)	//检查线程是否正在运行
	{
		MessageBox("正在排列生产计划！", "提示", MB_ICONSTOP);
		return;
	}
	GetDlgItem(IDC_BTN_PRODU_PLAN)->EnableWindow(FALSE);//将按钮不可用（无论执行成功还是失败）

	//获取工艺流程表行数、列数
	CHeaderCtrl* pHeaderCtrl1 = m_grid1.GetHeaderCtrl();
	uint16_t nColCount1 = pHeaderCtrl1->GetItemCount();//表格1总列数,1-8列为物料信息
	uint16_t nRowCount1 = m_grid1.GetItemCount();//表格1总行数

	//获取工位表格行数、列数
	CHeaderCtrl* pHeaderCtrl2 = m_grid2.GetHeaderCtrl();
	uint16_t nColCount2 = pHeaderCtrl2->GetItemCount();//表格2总列数,1-2列为工位信息
	uint16_t nRowCount2 = m_grid2.GetItemCount();//表格2总行数

	//获取工位信息序号（索引）
	IN_VALUE std::map<CString, uint16_t> mapWorkPlaceIndex; //<key:workplaceName ,value:workplace index>
	for (uint16_t nIndex = 0; nIndex < nRowCount2; ++nIndex)
	{
		mapWorkPlaceIndex.insert(std::make_pair(m_grid2.GetItemText(nIndex, 1), nIndex));
	}
		
	//获取空余工位信息
	IN_VALUE DynaArray2D<float> arrWorkPlaceFree(nRowCount2, nColCount2 - 2);
	for (int i = 0; i < nRowCount2; ++i)
	{
		for (int j = 2; j < nColCount2; ++j)
		{
			arrWorkPlaceFree[i][j - 2] = 8 * static_cast<float>(atof(m_grid2.GetItemText(i, j)));
		}
	}

	//获取工艺流程表中工位使用及时间
	IN_VALUE std::vector<std::map<uint16_t, float>> vecMapWorkPlaceUse;//map <key:workplace Index,value:workplace cost time>
	vecMapWorkPlaceUse.reserve(nRowCount1);

	//获取各物料到根节点总工时
	IN_VALUE std::vector<float> vecTimeToRoot;
	vecTimeToRoot.reserve(nRowCount1);

	IN_VALUE std::vector<STRING> vecMertID;//物料ID

	IN_VALUE std::vector<uint16_t> vecMertNumPer;//每个产品物料数
	vecMertNumPer.reserve(nRowCount1);

	uint16_t nOrigNum = atoi(m_grid1.GetItemText(0, 5));//原始数量

	for (int i = 0; i < nRowCount1; ++i)//i为第i行
	{
		//1.获取使用工位信息
		std::map<uint16_t, float> mapTemPlaceUse; //每一行的工位使用信息
		float fTemTimeCount = 0;	//每一行到根节点的总工时
		if (!getWorkPlaceUsedInfo(i, mapWorkPlaceIndex, mapTemPlaceUse, fTemTimeCount))//获取当前节点工位使用信息及所用工时
		{
			return;
		}
		vecMapWorkPlaceUse.emplace_back(std::move(mapTemPlaceUse));

		//2.获取物料ID
		CString strTempID = m_grid1.GetItemText(i, 2);
		vecMertID.emplace_back(strTempID);

		//3.获取每一行到根节点的总工时
		vecTimeToRoot.emplace_back(fTemTimeCount);
		strTempID = strTempID.Left(strTempID.GetLength() - 2);
		for (int j = i - 1; j >= 0; --j)
		{
			if (strTempID == m_grid1.GetItemText(j, 2))
			{
				vecTimeToRoot[i] += vecTimeToRoot[j];
				break;
			}
		}
		
		//4.获取单个产品零件数
		vecMertNumPer.emplace_back(atoi(m_grid1.GetItemText(i, 5)) / nOrigNum);
	}


	//获取排产选择
	CButton* radio = (CButton*)GetDlgItem(IDC_RADIO1);
	if (radio->GetCheck() == 1)//按指定数量排列生产计划
	{
		std::future<stuPlanRet> res1 = m_threadPool->submit(
			GenerateProductionPlan, //调用函数
			m_nTotalProdNum, 
			m_nNumPerTime, 
			mapWorkPlaceIndex, 
			arrWorkPlaceFree, 
			vecMapWorkPlaceUse,
			vecTimeToRoot,
			vecMertID,
			vecMertNumPer);

		stuPlanRet temRet = res1.get();
		if (temRet.nMaxDayLen == 0)
		{
			GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA("排列生产计划失败，请重新设置！");
			return;
		}

		//LOGDEBUG_BYSTREAM(temRet.arrResult);
		m_nMaxDayUsed = temRet.nMaxDayLen;//生产周期

		CString strTempMsg;
		//显示排列结果到列表控件1中
		for (int i = 0; i < nRowCount1; ++i)
			for (int j = 8; j < m_nMaxDayUsed + 8; ++j)
			{
				strTempMsg = temRet.arrResult[i][j - 8] == 0 ? "" : std::to_string(temRet.arrResult[i][j - 8]).c_str();
				m_grid1.SetItemText(i,j,strTempMsg);
			}

		//处理工位使用
		m_fArrWorkPlaceUsed = std::move(temRet.arrWorkUsed);
		strTempMsg.Format("共排列%d项物料，生产数量%d台（个），每组排产%d台（个），生产周期%d天",nRowCount1,m_nTotalProdNum,m_nNumPerTime, m_nMaxDayUsed);
		GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA(strTempMsg);
		GetDlgItem(IDC_BTN_SHOW_WORK_PLACE_USED)->EnableWindow(TRUE);//查看工位使用情况按钮
		GetDlgItem(IDC_BTN_PLAN_AND_WORK_TO_DB)->EnableWindow(TRUE);//保存工位使用情况到数据库按钮
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

void CTechnolProductionPlanDlg::GenerateProductionPlanAuto(
	const uint16_t nTotalNum	//总排列数目
	, const std::map<CString, uint16_t>& mapWorkPlIndex	//workplace info, <key:workplaceName ,value:workplace index>
	, const DynaArray2D<float>& arrWorkFree	//workplace info, free information
	, const std::vector<std::map<uint16_t, float>>& vecMapWorkPlaceUse	// material information ,map <key:workplace Index,value:workplace cost time>
	, const std::vector<float>& vecTimeToRoot	// material information ,cumulative time to root node);	
	, const std::vector<CString>& vecMertID	// material id
	, const std::vector<uint16_t>& vecMertNumPer)	 // material num per product
{
	//记录耗时
	clock_t start, ends;
	start = clock();
	//记录线程运行状态
	m_bTreadIsRunning = true;

	CString strTempMsg;
	strTempMsg="开始智能排列……";
	GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA(strTempMsg);
	//记录排列过程中参数
	std::vector<float> fVecTimeLong;//记录生产周期
	std::vector<float> fVecTotalDayMer;//记录物料累计占用时间
	std::vector<float> fVecTotalWorkPlace;//记录累计占用工位数量*天数

	fVecTimeLong.reserve(m_nTotalProdNum);
	fVecTotalDayMer.reserve(m_nTotalProdNum);
	fVecTotalWorkPlace.reserve(m_nTotalProdNum);

	std::vector<std::future<stuPlanRet>> vecFetResult;
	vecFetResult.reserve(m_nTotalProdNum);

	for (int numPer = 0; numPer <m_nTotalProdNum; ++numPer)/////从高往低排，任务添加到线程池
	{
		vecFetResult.emplace_back(m_threadPool->submit(
			GenerateProductionPlan, //调用函数
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
	std::vector<stuPlanRet> vecResult;//所有的结果
	vecResult.reserve(m_nTotalProdNum);
	bool isFailed = false;//是否排列失败
	int nBestNum = 0;
	do{
		for (int k = 0; k <m_nTotalProdNum; ++k)/////从高往低排，任务添加到线程池
		{
			strTempMsg = "等待结果……";
			GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA(strTempMsg);
			stuPlanRet temRet = vecFetResult[k].get();
			fVecTimeLong.emplace_back(static_cast<float>(temRet.nDayCount));
			fVecTotalDayMer.emplace_back(static_cast<float>(temRet.nMertTotalDay));
			fVecTotalWorkPlace.emplace_back(static_cast<float>(temRet.nWorkTotalDay));

			//保存所有结果
			vecResult.emplace_back(std::move(temRet));
		
			if(0 == vecResult[k].nMaxDayLen)
			{
				isFailed = true;
				//break;//是否有必要接受所有结果？
			}
			//显示进度
			strTempMsg.Format("已完成……%d %%", (100 * k / m_nTotalProdNum));
			GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA(strTempMsg);
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
	if(true == isFailed )
	{
		MessageBox("排列失败，请重新设置参数！", "提示", MB_ICONSTOP);//不会发生
		m_bTreadIsRunning = false;//解出线程运行状态
		GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA("");
		return;
	}

	//获取最佳排列结果
	stuPlanRet temRet = std::move(vecResult[nBestNum - 1]);

	m_fArrWorkPlaceUsed = std::move(temRet.arrWorkUsed);	//记录工位使用
	DynaArray2D<uint16_t> arrTemResult = std::move(temRet.arrResult);	//排列结果
	m_nMaxDayUsed = temRet.nMaxDayLen;	//累计时间
	//显示排列结果到列表控件1中
	for (int i = 0; i < arrTemResult.getRowCount(); ++i)
		for (int j = 8; j < m_nMaxDayUsed + 8; ++j)//大于最大周期的都是0了
		{
			strTempMsg = arrTemResult[i][j - 8] == 0 ? "" : std::to_string(arrTemResult[i][j - 8]).c_str();
			m_grid1.SetItemText(i, j, strTempMsg);
		}

	//结束时间
	ends = clock();
	float fInterval = static_cast<float>(ends - start);

	strTempMsg.Format("智能排列最佳结果（用时 %.3f s）：共排列%d项物料，生产数量%d台（个），每组排产%d台（个），生产周期%d天", fInterval / 1000, arrTemResult.getRowCount(), m_nTotalProdNum, nBestNum, m_nMaxDayUsed);
	GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA(strTempMsg);
	GetDlgItem(IDC_BTN_SHOW_RESULT)->EnableWindow(TRUE);//显示查看结果按钮
	GetDlgItem(IDC_BTN_SHOW_WORK_PLACE_USED)->EnableWindow(TRUE);//查看工位使用情况按钮
	GetDlgItem(IDC_BTN_PLAN_AND_WORK_TO_DB)->EnableWindow(TRUE);//保存工位使用情况到数据库按钮
	m_bTreadIsRunning = false;//解出线程运行状态
}


bool CTechnolProductionPlanDlg::getWorkPlaceUsedInfo(const uint16_t index, const std::map<CString, uint16_t>& mapWorkPlaceInfo, std::map<uint16_t, float>& mapWorkPlaceUse, float& fTimeCount)
{
	//存储临时工序和工时！！！！！
	std::vector<CString> strTemProcess;	//工序名称
	std::vector<float> fTemWorkTime;	//工序时间
	strTemProcess.reserve(10);
	fTemWorkTime.reserve(10);//一般工艺流程不会超过10个

	CString	strProcess = m_grid1.GetItemText(index, 6);//获取工序
	CString strWorkTime = m_grid1.GetItemText(index, 7);//获取工时

	strProcess = strProcess + "->";
	while (strProcess.GetLength() > 2)//拆分工艺流程
	{
		int posi = strProcess.Find("->");
		strTemProcess.emplace_back(strProcess.Left(posi));
		strProcess = strProcess.Right(strProcess.GetLength() - posi - 2);
	}
	strWorkTime = strWorkTime + "+";
	while (strWorkTime.GetLength() > 1)//拆分工时
	{
		int pos1 = strWorkTime.Find("+");
		fTemWorkTime.emplace_back(atof(strWorkTime.Left(pos1)));
		strWorkTime = strWorkTime.Right(strWorkTime.GetLength() - pos1 - 1);
	}
	//检查工序和工时数量是否一致
	if (strTemProcess.size() != fTemWorkTime.size())
	{
		MessageBox("工艺流程表有误，工艺流程信息与工时信息数量不符", "提示", MB_ICONSTOP);
		return false;
	}
	//各零件工序和工时赋值
	for (int j = 0; j < strTemProcess.size(); ++j)
	{
		auto iter = mapWorkPlaceInfo.find(strTemProcess[j]);
		if (iter == mapWorkPlaceInfo.end())//检查工艺流程是否存在
		{
			CString temStr;
			temStr.Format("第 %d 行，工艺流程 %s 不存在！", index, strTemProcess[j]);
			MessageBox(temStr, "提示", MB_ICONSTOP);
			return false;
		}
		uint16_t nIndex = iter->second;
		auto iterMapUse = mapWorkPlaceUse.find(nIndex);
		if (iterMapUse == mapWorkPlaceUse.end())//首次添加工位
		{
			mapWorkPlaceUse.insert(std::make_pair(nIndex, fTemWorkTime[j]));
		}
		else
		{
			iterMapUse->second += fTemWorkTime[j];
		}

		fTimeCount += fTemWorkTime[j];
	}
	return true;
}

//查看工位使用情况
void CTechnolProductionPlanDlg::OnBnClickedBtnShowWorkPlaceUsedToList2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (true == m_bTreadIsRunning)	//检查线程是否正在运行
	{
		MessageBox("正在排列生产计划！", "提示", MB_ICONSTOP);
		return;
	}
	//获取工位可用信息
	CHeaderCtrl* pHeaderCtrl2 = m_grid2.GetHeaderCtrl();
	int nColCount2 = pHeaderCtrl2-> GetItemCount() - 2;//表格2总列数,1-2列为物料信息
	int nRowCount2 = m_grid2.GetItemCount();//表格2总行数
	if (m_fArrWorkPlaceUsed.getRowCount() != nRowCount2 || m_fArrWorkPlaceUsed.getColCount() != nColCount2)

	{
		MessageBox("工位信息错误！", "提示", MB_ICONSTOP);
		return;
	}

	CString temWorkUsed;
	for (int i = 0; i < nRowCount2; ++i)
	{
		for (int j = 0; j < nColCount2; ++j)
		{
			m_fArrWorkPlaceUsed[i][j] == 0 ? temWorkUsed = "" : temWorkUsed.Format("%.2f", static_cast<float>(m_fArrWorkPlaceUsed[i][j]));
			m_grid2.SetItemText(i, j + 2, temWorkUsed);
		}
	}
}

//将工位信息保存到数据库
void CTechnolProductionPlanDlg::OnBnClickedBtnSavePlanAndWorkplaceUsedInfoToDb()
{
	// TODO: 在此添加控件通知处理程序代码
	if (true == m_bTreadIsRunning)	//检查线程是否正在运行
	{
		MessageBox("正在排列生产计划！", "提示", MB_ICONSTOP);
		return;
	}
	ASSERT(m_nMaxDayUsed > 0);//排列完生产计划并赋值

	//确定表格名称（不存在则创建一个，存在则替换）
	CString strTableName;//创建的表格名称
	CString strDate;
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(strTableName);
	GetDlgItem(IDC_DATETIMEPICKER1)->GetWindowTextA(strDate);
	CTechnolProceDBNameSaveDlg dlg("WorkPlaceUsedInfo");//传入数据库名参数
	dlg.m_strTableName = strDate + strTableName;
	if (dlg.DoModal() != IDOK)
	{
		return;
	}
	strTableName = dlg.m_strTableName;

	uint16_t mRowCount1 = m_grid1.GetItemCount();//物料排列总行数
	uint16_t nRowCount2 = m_grid2.GetItemCount();//工位总行数

	//确定生产计划和工位使用信息的保存区域
	uint16_t nDayMin = 0;//记录最小和最大天数，工位和生产计划一致
	uint16_t	nDayMax = m_nMaxDayUsed;
	std::vector<CString> strVecWorkPlaceName;	//记录哪个工位被使用了，工位名称
	std::vector<uint16_t> nVecWorkPlaceUsedIndex;	//记录哪个工位被使用了，工位索引
	GetPlanWorkPlaceUsedInfo(strVecWorkPlaceName, nVecWorkPlaceUsedIndex, nDayMin, nDayMax);

	uint32_t nDaysCount = nDayMax - nDayMin;//占用的生产天数

	//保存工位使用信息
	{
		//创建表格SQL语句
		CString strSQL = "CREATE TABLE " + strTableName + "(Num TEXT,	DataInfo TEXT";
		for (auto iter : strVecWorkPlaceName)
		{
			strSQL += ",";
			strSQL += " " + iter + " TEXT";
		}
		strSQL += ")";
		//获取列、行数量，分配内存
		int nColumuCount = strVecWorkPlaceName.size() + 2;//列数量

		DynaArray2D<STRING> temValues(nDaysCount, nColumuCount);
		//写插入数据
		for (int i = 0; i < nDaysCount; ++i)//生产天数作为行
		{
			temValues[i][0] = std::to_string(i + 1).c_str();//序号
			temValues[i][1] = m_VecDataName[nDayMin + i];//日期
			for (int j = 2; j < nColumuCount; ++j)
			{
				if (m_fArrWorkPlaceUsed[nVecWorkPlaceUsedIndex[j - 2]][nDayMin + i] > 0)
				{
					temValues[i][j].Format("%.2f", m_fArrWorkPlaceUsed[nVecWorkPlaceUsedIndex[j - 2]][nDayMin + i]);
				}
			}
		}
		//添加表格命令
		//uint32_t cmdID = m_dbService->createTableCmd("WorkPlaceUsedInfo", strTableName, strSQL, std::move(temValues));
		//g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable));
		if (!m_dbService->createTableSyn("WorkPlaceUsedInfo", strTableName, strSQL, std::move(temValues)))//使用同步命令
		{
			MessageBox("保存工位使用信息失败！", "提示", MB_ICONERROR);
			return;
		}
	}

	//保存排列的生产计划
	{
		//创建表格SQL语句
		CString strSQL = "CREATE TABLE " + strTableName + "(序号 TEXT, 层级 TEXT, 标识 TEXT, 物料编号 TEXT,\
																物料名称 TEXT, 数量 TEXT, 制造流程 TEXT, 工时 TEXT";
		for (int i = 0; i < nDaysCount; ++i)
		{
			strSQL += ",";
			strSQL += " " + m_VecDataName[nDayMin + i] + " TEXT";
		}
		strSQL += ")";
		strSQL.Replace('-', '_');//更换字符，否则创建表格非法

		//分配内存,并加入写入值
		DynaArray2D<STRING> temValues(mRowCount1, nDaysCount + 8); 
		for (int i = 0; i < mRowCount1; ++i)
		{
			for (int j = 0; j < 8; ++j)//写入基本信息
			{
				temValues[i][j] = m_grid1.GetItemText(i, j);
			}
			for (int j = nDayMin + 8 ; j < nDayMax + 8; ++j)//写入生产计划信息
			{
				temValues[i][j - nDayMin] = m_grid1.GetItemText(i, j);
			}
			//写入生产计划信息
		}

		//uint32_t cmdID = m_dbService->createTableCmd("ProductionPlanedInfo", strTableName, strSQL, std::move(temValues));
		//g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable));
		if (!m_dbService->createTableSyn("ProductionPlanedInfo", strTableName, strSQL, std::move(temValues)))//使用同步命令
		{
			m_dbService->deleteTableSyn("WorkPlaceUsedInfo", strTableName);//删掉工位信息中的表格
			MessageBox("保存工位使用及生产计划信息失败！", "提示", MB_ICONERROR);
			return;
		}
	}

	OnBnClickedBtnShowWorkPlaceUsedToList2();//显示已用工位

	//显示结果到状态栏
	{
		CString strStatus;
		strStatus.Format("添加表格 成功 ！表格名称 %s 。", strTableName);
		m_StatusBar.SetPaneText(1, strStatus);
		//显示设置
		g_statuData.strInfo = strStatus;
		g_statuData.nTimeCount = 15;//显示15s
	}
}

//获取工位使用信息
bool CTechnolProductionPlanDlg::GetPlanWorkPlaceUsedInfo(std::vector<CString> & strVecWorkPlaName, std::vector<uint16_t> & nVecWorkPlaRowUsed, uint16_t & nDaysMin, const uint16_t nDaysMax)
{
	uint16_t nRowCount2 = m_grid2.GetItemCount();//表格2总行数
	strVecWorkPlaName.reserve(nRowCount2);
	nVecWorkPlaRowUsed.reserve(nRowCount2);

	//记录被占用的工位
	for (int i = 0; i < nRowCount2; ++i)
	{
		for (int j = 0; j < nDaysMax; ++j)
		{
			if(m_fArrWorkPlaceUsed[i][j] != 0)//工位被占用了
			{
				nVecWorkPlaRowUsed.emplace_back(i);//被占用工位索引
				strVecWorkPlaName.emplace_back(m_grid2.GetItemText(i,1));//工位名称
				break;
			}
		}
	}

	//找到最早开始时间
	int nWorkPlaceUsedCount = nVecWorkPlaRowUsed.size();
	bool bIsBreak = false;
	for (int j = 0; j < nDaysMax; ++j)//查找生产开始日期
	{
		for (int i = 0; i < nWorkPlaceUsedCount; ++i)
		{
			if(m_fArrWorkPlaceUsed[nVecWorkPlaRowUsed[i]][j] != 0)
			{
				nDaysMin = j;
				bIsBreak = true;
				break;
			}
		}
		if (bIsBreak)
			break;
	}

	return true;
}



void CTechnolProductionPlanDlg::OnBnClickedBtnShowResult()
{
	// TODO: 在此添加控件通知处理程序代码
	if (true == m_bTreadIsRunning)	//检查线程是否正在运行
	{
		MessageBox("正在排列生产计划！", "提示", MB_ICONSTOP);
		return;
	}
	//////非模式对话框
	if(!IsWindow(m_ResultDlg.m_hWnd))//如果对话框狂存在，销毁窗口
	{
		m_ResultDlg.Create(IDD_TEC_PROD_PLAN_SHOW,NULL);
	}
	m_ResultDlg.ShowWindow(SW_SHOW);
}

