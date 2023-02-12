#pragma once


// TechnolProductionPlanResultDlg 对话框

class TechnolProductionPlanResultDlg : public CDialog
{
	DECLARE_DYNAMIC(TechnolProductionPlanResultDlg)

public:
	TechnolProductionPlanResultDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~TechnolProductionPlanResultDlg();

// 对话框数据
	enum { IDD = IDD_TEC_PROD_PLAN_SHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CListCtrl m_list;
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedSaveExcel();//保存到EXCEL表格按钮
	int CalculatBestSolution(const std::vector<float>& fVecTimeLong, const std::vector<float>& fVecTotalDay, const std::vector<float>& fVecTotalWorkPlace);
private:
	int m_nColNum;//列表控件中的列数量
	int m_nBestReualt;//记录最佳结果
	float m_fTimeWeight;//排列生产计划的时间权重
	DynaArray2D<CString> m_strShowListValue;//显示到对话框中信息
	CString m_strShowEditOne;//显示到编辑框控件1
	CString m_StrShowEditTwo;//显示到编辑框控件2

public:
	afx_msg void OnBnClickedSaveExcel();
	void setTimeWeight(float ftimeWeigth);//设置时间权重
};
