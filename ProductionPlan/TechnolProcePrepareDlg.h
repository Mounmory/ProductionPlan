#pragma once
#include "afxcmn.h"
#include "afxwin.h"

// CTechnolProcePrepareDlg �Ի���
class CTechnolProcePrepareDlg : public CDialog, public IListenner
{
	DECLARE_DYNAMIC(CTechnolProcePrepareDlg)

public:
	CTechnolProcePrepareDlg(std::shared_ptr<DBService> dbService,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTechnolProcePrepareDlg();

// �Ի�������
	enum { IDD = IDD_TEC_PROGR };
	virtual void dealDBResult(std::queue<DBResult>& rst) override;//����������֪ͨ

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	//��Ա����
	virtual BOOL OnInitDialog();
	void ShowMdlTreeToBillOfMaterial(void);//��ģ����ת�����������̱���
	void ShowChildTreeList(HTREEITEM Hitem);//��ʾ�Ӽ��嵥
	void ShowWorkplaceToCombo1(void);

	afx_msg void OnBnClickedBtnViewTableListInDB();//�鿴���ݿⰴť
	afx_msg void OnClickedBtnSaveList1ToBd();//��������Ϣ�����ݿ�
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnAddSingleProgrToList2();
	afx_msg void OnBnClickedBtnAddEditProcToList1();
	void ShowProcessTableListToCombo2(void);
	afx_msg void OnBnClickedBtnShowProcessInfoFromDBToTable();
public:	//��Ա����
	std::shared_ptr<DBService> m_dbService;//���ݿ����

	//�ؼ���Ա����
	CImageList	m_ImageList;
	CStatusBar m_StatusBar;	//����״̬������

#ifdef PRO_USE_VAR_ARGS
	CDpview m_CDprivew;//�¼ӵ�Ԥ���ؼ�
	CArray<ProFeature, ProFeature> p_asmfeatlist;
	CArray<ProDimension, ProDimension> p_featdimlist;
#endif

	CTreeCtrl m_Tree;//���ؼ�����
	CListCtrl m_grid1;//�б�ؼ���Ա����
	CListCtrl m_grid2;
	CComboBox m_combo1;
	CComboBox m_combo2;
	float m_fWorkTime;

	//�Զ����Ա����
	std::vector<CString> m_vecStrParents;//���������嵥�д洢����ĸ���
	CString m_strModelPath;//ģ������·��
	CString m_strRootMdlName;//��ģ������


	afx_msg void OnBnClickedBtnDelProgInList2();
	afx_msg void OnBnClickedBtnDeleSelectedProgInLiset2();
	afx_msg void OnBnClickedBtnMoveUperInList2();
	afx_msg void OnBnClickedBtnMoveDownInList2();
};
