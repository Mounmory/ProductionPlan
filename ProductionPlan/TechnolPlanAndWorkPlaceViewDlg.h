#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CTechnolPlanAndWorkPlaceViewDlg 对话框

class CTechnolPlanAndWorkPlaceViewDlg : public CDialog, public IListenner
{
	DECLARE_DYNAMIC(CTechnolPlanAndWorkPlaceViewDlg)

public:
	CTechnolPlanAndWorkPlaceViewDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTechnolPlanAndWorkPlaceViewDlg();

// 对话框数据
	enum { IDD = IDD_TEC_VIEW_WORK_PLACE_INFO };
	virtual void dealDBResult(std::queue<DBResult>& rst) override;//接收数服务通知

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CImageList	m_ImageList;	//创建列表控件前的图标对象
	CStatusBar m_StatusBar;	//创建状态栏变量
	CComboBox m_combo1;
	CListCtrl m_grid1;
	CListCtrl m_grid2;
	CListCtrl m_grid3;

	std::shared_ptr<DBService> m_dbService;

	void ShowPlanAndPlaceUsedTableToCombo1(void);
	void ShowWorkPlaceInfoToList3(void);
	void SaveWorkPlaceInfoToDBList3(void);

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClickedBtnShowPlanAndWorkplaceUsedToList();
	afx_msg void OnBnClickedBtnDeleteCurrentPlan();

	afx_msg void OnDblclkList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnAddWorkplace();
	afx_msg void OnBnClickedBtnDeleteWorkplac();

};
