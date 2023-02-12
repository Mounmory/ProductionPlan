#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CTechnolProcePrepareDlg 对话框
class CTechnolProcePrepareDlg : public CDialog, public IListenner
{
	DECLARE_DYNAMIC(CTechnolProcePrepareDlg)

public:
	CTechnolProcePrepareDlg(std::shared_ptr<DBService> dbService,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTechnolProcePrepareDlg();

// 对话框数据
	enum { IDD = IDD_TEC_PROGR };
	virtual void dealDBResult(std::queue<DBResult>& rst) override;//接收数服务通知

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//成员函数
	virtual BOOL OnInitDialog();
	void ShowMdlTreeToBillOfMaterial(void);//将模型树转换到工艺流程表中
	void ShowChildTreeList(HTREEITEM Hitem);//显示子级清单
	void ShowWorkplaceToCombo1(void);

	afx_msg void OnBnClickedBtnViewTableListInDB();//查看数据库按钮
	afx_msg void OnClickedBtnSaveList1ToBd();//保存表格信息到数据库
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnAddSingleProgrToList2();
	afx_msg void OnBnClickedBtnAddEditProcToList1();
	void ShowProcessTableListToCombo2(void);
	afx_msg void OnBnClickedBtnShowProcessInfoFromDBToTable();
public:	//成员变量
	std::shared_ptr<DBService> m_dbService;//数据库服务

	//控件成员变量
	CImageList	m_ImageList;
	CStatusBar m_StatusBar;	//创建状态栏变量

#ifdef PRO_USE_VAR_ARGS
	CDpview m_CDprivew;//新加的预览控件
	CArray<ProFeature, ProFeature> p_asmfeatlist;
	CArray<ProDimension, ProDimension> p_featdimlist;
#endif

	CTreeCtrl m_Tree;//树控件变量
	CListCtrl m_grid1;//列表控件成员变量
	CListCtrl m_grid2;
	CComboBox m_combo1;
	CComboBox m_combo2;
	float m_fWorkTime;

	//自定义成员变量
	std::vector<CString> m_vecStrParents;//生成物料清单中存储父项的父项
	CString m_strModelPath;//模型所在路径
	CString m_strRootMdlName;//根模型名称


	afx_msg void OnBnClickedBtnDelProgInList2();
	afx_msg void OnBnClickedBtnDeleSelectedProgInLiset2();
	afx_msg void OnBnClickedBtnMoveUperInList2();
	afx_msg void OnBnClickedBtnMoveDownInList2();
};
