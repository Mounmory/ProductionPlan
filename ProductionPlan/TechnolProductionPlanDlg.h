#pragma once
#include "afxwin.h"
#include "TechnolProductionPlanResultDlg.h"


// CTechnolProductionPlanDlg �Ի���

class CTechnolProductionPlanDlg : public CDialog, public IListenner
{
	DECLARE_DYNAMIC(CTechnolProductionPlanDlg)

public:
	CTechnolProductionPlanDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTechnolProductionPlanDlg();

	// �Ի�������
	enum { IDD = IDD_TEC_PROD_PLAN };
	virtual void dealDBResult(std::queue<DBResult>& rst) override;//����������֪ͨ
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	//��Ա����
	void ShowProcessTableListToCombo1(void);
	void ShowWorkplaceToList2(void);//���б�ؼ�2����ӹ�λ��Ϣ
	void GenerateLevelID(const int nLevel);
	bool getWorkPlaceUsedInfo(const uint16_t index, const std::map<CString, uint16_t>& mapWorkPlaceInfo, std::map<uint16_t, float>& mapWorkPlaceUse, float& fTimeCount);//��ȡ��λʹ����Ϣ
	void GenerateProductionPlanAuto(
		const uint16_t nTotalNum	//��������Ŀ
		, const std::map<CString, uint16_t>& mapWorkPlIndex	//workplace info, <key:workplaceName ,value:workplace index>
		, const DynaArray2D<float>& arrWorkFree	//workplace info, free information
		, const std::vector<std::map<uint16_t, float>>& vecMapWorkPlaceUse	// material information ,map <key:workplace Index,value:workplace cost time>
		, const std::vector<float>& vecTimeToRoot	// material information ,cumulative time to root node);	
		, const std::vector<CString>& vecMertID	// material id
		, const std::vector<uint16_t>& vecMertNumPer);
	bool GetPlanWorkPlaceUsedInfo(std::vector<CString> & strVecWorkPlaName, std::vector<uint16_t> & nVecWorkPlaRowUsed, uint16_t & nDaysMin, const uint16_t nDaysMax);//����λ��Ϣ���浽���ݿ�ʱ�������Щ��λ��ռ����

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnBnClickedBtnSetProcAndWorkPlaceInfoToList1AndList2();//����������ð�ť
	afx_msg void OnBnClickedBtnGetWorkplaceInfoFromDbToList2();//��ȡ���ݿ��п��ù�λ��Ϣ
	afx_msg void OnBnClickedBtnProduPlan();//���������ƻ���ť
	afx_msg void OnBnClickedBtnShowWorkPlaceUsedToList2();//��ʾ��λʹ�õ��б�2�ؼ���ť
	afx_msg void OnBnClickedBtnSavePlanAndWorkplaceUsedInfoToDb();//����λʹ��������浽���ݿ�
	afx_msg void OnBnClickedBtnShowResult();//��ʾ���н���Ի���
private:
	TechnolProductionPlanResultDlg m_ResultDlg;	//��ʾ����Ի������
	std::shared_ptr<DBService> m_dbService;		//���ݿ����
	ThreadPool* m_threadPool;	//�̳߳ط���

	//�ؼ�����
	CImageList	m_ImageList;	//�����б�ؼ�ǰ��ͼ�����
	CStatusBar m_StatusBar;	//����״̬������
	CListCtrl m_grid1;	//�б�ؼ�1����ʾ���ϼ������ƻ���Ϣ
	CListCtrl m_grid2;	//�б�ؼ�2����ʾ��λ��Ϣ
	CComboBox m_combo1;	//��Ͽ�ؼ�����ʾ�������̱�

	//�Զ������
	std::atomic_bool m_bTreadIsRunning;//���߳��������б��
	int16_t m_nTotalProdNum;//Ҫ�Ų���������
	int16_t m_nNumPerTime;//ÿ���Ų�������
	uint16_t m_nMaxDayPlan;//���������ƻ��������
	uint16_t m_nMaxDayUsed;//���е����������

	std::vector<uint16_t> m_nVecWorkPInfo;	//��¼���ݿ��й�λ������Ϣ
	std::vector<CString> m_VecDataName;		//��¼�����������ڵ���������
	DynaArray2D<float> m_fArrWorkPlaceUsed;	//��¼���������ƻ���ʹ�õĹ�λ
};
