
// ProductionPlanMenuDlg.h : ͷ�ļ�
//

#pragma once


// CProductionPlanMenuDlg �Ի���
class CProductionPlanMenuDlg : public CDialogEx
{
// ����
public:
	CProductionPlanMenuDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CProductionPlanMenuDlg();


// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRODUCTIONPLANMENU_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedBtnWorkPlaceManage();
	afx_msg void OnBnClickedBtnMakePlan();
	afx_msg void OnBnClickedBtnMakeProcess();

public:
	HMODULE m_dllHandel;
};
