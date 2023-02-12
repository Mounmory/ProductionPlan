// TechnolPlanAndWorkPlaceViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolPlanAndWorkPlaceViewDlg.h"
#include "afxdialogex.h"

static std::map<uint32_t, STRING> g_mapDealResult;//<key:cmdID,value:strCmd>根据执行结果进行下一个处理
static const STRING updateProcTable[2] = { "UpCom1", "UpLis2"};//更新组合框控件1，列表控件2.
static std::atomic_bool g_bWaitResult = false;	//等待结果
//设置状态栏
static StatusData g_statuData;

#define CALL_BACK_WAIT_CHECK \
if (true == g_bWaitResult)\
{\
	MessageBox("等待消息回调，请稍后操作！");\
}\
g_bWaitResult = true;

// CTechnolPlanAndWorkPlaceViewDlg 对话框

IMPLEMENT_DYNAMIC(CTechnolPlanAndWorkPlaceViewDlg, CDialog)

CTechnolPlanAndWorkPlaceViewDlg::CTechnolPlanAndWorkPlaceViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTechnolPlanAndWorkPlaceViewDlg::IDD, pParent)
	, m_dbService(getProPlanDBService())
{
	m_dbService->addListener(this);
}

CTechnolPlanAndWorkPlaceViewDlg::~CTechnolPlanAndWorkPlaceViewDlg()
{
	m_dbService->removeListener(this);
}

void CTechnolPlanAndWorkPlaceViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo1);
	DDX_Control(pDX, IDC_LIST1, m_grid1);
	DDX_Control(pDX, IDC_LIST2, m_grid2);
	DDX_Control(pDX, IDC_LIST3, m_grid3);
}


BEGIN_MESSAGE_MAP(CTechnolPlanAndWorkPlaceViewDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SHOW_PLAN_TO_LIST, &CTechnolPlanAndWorkPlaceViewDlg::OnClickedBtnShowPlanAndWorkplaceUsedToList)
	ON_BN_CLICKED(IDC_BTN_DELETE_PLAN, &CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnDeleteCurrentPlan)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST3, &CTechnolPlanAndWorkPlaceViewDlg::OnDblclkList3)
	ON_BN_CLICKED(IDC_BTN_ADD_WORKPLACE, &CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnAddWorkplace)
	ON_BN_CLICKED(IDC_BTN_DELETE_WORKPLAC, &CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnDeleteWorkplac)
END_MESSAGE_MAP()


// CTechnolPlanAndWorkPlaceViewDlg 消息处理程序

void CTechnolPlanAndWorkPlaceViewDlg::dealDBResult(std::queue<DBResult>& rst)
{
	LOGDEBUG("[%s][%d] begin, result size %d !", __STRING_FUNCTION__, __LINE__, rst.size());

	while (rst.size() > 0)
	{
		auto iterRst = rst.front();
		if (g_mapDealResult.find(iterRst.cmdID) != g_mapDealResult.end())
		{
			//设置状态栏
			CString strStatus;
			if (updateProcTable[0] == g_mapDealResult[iterRst.cmdID]) 
			{
				ShowPlanAndPlaceUsedTableToCombo1();//更新组合框控件
				
				strStatus.Format("删除表格 %s ！数据库名称 %s 表格名称 %s 。"
					, iterRst.bResult ? "成功" : "失败"
					, iterRst.strDBName
					, iterRst.strTableName);

			}
			else if (updateProcTable[1] == g_mapDealResult[iterRst.cmdID])
			{
				//ShowWorkPlaceInfoToList3();

				strStatus.Format("更新表格 %s ！数据库名称 %s 表格名称 %s 。"
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

			g_bWaitResult = false;
		}
		rst.pop();
	}
	LOGDEBUG("[%s][%d] CTechnolPlanAndWorkPlaceViewDlg::dealDBResult end!", __STRING_FUNCTION__, __LINE__);
}


BOOL CTechnolPlanAndWorkPlaceViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	////设置对话框图标
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CREO);//IDR_ICON为图标资源名
	SetIcon(m_hIcon, FALSE);
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
	m_StatusBar.SetPaneInfo(4,array[4],0,110); //
	m_StatusBar.SetPaneInfo(5,array[5],0,110); 

	CTime Time = CTime::GetCurrentTime();
	m_StatusBar.SetPaneText(0,"状态：");//设置面板文本
	m_StatusBar.SetPaneText(1,"");//设置面板文本
	m_StatusBar.SetPaneText(2,"");//设置面板文本
	m_StatusBar.SetPaneText(3,"");//设置面板文本
	m_StatusBar.SetPaneText(4,Time.Format("%Y-%m-%d"));
	m_StatusBar.SetPaneText(5,Time.Format("%H:%M:%S"));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
	SetTimer(1,1000,NULL); //设置定时器	
	//设置图标
	m_ImageList.Create(14, 14, ILC_COLOR24|ILC_MASK, 1, 1);
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_ASM)));
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PRT)));
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
	//设置列表控件2标题
	m_grid3.InsertColumn(0,_T("序号"),LVCFMT_LEFT,50,0);
	m_grid3.InsertColumn(1,_T("工位"),LVCFMT_LEFT,80,0);
	m_grid3.InsertColumn(2,_T("数量"),LVCFMT_LEFT,80,0);
	m_grid3.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	ShowPlanAndPlaceUsedTableToCombo1();//将生产计划表显示到组合框控件
	ShowWorkPlaceInfoToList3();//将工位信息添加到表3
	return TRUE;  // return TRUE unless you set the focus to a control

	// 异常: OCX 属性页应返回 FALSE
}


void CTechnolPlanAndWorkPlaceViewDlg::ShowPlanAndPlaceUsedTableToCombo1(void)
{
	//将表格显示到组合框控件
	m_combo1.ResetContent();
	std::vector<STRING> temTableName = (*m_dbService)["WorkPlaceUsedInfo"].m_TableNames;

	auto iterCol = temTableName.begin();
	size_t index = 0;
	while (iterCol != temTableName.end())
	{
		m_combo1.InsertString(index++, *(iterCol++));//插入控件
	}
	m_combo1.SetCurSel(0);
}


void CTechnolPlanAndWorkPlaceViewDlg::ShowWorkPlaceInfoToList3(void)
{
	m_grid3.DeleteAllItems();

	TableInfo temTable = (*m_dbService)["WorkPlaceInfo"]["WorkPlaceDef"];
	int32_t nRowCount = temTable.m_Values.getRowCount();

	for (int i = 0; i < nRowCount ;++i)
	{
		m_grid3.InsertItem(i, std::to_string(i + 1).c_str());
		m_grid3.SetItemText(i,1, temTable.m_Values[i][1]);
		m_grid3.SetItemText(i,2, temTable.m_Values[i][2]);
	}
}

//将生产计划和工位使用显示到列表控件
void CTechnolPlanAndWorkPlaceViewDlg::OnClickedBtnShowPlanAndWorkplaceUsedToList()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTableName;
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(strTableName);//生成计划表名称
	if ("" == strTableName)
	{
		return;
	}
	//清空列表控件中的内容
	m_grid1.DeleteAllItems();
	m_grid2.DeleteAllItems();
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

	//读取工位使用信息表格
	{
		TableInfo temTable = (*m_dbService)["WorkPlaceUsedInfo"][strTableName];
		size_t nValueRowCount = temTable.m_Values.getRowCount();
		size_t nValueColCount = temTable.m_Values.getColCount();
		//更新列表控件前2列
		for (int i = 0; i < nValueColCount - 2; ++i)
		{
			m_grid2.InsertItem(i, std::to_string(i).c_str());//插入第一列
			m_grid2.SetItemText(i, 1, temTable.m_vevCols[i + 2]);
		}
		//插入日期列
		for (int i = 0; i < nValueRowCount; ++i)
		{
			m_grid1.InsertColumn(i + 8, temTable.m_Values[i][1], LVCFMT_LEFT, 70, 0);//插入日期列
			m_grid2.InsertColumn(i + 2, temTable.m_Values[i][1], LVCFMT_LEFT, 70, 0);//插入日期列
			for (int j = 2; j < nValueColCount; ++j)
			{
				m_grid2.SetItemText(j - 2, i + 2, temTable.m_Values[i][j]);//显示工位使用情况
			}
		}
	}

	//读取生产计划
	{
		TableInfo temTable = (*m_dbService)["ProductionPlanedInfo"][strTableName];
		size_t nValueRowCount = temTable.m_Values.getRowCount();
		size_t nValueColCount = temTable.m_Values.getColCount();
		//更新列表控件1
		for (int i = 0; i < nValueRowCount; ++i)
		{
			m_grid1.InsertItem(i, temTable.m_Values[i][0]);
			for (int j = 1; j < nValueColCount; ++j)
			{
				m_grid1.SetItemText(i, j, temTable.m_Values[i][j]);
			}
		}
	}
}


void CTechnolPlanAndWorkPlaceViewDlg::OnTimer(UINT_PTR nIDEvent)
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

void CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnDeleteCurrentPlan()
{
	// TODO: 在此添加控件通知处理程序代码
	CALL_BACK_WAIT_CHECK

	CString strTableName;
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(strTableName);
	if ("" == strTableName)
	{
		return;
	}

	//删除工位使用信息中表格
	uint32_t cmdID = m_dbService->deleteTableCmd("WorkPlaceUsedInfo", strTableName);
	g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable[0]));

	//删除生产计划中表格
	cmdID = m_dbService->deleteTableCmd("ProductionPlanedInfo", strTableName);
	g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable[0]));

	m_grid1.DeleteAllItems();
	m_grid2.DeleteAllItems();
}


void CTechnolPlanAndWorkPlaceViewDlg::OnDblclkList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int i=m_grid3.GetSelectionMark();
	//如果没有选中，返回
	if(i==-1)
	{
		return;
	}
	GetDlgItem(IDC_EDIT1)->SetWindowTextA(m_grid3.GetItemText(i,1));
	GetDlgItem(IDC_EDIT2)->SetWindowTextA(m_grid3.GetItemText(i,2));

	*pResult = 0;
}


void CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnAddWorkplace()
{
	// TODO: 在此添加控件通知处理程序代码
	CALL_BACK_WAIT_CHECK//检查前一个消息是否处理完
	
	CString strName,strNum;
	int nNum(0);
	GetDlgItem(IDC_EDIT1)->GetWindowTextA(strName);
	GetDlgItem(IDC_EDIT2)->GetWindowTextA(strNum);
	nNum = atoi(strNum);
	if (strName == "" || nNum <= 0)
	{
		MessageBox("参数错误！","提示",MB_ICONERROR);
		return;
	}
	int nRowCount = m_grid3.GetItemCount();
	//检查名称是否存在
	strNum = std::to_string(nNum).c_str();
	for (int i = 0; i < nRowCount; ++i)
	{
		if (m_grid3.GetItemText(i,1) == strName)//修改
		{
			m_grid3.SetItemText(i,2,strNum);
			return SaveWorkPlaceInfoToDBList3();
		}
	}
	
	m_grid3.InsertItem(nRowCount,std::to_string(nRowCount + 1).c_str());
	m_grid3.SetItemText(nRowCount,1,strName);
	m_grid3.SetItemText(nRowCount,2,strNum);
	SaveWorkPlaceInfoToDBList3();
}


void CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnDeleteWorkplac()
{
	// TODO: 在此添加控件通知处理程序代码
	CALL_BACK_WAIT_CHECK

	int nRowIndex=m_grid3.GetSelectionMark();
	//如果没有选中，返回
	if(nRowIndex == -1)
	{
		return;
	}
	m_grid3.DeleteItem(nRowIndex);
	//重新排序
	int nRowCount = m_grid3.GetItemCount();
	for (size_t index = 0 ; index < nRowCount; )
	{
		m_grid3.SetItemText(index, 0, std::to_string(++index).c_str());
	}
	SaveWorkPlaceInfoToDBList3();

}


void CTechnolPlanAndWorkPlaceViewDlg::SaveWorkPlaceInfoToDBList3(void)
{
	int nRowCount = m_grid3.GetItemCount();
	int nColumuCount = 3;//列数量
	DynaArray2D<STRING> temValues(nRowCount, nColumuCount);
	for (int i = 0; i < nRowCount; ++i)
	{
		for (int j = 0; j < nColumuCount; ++j)
		{
			temValues[i][j] = m_grid3.GetItemText(i, j);
			//temValues = m_grid1.GetItemText(i, j + 1);
		}
	}

	uint32_t cmdID = m_dbService->updateTableCmd("WorkPlaceInfo", "WorkPlaceDef", std::move(temValues));
	g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable[1]));
}
