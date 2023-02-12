#pragma once
#include "afxwin.h"


// CTechnolProceDBNameSaveDlg �Ի���

class CTechnolProceDBNameSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CTechnolProceDBNameSaveDlg)

public:
	//CTechnolProceDBNameSaveDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CTechnolProceDBNameSaveDlg(CString strDBName/* = ""*/, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTechnolProceDBNameSaveDlg();

// �Ի�������
	enum { IDD = IDD_TEC_SAVE_DB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_Combo1;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CString m_strTableName;

private:
	std::shared_ptr<DBService> m_dbService;
	CString m_strDataBaseName;//���ݿ�����
};
