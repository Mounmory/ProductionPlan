#pragma once


// TechnolProductionPlanResultDlg �Ի���

class TechnolProductionPlanResultDlg : public CDialog
{
	DECLARE_DYNAMIC(TechnolProductionPlanResultDlg)

public:
	TechnolProductionPlanResultDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~TechnolProductionPlanResultDlg();

// �Ի�������
	enum { IDD = IDD_TEC_PROD_PLAN_SHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	CListCtrl m_list;
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnBnClickedSaveExcel();//���浽EXCEL���ť
	int CalculatBestSolution(const std::vector<float>& fVecTimeLong, const std::vector<float>& fVecTotalDay, const std::vector<float>& fVecTotalWorkPlace);
private:
	int m_nColNum;//�б�ؼ��е�������
	int m_nBestReualt;//��¼��ѽ��
	float m_fTimeWeight;//���������ƻ���ʱ��Ȩ��
	DynaArray2D<CString> m_strShowListValue;//��ʾ���Ի�������Ϣ
	CString m_strShowEditOne;//��ʾ���༭��ؼ�1
	CString m_StrShowEditTwo;//��ʾ���༭��ؼ�2

public:
	afx_msg void OnBnClickedSaveExcel();
	void setTimeWeight(float ftimeWeigth);//����ʱ��Ȩ��
};
