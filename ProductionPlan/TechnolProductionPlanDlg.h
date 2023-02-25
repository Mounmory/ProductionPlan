#pragma once
#include "afxwin.h"
#include "TechnolProductionPlanResultDlg.h"


// CTechnolProductionPlanDlg 对话框

class CTechnolProductionPlanDlg : public CDialog, public IListenner
{
	DECLARE_DYNAMIC(CTechnolProductionPlanDlg)

public:
	CTechnolProductionPlanDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTechnolProductionPlanDlg();

	// 对话框数据
	enum { IDD = IDD_TEC_PROD_PLAN };
	virtual void dealDBResult(std::queue<DBResult>& rst) override;//接收数服务通知
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	//成员函数
	void ShowProcessTableListToCombo1(void);
	void ShowWorkplaceToList2(void);//向列表控件2中添加工位信息
	void GenerateLevelID(const int nLevel);
	bool getWorkPlaceUsedInfo(const uint16_t index, const std::map<CString, uint16_t>& mapWorkPlaceInfo, std::map<uint16_t, float>& mapWorkPlaceUse, float& fTimeCount);//获取工位使用信息
	void GenerateProductionPlanAuto(
		const uint16_t nTotalNum	//总排列数目
		, const std::map<CString, uint16_t>& mapWorkPlIndex	//workplace info, <key:workplaceName ,value:workplace index>
		, const DynaArray2D<float>& arrWorkFree	//workplace info, free information
		, const std::vector<std::map<uint16_t, float>>& vecMapWorkPlaceUse	// material information ,map <key:workplace Index,value:workplace cost time>
		, const std::vector<float>& vecTimeToRoot	// material information ,cumulative time to root node);	
		, const std::vector<CString>& vecMertID	// material id
		, const std::vector<uint16_t>& vecMertNumPer);
	bool GetPlanWorkPlaceUsedInfo(std::vector<CString> & strVecWorkPlaName, std::vector<uint16_t> & nVecWorkPlaRowUsed, uint16_t & nDaysMin, const uint16_t nDaysMax);//将工位信息保存到数据库时，检查哪些工位被占用了

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnBnClickedBtnSetProcAndWorkPlaceInfoToList1AndList2();//完成生产设置按钮
	afx_msg void OnBnClickedBtnGetWorkplaceInfoFromDbToList2();//获取数据库中可用工位信息
	afx_msg void OnBnClickedBtnProduPlan();//排列生产计划按钮
	afx_msg void OnBnClickedBtnShowWorkPlaceUsedToList2();//显示工位使用到列表2控件按钮
	afx_msg void OnBnClickedBtnSavePlanAndWorkplaceUsedInfoToDb();//将工位使用情况保存到数据库
	afx_msg void OnBnClickedBtnShowResult();//显示排列结果对话框
private:
	TechnolProductionPlanResultDlg m_ResultDlg;	//显示结果对话框对象
	std::shared_ptr<DBService> m_dbService;		//数据库服务
	ThreadPool* m_threadPool;	//线程池服务

	//控件变量
	CImageList	m_ImageList;	//创建列表控件前的图标对象
	CStatusBar m_StatusBar;	//创建状态栏变量
	CListCtrl m_grid1;	//列表控件1，显示物料及生产计划信息
	CListCtrl m_grid2;	//列表控件2，显示工位信息
	CComboBox m_combo1;	//组合框控件，显示工艺流程表

	//自定义变量
	std::atomic_bool m_bTreadIsRunning;//多线程正在运行标记
	int16_t m_nTotalProdNum;//要排产的总数量
	int16_t m_nNumPerTime;//每次排产的数量
	uint16_t m_nMaxDayPlan;//排列生产计划最大天数
	uint16_t m_nMaxDayUsed;//排列的最长生产周期

	std::vector<uint16_t> m_nVecWorkPInfo;	//记录数据库中工位数量信息
	std::vector<CString> m_VecDataName;		//记录所有排列所在的所有日期
	DynaArray2D<float> m_fArrWorkPlaceUsed;	//记录排列生产计划所使用的工位
};
