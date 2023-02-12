#pragma once
#include "afxwin.h"


// CTechnolProceDBNameSaveDlg 对话框

class CTechnolProceDBNameSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CTechnolProceDBNameSaveDlg)

public:
	//CTechnolProceDBNameSaveDlg(CWnd* pParent = NULL);   // 标准构造函数
	CTechnolProceDBNameSaveDlg(CString strDBName/* = ""*/, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTechnolProceDBNameSaveDlg();

// 对话框数据
	enum { IDD = IDD_TEC_SAVE_DB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_Combo1;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CString m_strTableName;

private:
	std::shared_ptr<DBService> m_dbService;
	CString m_strDataBaseName;//数据库名称
};
