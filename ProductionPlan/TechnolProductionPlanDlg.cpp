// TechnolProductionPlanDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolProductionPlanDlg.h"
#include "TechnolProceDBNameSaveDlg.h"
#include "afxdialogex.h"

#include "ProductionPlanCalculate.h"//���������ƻ�ר��

static std::map<uint32_t, STRING> g_mapDealResult;//<key:cmdID,value:strCmd>����ִ�н��������һ������
static const STRING updateProcTable ;//��ʾ��״̬��
																//����״̬��
static StatusData g_statuData;

//��鲢�ر���ʾ����Ի���
#define RESULT_WINDOW_CHECK_CLOSE \
if (IsWindow(m_ResultDlg.m_hWnd))\
	m_ResultDlg.DestroyWindow();\



// CTechnolProductionPlanDlg �Ի���

IMPLEMENT_DYNAMIC(CTechnolProductionPlanDlg, CDialog)

CTechnolProductionPlanDlg::CTechnolProductionPlanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTechnolProductionPlanDlg::IDD, pParent)
	, m_dbService(getProPlanDBService())
	, m_threadPool(getThreadPoolService())
	,m_bTreadIsRunning(false)
	,m_nMaxDayPlan(60)
{
	m_dbService->addListener(this);
}

CTechnolProductionPlanDlg::~CTechnolProductionPlanDlg()
{
	m_dbService->removeListener(this);
}

void CTechnolProductionPlanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo1);
	DDX_Control(pDX, IDC_LIST1, m_grid1);
	DDX_Control(pDX, IDC_LIST2, m_grid2);
}


BEGIN_MESSAGE_MAP(CTechnolProductionPlanDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CTechnolProductionPlanDlg::OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BTN_SET_PROC_INFO, &CTechnolProductionPlanDlg::OnBnClickedBtnSetProcAndWorkPlaceInfoToList1AndList2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_GET_WORKPLACE_FROM_DB, &CTechnolProductionPlanDlg::OnBnClickedBtnGetWorkplaceInfoFromDbToList2)
	ON_BN_CLICKED(IDC_BTN_PRODU_PLAN, &CTechnolProductionPlanDlg::OnBnClickedBtnProduPlan)
	ON_BN_CLICKED(IDC_BTN_SHOW_WORK_PLACE_USED, &CTechnolProductionPlanDlg::OnBnClickedBtnShowWorkPlaceUsedToList2)
	ON_BN_CLICKED(IDC_BTN_PLAN_AND_WORK_TO_DB, &CTechnolProductionPlanDlg::OnBnClickedBtnSavePlanAndWorkplaceUsedInfoToDb)
	ON_BN_CLICKED(IDC_BTN_SHOW_RESULT, &CTechnolProductionPlanDlg::OnBnClickedBtnShowResult)
END_MESSAGE_MAP()


// CTechnolProductionPlanDlg ��Ϣ�������
void CTechnolProductionPlanDlg::dealDBResult(std::queue<DBResult>& rst)
{
	LOGDEBUG("[%s][%d] begin, result size %d !", __STRING_FUNCTION__, __LINE__, rst.size());

	while (rst.size() > 0)
	{
		auto iterRst = rst.front();
		if (g_mapDealResult.find(iterRst.cmdID) != g_mapDealResult.end())
		{
			//����״̬��
			CString strStatus;
			if (updateProcTable == g_mapDealResult[iterRst.cmdID])
			{
				strStatus.Format("��ӱ�� %s �����ݿ����� %s ������� %s ��"
					, iterRst.bResult ? "�ɹ�" : "ʧ��"
					, iterRst.strDBName
					, iterRst.strTableName);
			}
			else
			{
			}
			m_StatusBar.SetPaneText(1, strStatus);
			//��ʾ����
			g_statuData.strInfo = strStatus;
			g_statuData.nTimeCount = 15;//��ʾ15s
			g_mapDealResult.erase(iterRst.cmdID);
		}
		rst.pop();
	}
	LOGDEBUG("[%s][%d] CTechnolPlanAndWorkPlaceViewDlg::dealDBResult end!", __STRING_FUNCTION__, __LINE__);
}

BOOL CTechnolProductionPlanDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	////���öԻ���ͼ��
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CREO);//IDR_ICONΪͼ����Դ��
	SetIcon(m_hIcon, FALSE);
	
	//����ͼ��
	m_ImageList.Create(14, 14, ILC_COLOR24 | ILC_MASK, 1, 1);
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_ASM)));
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PRT)));
	
	//����״̬��
	UINT array[8];
	for(int i=0;i<8;i++)
	{
		array[i] = 1001 + i;
	}
	m_StatusBar.Create(this); //����״̬������
	m_StatusBar.SetIndicators(array,sizeof(array)/sizeof(UINT)); //������
	m_StatusBar.SetPaneInfo(0,array[0],0,70); //���������
	m_StatusBar.SetPaneInfo(1,array[1],0,700); //���������
	m_StatusBar.SetPaneInfo(2,array[2],0,100); //
	m_StatusBar.SetPaneInfo(3,array[3],0,100); 
	m_StatusBar.SetPaneInfo(4,array[4],0,150); //
	m_StatusBar.SetPaneInfo(5,array[5],0,150); 

	CTime Time = CTime::GetCurrentTime();
	m_StatusBar.SetPaneText(0,"״̬��");//��������ı�
	m_StatusBar.SetPaneText(1,"");//��������ı�
	m_StatusBar.SetPaneText(2,"");//��������ı�
	m_StatusBar.SetPaneText(3,"");//��������ı�
	m_StatusBar.SetPaneText(4,Time.Format("%Y-%m-%d"));
	m_StatusBar.SetPaneText(5,Time.Format("%H:%M:%S"));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
	SetTimer(1,1000,NULL); //���ö�ʱ��	

	//�����б�ؼ�1����
	m_grid1.InsertColumn(0,_T("���"),LVCFMT_LEFT,40,0);
	m_grid1.InsertColumn(1,_T("�㼶"),LVCFMT_LEFT,75,0);
	m_grid1.InsertColumn(2,_T("��ʶ��"),LVCFMT_LEFT,85,0);
	m_grid1.InsertColumn(3,_T("���ϱ��"),LVCFMT_LEFT,80,0);
	m_grid1.InsertColumn(4,_T("��������"),LVCFMT_LEFT,100,0);
	m_grid1.InsertColumn(5,_T("����(PC)"),LVCFMT_LEFT,65,0);
	m_grid1.InsertColumn(6,_T("��������"),LVCFMT_LEFT,150,0);
	m_grid1.InsertColumn(7,_T("��ʱ(h/PC)"),LVCFMT_LEFT,150,0);
	m_grid1.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	//�����б�ؼ�2���⣬�����õ�һ������
	m_grid2.InsertColumn(0,_T("���"),LVCFMT_LEFT,40,0);
	m_grid2.InsertColumn(1,_T("��λ"),LVCFMT_LEFT,60,0);
	m_grid2.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	
	//���õ�ѡ��ťͼ��
	CButton* radio=(CButton*)GetDlgItem(IDC_RADIO2);
	radio->SetCheck(1);
	
	//������Ͽ�ؼ�2
	CComboBox* combo=(CComboBox*)GetDlgItem(IDC_COMBO2);
	combo->ResetContent();
	combo->InsertString(0, "30");
	combo->InsertString(1, "60");
	combo->InsertString(2, "90");
	combo->InsertString(3, "120");
	combo->SetCurSel(1); //Ĭ�������������

	//�����ݿ��б����ʾ����Ͽ�ؼ�1
	ShowProcessTableListToCombo1();//���������̱���Ϣ��ʾ����Ͽ�ؼ�1��
	ShowWorkplaceToList2();//����λ��Ϣ��ʾ���б�ؼ�2��

	GetDlgItem(IDC_BTN_SET_PROC_INFO)->EnableWindow(FALSE);//��������Ų���ť������
	GetDlgItem(IDC_BTN_PRODU_PLAN)->EnableWindow(FALSE);//���������ƻ���ť������
	GetDlgItem(IDC_BTN_SHOW_RESULT)->EnableWindow(FALSE);//��ʾ�鿴�����ť
	GetDlgItem(IDC_BTN_SHOW_WORK_PLACE_USED)->EnableWindow(FALSE);//��ʾ�鿴ʹ�ù�λ�����ť
	GetDlgItem(IDC_BTN_PLAN_AND_WORK_TO_DB)->EnableWindow(FALSE);//���湤λʹ�õ����ݿⰴť

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CTechnolProductionPlanDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CTime Time;
	Time = CTime::GetCurrentTime();

	m_StatusBar.SetPaneText(4,Time.Format("%Y-%m-%d"));
	m_StatusBar.SetPaneText(5,Time.Format("%H:%M:%S"));

	//����״̬
	if (g_statuData.nTimeCount > 0)
	{
		CString temStr = g_statuData.strInfo + "(" + std::to_string(g_statuData.nTimeCount--).c_str() + ")";
		m_StatusBar.SetPaneText(1, temStr);
	}
	else
	{
		m_StatusBar.SetPaneText(1, "");
	}
	CDialog::OnTimer(nIDEvent);
}

//����Ͽ�ؼ�����ʾ���ݿ��б��
void CTechnolProductionPlanDlg::ShowProcessTableListToCombo1(void)
{
	//�������ʾ����Ͽ�ؼ�
	m_combo1.ResetContent();
	std::vector<STRING> temCols = (*m_dbService)["ProductionProcessInfo"].m_TableNames;
	auto iterCol = temCols.begin();
	size_t index = 0;
	while (iterCol != temCols.end())
	{
		m_combo1.InsertString(index++, *(iterCol++));//����ؼ�
	}
	//m_combo1.SetCurSel(0);
	m_combo1.SetCurSel(-1);
}

//��Ͽ�ؼ��޸��꣬��ʾ�����ƻ�
void CTechnolProductionPlanDlg::OnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(true == m_bTreadIsRunning)	//����߳��Ƿ���������
	{
		MessageBox("�������������ƻ���","��ʾ",MB_ICONSTOP);
		return;
	}

	//�������Ի�����ڣ����ٴ���
	RESULT_WINDOW_CHECK_CLOSE

	m_grid1.SetImageList(&m_ImageList,LVSIL_SMALL);//����ͼ��
	m_grid1.DeleteAllItems();

	CString strTableName;
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(strTableName);//��ȡ�������
	TableInfo temTable = (*m_dbService)["ProductionProcessInfo"][strTableName];
	size_t nValueRowCount = temTable.m_Values.getRowCount();
	size_t nValueColCount = temTable.m_Values.getColCount();
	//���������

	//��ʾ�������
	int nImageIndex = 0;//ͼ������
	int nMerCount = 0;//��Ҫ���е������������⹺���ϲ�����
	bool hasEmptyProc = false;//�Ƿ��п�ȱ��������
	for (int i = 0; i < nValueRowCount; ++i)
	{
		CString strProcess = temTable.m_Values[i][8];
		if (strProcess.Find("�⹺") != -1)//�⹺�����żƻ�
		{
			continue;
		}
		if (strProcess == "")
		{
			hasEmptyProc = true;
		}

		//����ģ������ͼƬ
		temTable.m_Values[i][5] == "asm" ? nImageIndex = 0 : nImageIndex = 1;/////ģ������
		//m_grid1.InsertItem(nMerCount,temTable.m_Values[i][0],nImageIndex);	//ͼƬ������(��һ��ͼƬ)
		m_grid1.InsertItem(nMerCount, std::to_string(nMerCount + 1).c_str(), nImageIndex);	//ͼƬ������(��һ��ͼƬ)
		m_grid1.SetItemText(nMerCount, 1, temTable.m_Values[i][1]);//�㼶
		m_grid1.SetItemText(nMerCount, 2, "00");//���ϱ�ʶ����ʼ2�ַ���Ϊ"00"
		m_grid1.SetItemText(nMerCount, 3, temTable.m_Values[i][2]);//ģ��ID
		m_grid1.SetItemText(nMerCount, 4, temTable.m_Values[i][3]);//��������
		m_grid1.SetItemText(nMerCount, 5, temTable.m_Values[i][6]);//����
		m_grid1.SetItemText(nMerCount, 6, temTable.m_Values[i][8]);//��������
		m_grid1.SetItemText(nMerCount, 7, temTable.m_Values[i][9]);//����ʱ��
		++nMerCount;
	}

	GetDlgItem(IDC_BTN_SET_PROC_INFO)->EnableWindow(TRUE);//��������Ų���ť����
	GetDlgItem(IDC_BTN_PRODU_PLAN)->EnableWindow(FALSE);//���������ƻ���ť������
	GetDlgItem(IDC_BTN_SHOW_RESULT)->EnableWindow(FALSE);//��ʾ�鿴�����ť
	GetDlgItem(IDC_BTN_SHOW_WORK_PLACE_USED)->EnableWindow(FALSE);//��ʾ�鿴ʹ�ù�λ�����ť
	GetDlgItem(IDC_BTN_PLAN_AND_WORK_TO_DB)->EnableWindow(FALSE);//���湤λʹ�õ����ݿⰴť
	//��鹤���й������̱��Ƿ���Ƿȱ
	if (true == hasEmptyProc)
	{
		MessageBox("�������̱��й���������Ƿȱ", "��ʾ", MB_ICONERROR);
		GetDlgItem(IDC_BTN_SET_PROC_INFO)->EnableWindow(FALSE);
		return;
	}

	//�������ϱ�ʶ
	int nLevel = 4;
	GenerateLevelID(nLevel);
}


void CTechnolProductionPlanDlg::GenerateLevelID(const int nLevel)
{
	std::vector<CString> strID(m_grid1.GetItemCount());
	int j=1;//��¼��j���������ֵ
	//int dengyu=0,dayu=0;///ͳ�Ƶ��ڲ㼶����ڲ㼶������Ϊ�㣬˵���㼶��������ˣ��˳�ѭ��
	bool isequal = false;
	bool ishigh = false;
	CString strtemp;
	for(int i=0;i<m_grid1.GetItemCount();i++)
	{
		strtemp=m_grid1.GetItemText(i,1);
		if(strtemp.GetLength() < nLevel)//С�ڼ��㼶����ʾ����һ�㼶û����������������������ֵΪ1��������һѭ��
		{
			j=1;
			continue;
		}
		else if(strtemp.GetLength() == nLevel)//���ڼ��㼶
		{
			strID[i].Format("%s%02d",m_grid1.GetItemText(i,2),j);
			m_grid1.SetItemText(i,2,strID[i]);
			j++;
			isequal =true;
		}
		else//���ڼ��㼶���������ϲ�һ��
		{
			strID[i]=m_grid1.GetItemText(i-1,2);
			m_grid1.SetItemText(i,2,strID[i]);
			ishigh =true;
		}
	}
	if(isequal==true || ishigh ==true)	////ѭ�������б�Ų������ݹ鵽��һ�㼶
		return GenerateLevelID(nLevel+4);
	else	////ѭ��ʱ�ޱ�Ų�������ֹѭ��	
		return;
}

//����������ð�ť
void CTechnolProductionPlanDlg::OnBnClickedBtnSetProcAndWorkPlaceInfoToList1AndList2()//�������ô���֤������ȷ��
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if(true == m_bTreadIsRunning)	//����߳��Ƿ���������
	{
		MessageBox("�������������ƻ���","��ʾ",MB_ICONSTOP);
		return;
	}
	CString strTotalNum, strDate, strPerNum, strMaxDay;
	GetDlgItem(IDC_EDIT1)->GetWindowTextA(strTotalNum);//��ȡ����
	GetDlgItem(IDC_DATETIMEPICKER1)->GetWindowTextA(strDate);//��ȡ����
	GetDlgItem(IDC_COMBO2)->GetWindowTextA(strMaxDay);//��ȡ�����������
	GetDlgItem(IDC_EDIT2)->GetWindowTextA(strPerNum);//��ȡÿ���Ų�����
	if(strTotalNum == "")
	{
		MessageBox("������Ϣ������Ϊ�գ�","��ʾ",MB_ICONSTOP);
		return;
	}
	m_nTotalProdNum = atoi(strTotalNum);
	m_nMaxDayPlan = atoi(strMaxDay);
	if(m_nTotalProdNum <= 0)
	{
		MessageBox("��������ӦΪ����0��������","��ʾ");
		return;
	}
	///////////////////******************���ݰ�ť�ؼ����������з�ʽ*************////
	CButton* radio=(CButton*)GetDlgItem(IDC_RADIO1);
	if(radio->GetCheck()==1)//����ÿ��̶���������
	{
		GetDlgItem(IDC_EDIT3)->SetWindowText("");//����ʱ��Ȩ������Ϊ��
		m_nNumPerTime = atoi(strPerNum);//ÿ������������
		if(m_nNumPerTime <= 0 || m_nNumPerTime > m_nTotalProdNum)
		{
			MessageBox("��������ֵ����","��ʾ");
			return;
		}
	}
	else//�������������ƻ�
	{
		GetDlgItem(IDC_EDIT2)->SetWindowText("");////�������õ�һ������Ϊ��
		/////ʱ��Ȩ������Ϊ�Զ����ã��ʴ���ע�͵���
		CString strTimeWeight;
		GetDlgItem(IDC_EDIT3)->GetWindowText(strTimeWeight);
		if ("" != strTimeWeight)
		{
			float fTimeWeight = static_cast<float>(atof(strTimeWeight));//��doubleǿ��ת��Ϊfloat��ʽ
			if (fTimeWeight < 0 || fTimeWeight > 1)
			{
				MessageBox("ʱ��Ȩ�ز��Ϸ������������룡");
				return;
			}
			else 
			{
				m_ResultDlg.setTimeWeight(fTimeWeight);//����ʱ��Ȩ��
			}
		}
	}

	//�����б�ؼ�1�е��Ų�����
	int nOrigNum = atoi(m_grid1.GetItemText(0,5));//ÿ���������̱��г�ʼ������һ�����Ϊ1�����ε㰴ť���ܻ�Ϊ���ֵ
	int nRowCount1 = m_grid1.GetItemCount();//�����嵥�������
	int nTempNum = 0;//����ѭ���б�����ʱ����
	CString strTemp;//����ѭ���б�����ʱ����
	for(int i = 0; i < nRowCount1; ++i)
	{
		nTempNum = atoi(m_grid1.GetItemText(i,5));
		m_grid1.SetItemText(i,5,std::to_string(nTempNum * m_nTotalProdNum / nOrigNum).c_str());
	}
	//ɾ���ɵ������У�����У�
	CHeaderCtrl* pHeaderCtrl=m_grid1.GetHeaderCtrl();
	int nColumn=pHeaderCtrl-> GetItemCount();
	if(nColumn > 8)
	{
		for(int i = nColumn; i > 7; --i)
		{
			m_grid1.DeleteColumn(i);
			m_grid2.DeleteColumn(i-6);//���2�ȱ��1��6��
		}
	}
	//����µ�������
	strDate = GlobalFunction::translatDay(strDate);
	m_VecDataName.clear();
	for(int i = 0; i < m_nMaxDayPlan; ++i)
	{
		m_VecDataName.push_back(strDate);
		m_grid1.InsertColumn(i+8,strDate,LVCFMT_LEFT,70,0);
		m_grid2.InsertColumn(i+2,strDate,LVCFMT_LEFT,70,0);
		strDate = GlobalFunction::nextDay(strDate);
	}
	//�����ù�λ��Ϣ��ʾ���б�2
	OnBnClickedBtnGetWorkplaceInfoFromDbToList2();
	//�޸Ĺ�λ��Ϣ��Ͽ����
	//GetDlgItem(IDC_GW_INFO)->SetWindowText("��λ��Ϣ��8h/��λ����");
	//�������н��Ϊ��
	GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA("");
	//���ء��鿴���мƻ���ť��
	GetDlgItem(IDC_BTN_SHOW_RESULT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_PRODU_PLAN)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SHOW_WORK_PLACE_USED)->EnableWindow(FALSE);//�鿴��λʹ�������ť
	GetDlgItem(IDC_BTN_PLAN_AND_WORK_TO_DB)->EnableWindow(FALSE);//���湤λʹ����������ݿⰴť

}

void CTechnolProductionPlanDlg::ShowWorkplaceToList2(void)
{
	TableInfo temTable = (*m_dbService)["WorkPlaceInfo"]["WorkPlaceDef"];
	int32_t nRowCount = temTable.m_Values.getRowCount();
	m_nVecWorkPInfo.clear();
	m_nVecWorkPInfo.reserve(nRowCount);
	for (int i = 0; i < nRowCount; ++i)
	{
		m_grid2.InsertItem(i, temTable.m_Values[i][0]);
		m_grid2.SetItemText(i, 1, temTable.m_Values[i][1]);
		//m_grid2.SetItemText(i, 2, temTable.m_Values[i][2]);
		m_nVecWorkPInfo.emplace_back(_ttoi(temTable.m_Values[i][2]));//��¼��λ����
	}
}


void CTechnolProductionPlanDlg::OnBnClickedBtnGetWorkplaceInfoFromDbToList2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	if(true==m_bTreadIsRunning)	//����߳��Ƿ���������
	if(true == m_bTreadIsRunning)	//����߳��Ƿ���������
	{
		MessageBox("�������������ƻ���","��ʾ",MB_ICONSTOP);
		return;
	}
	//ɾ���ɵ������У�����У�
	CHeaderCtrl* pHeaderCtrl = m_grid2.GetHeaderCtrl();
	int nColumnCount = pHeaderCtrl-> GetItemCount();//������
	int nRowCount = m_grid2.GetItemCount();//������
	if(nColumnCount <=2)//����б�ؼ�2������
	{
		MessageBox("�������ÿ������ڣ�","��ʾ",MB_ICONSTOP);
		return;
	}
	if( nRowCount != m_nVecWorkPInfo.size())
	{
		MessageBox("��λ��Ϣ��ȡ����","��ʾ",MB_ICONSTOP);
		return;
	}
	//��ȡ�����ƻ���ʼʱ��
	CString strStartTime;
	{
		HDITEM pHeadItem;  
		pHeadItem.mask = HDI_TEXT;
		TCHAR  lpBuffer[64];
		pHeadItem.pszText = lpBuffer;
		pHeadItem.cchTextMax = 64;
		pHeaderCtrl->GetItem(2,&pHeadItem);
		strStartTime = pHeadItem.pszText;//��ȡ�����ƻ���ʼʱ��
	}
	//�����ݿ��ж�ȡ��λ��Ϣ
	std::map<CString,int> mapDataIndex;//�����������<key:data,value:index>
	std::map<CString,int> mapWorkPlaceIndex;//��λ����<key:workplacename,value:index>
	{
		//������������
		CString strTempDate = strStartTime;
		for (int i = 0; i < nColumnCount - 2; ++i)
		{
			mapDataIndex[strTempDate] = i;
			strTempDate = GlobalFunction::nextDay(strTempDate);
		}
		//������λ����
		for (int i = 0; i < nRowCount; ++i)
		{
			mapWorkPlaceIndex[m_grid2.GetItemText(i,1)] = i;
		}
	}
	//�������ݿ��й�λʹ�õ�ֵ
	DynaArray2D<float> fArrUsedWorkPlaceInfo(nRowCount,nColumnCount-2);//������ʹ�õĹ�λ
	fArrUsedWorkPlaceInfo.zeroArray();//����

	DataBaseInfo temDatabase = (*m_dbService)["WorkPlaceUsedInfo"];
	for (auto iterTabel : temDatabase.m_tables)//�������
	{
		int nValueRowCount = iterTabel.second.m_Values.getRowCount();
		int nValueColCount = iterTabel.second.m_Values.getColCount();

		for (int i = nValueRowCount - 1; i >= 0; --i)//�����һ�б���,�����ɴ�С
		{
			if (iterTabel.second.m_Values[i][1] < strStartTime)//����������������
			{
				break;
			}
			for (int j = 2; j < nValueColCount; ++j)//��һ������ţ��ڶ��������ڣ������п�ʼ�ǹ�λ����
			{
				auto iterDataIndex = mapDataIndex.find(iterTabel.second.m_Values[i][1]);//��ȡ��������
				auto iterWorkPlacIndex = mapWorkPlaceIndex.find(iterTabel.second.m_vevCols[j]);//��ȡ��λ����
				if (iterDataIndex != mapDataIndex.end() && iterWorkPlacIndex != mapWorkPlaceIndex.end())
				{
					float fTempData = atof(iterTabel.second.m_Values[i][j]);//��ȡ��λʹ��ֵ
					fArrUsedWorkPlaceInfo[iterWorkPlacIndex->second][iterDataIndex->second] += fTempData;
				}
			}
		}
	}

	//�����ù�λ��Ϣ��ʾ�����ݿ�
	CString tmpWork;//��ʱ�ַ���
	float fWorkPlaceFree(0);//����Ĺ�λ
	for (int i = 2; i<nColumnCount; ++i)
	{
		for (int j = 0; j < nRowCount; ++j)
		{
			fWorkPlaceFree = static_cast<float>(m_nVecWorkPInfo[j]) - fArrUsedWorkPlaceInfo[j][i-2];
			if (fWorkPlaceFree > 0)
			{
				tmpWork.Format("%.2f",fWorkPlaceFree);//�п���Ĺ�λ
				m_grid2.SetItemText(j,i,tmpWork);
			}
			else
			{
				m_grid2.SetItemText(j,i,"");
			}
		}
	}
	GetDlgItem(IDC_GW_INFO)->SetWindowText("���๤λ��Ϣ��8h/��λ����");
}

#define IN_VALUE
void CTechnolProductionPlanDlg::OnBnClickedBtnProduPlan()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (true == m_bTreadIsRunning)	//����߳��Ƿ���������
	{
		MessageBox("�������������ƻ���", "��ʾ", MB_ICONSTOP);
		return;
	}
	GetDlgItem(IDC_BTN_PRODU_PLAN)->EnableWindow(FALSE);//����ť�����ã�����ִ�гɹ�����ʧ�ܣ�

	//��ȡ�������̱�����������
	CHeaderCtrl* pHeaderCtrl1 = m_grid1.GetHeaderCtrl();
	uint16_t nColCount1 = pHeaderCtrl1->GetItemCount();//���1������,1-8��Ϊ������Ϣ
	uint16_t nRowCount1 = m_grid1.GetItemCount();//���1������

	//��ȡ��λ�������������
	CHeaderCtrl* pHeaderCtrl2 = m_grid2.GetHeaderCtrl();
	uint16_t nColCount2 = pHeaderCtrl2->GetItemCount();//���2������,1-2��Ϊ��λ��Ϣ
	uint16_t nRowCount2 = m_grid2.GetItemCount();//���2������

	//��ȡ��λ��Ϣ��ţ�������
	IN_VALUE std::map<CString, uint16_t> mapWorkPlaceIndex; //<key:workplaceName ,value:workplace index>
	for (uint16_t nIndex = 0; nIndex < nRowCount2; ++nIndex)
	{
		mapWorkPlaceIndex.insert(std::make_pair(m_grid2.GetItemText(nIndex, 1), nIndex));
	}
		
	//��ȡ���๤λ��Ϣ
	IN_VALUE DynaArray2D<float> arrWorkPlaceFree(nRowCount2, nColCount2 - 2);
	for (int i = 0; i < nRowCount2; ++i)
	{
		for (int j = 2; j < nColCount2; ++j)
		{
			arrWorkPlaceFree[i][j - 2] = 8 * static_cast<float>(atof(m_grid2.GetItemText(i, j)));
		}
	}

	//��ȡ�������̱��й�λʹ�ü�ʱ��
	IN_VALUE std::vector<std::map<uint16_t, float>> vecMapWorkPlaceUse;//map <key:workplace Index,value:workplace cost time>
	vecMapWorkPlaceUse.reserve(nRowCount1);

	//��ȡ�����ϵ����ڵ��ܹ�ʱ
	IN_VALUE std::vector<float> vecTimeToRoot;
	vecTimeToRoot.reserve(nRowCount1);

	IN_VALUE std::vector<STRING> vecMertID;//����ID

	IN_VALUE std::vector<uint16_t> vecMertNumPer;//ÿ����Ʒ������
	vecMertNumPer.reserve(nRowCount1);

	uint16_t nOrigNum = atoi(m_grid1.GetItemText(0, 5));//ԭʼ����

	for (int i = 0; i < nRowCount1; ++i)//iΪ��i��
	{
		//1.��ȡʹ�ù�λ��Ϣ
		std::map<uint16_t, float> mapTemPlaceUse; //ÿһ�еĹ�λʹ����Ϣ
		float fTemTimeCount = 0;	//ÿһ�е����ڵ���ܹ�ʱ
		if (!getWorkPlaceUsedInfo(i, mapWorkPlaceIndex, mapTemPlaceUse, fTemTimeCount))//��ȡ��ǰ�ڵ㹤λʹ����Ϣ�����ù�ʱ
		{
			return;
		}
		vecMapWorkPlaceUse.emplace_back(std::move(mapTemPlaceUse));

		//2.��ȡ����ID
		CString strTempID = m_grid1.GetItemText(i, 2);
		vecMertID.emplace_back(strTempID);

		//3.��ȡÿһ�е����ڵ���ܹ�ʱ
		vecTimeToRoot.emplace_back(fTemTimeCount);
		strTempID = strTempID.Left(strTempID.GetLength() - 2);
		for (int j = i - 1; j >= 0; --j)
		{
			if (strTempID == m_grid1.GetItemText(j, 2))
			{
				vecTimeToRoot[i] += vecTimeToRoot[j];
				break;
			}
		}
		
		//4.��ȡ������Ʒ�����
		vecMertNumPer.emplace_back(atoi(m_grid1.GetItemText(i, 5)) / nOrigNum);
	}


	//��ȡ�Ų�ѡ��
	CButton* radio = (CButton*)GetDlgItem(IDC_RADIO1);
	if (radio->GetCheck() == 1)//��ָ���������������ƻ�
	{
		std::future<stuPlanRet> res1 = m_threadPool->submit(
			GenerateProductionPlan, //���ú���
			m_nTotalProdNum, 
			m_nNumPerTime, 
			mapWorkPlaceIndex, 
			arrWorkPlaceFree, 
			vecMapWorkPlaceUse,
			vecTimeToRoot,
			vecMertID,
			vecMertNumPer);

		stuPlanRet temRet = res1.get();
		if (temRet.nMaxDayLen == 0)
		{
			GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA("���������ƻ�ʧ�ܣ����������ã�");
			return;
		}

		//LOGDEBUG_BYSTREAM(temRet.arrResult);
		m_nMaxDayUsed = temRet.nMaxDayLen;//��������

		CString strTempMsg;
		//��ʾ���н�����б�ؼ�1��
		for (int i = 0; i < nRowCount1; ++i)
			for (int j = 8; j < m_nMaxDayUsed + 8; ++j)
			{
				strTempMsg = temRet.arrResult[i][j - 8] == 0 ? "" : std::to_string(temRet.arrResult[i][j - 8]).c_str();
				m_grid1.SetItemText(i,j,strTempMsg);
			}

		//����λʹ��
		m_fArrWorkPlaceUsed = std::move(temRet.arrWorkUsed);
		strTempMsg.Format("������%d�����ϣ���������%d̨��������ÿ���Ų�%d̨����������������%d��",nRowCount1,m_nTotalProdNum,m_nNumPerTime, m_nMaxDayUsed);
		GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA(strTempMsg);
		GetDlgItem(IDC_BTN_SHOW_WORK_PLACE_USED)->EnableWindow(TRUE);//�鿴��λʹ�������ť
		GetDlgItem(IDC_BTN_PLAN_AND_WORK_TO_DB)->EnableWindow(TRUE);//���湤λʹ����������ݿⰴť
	}
	else//�������������ƻ�
	{
		//�������Ի�����ڣ����ٴ���
		RESULT_WINDOW_CHECK_CLOSE

		//�����߳����������ƻ�
		std::thread(&CTechnolProductionPlanDlg::GenerateProductionPlanAuto,this,
			m_nTotalProdNum,
			mapWorkPlaceIndex,
			arrWorkPlaceFree,
			vecMapWorkPlaceUse,
			vecTimeToRoot,
			vecMertID,
			vecMertNumPer).detach();
	}
}

void CTechnolProductionPlanDlg::GenerateProductionPlanAuto(
	const uint16_t nTotalNum	//��������Ŀ
	, const std::map<CString, uint16_t>& mapWorkPlIndex	//workplace info, <key:workplaceName ,value:workplace index>
	, const DynaArray2D<float>& arrWorkFree	//workplace info, free information
	, const std::vector<std::map<uint16_t, float>>& vecMapWorkPlaceUse	// material information ,map <key:workplace Index,value:workplace cost time>
	, const std::vector<float>& vecTimeToRoot	// material information ,cumulative time to root node);	
	, const std::vector<CString>& vecMertID	// material id
	, const std::vector<uint16_t>& vecMertNumPer)	 // material num per product
{
	//��¼��ʱ
	clock_t start, ends;
	start = clock();
	//��¼�߳�����״̬
	m_bTreadIsRunning = true;

	CString strTempMsg;
	strTempMsg="��ʼ�������С���";
	GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA(strTempMsg);
	//��¼���й����в���
	std::vector<float> fVecTimeLong;//��¼��������
	std::vector<float> fVecTotalDayMer;//��¼�����ۼ�ռ��ʱ��
	std::vector<float> fVecTotalWorkPlace;//��¼�ۼ�ռ�ù�λ����*����

	fVecTimeLong.reserve(m_nTotalProdNum);
	fVecTotalDayMer.reserve(m_nTotalProdNum);
	fVecTotalWorkPlace.reserve(m_nTotalProdNum);

	std::vector<std::future<stuPlanRet>> vecFetResult;
	vecFetResult.reserve(m_nTotalProdNum);

	for (int numPer = 0; numPer <m_nTotalProdNum; ++numPer)/////�Ӹ������ţ�������ӵ��̳߳�
	{
		vecFetResult.emplace_back(m_threadPool->submit(
			GenerateProductionPlan, //���ú���
			m_nTotalProdNum,
			numPer + 1,
			mapWorkPlIndex,
			arrWorkFree,
			vecMapWorkPlaceUse,
			vecTimeToRoot,
			vecMertID,
			vecMertNumPer));
	}
	
	//�ȴ����н��
	std::vector<stuPlanRet> vecResult;//���еĽ��
	vecResult.reserve(m_nTotalProdNum);
	bool isFailed = false;//�Ƿ�����ʧ��
	int nBestNum = 0;
	do{
		for (int k = 0; k <m_nTotalProdNum; ++k)/////�Ӹ������ţ�������ӵ��̳߳�
		{
			strTempMsg = "�ȴ��������";
			GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA(strTempMsg);
			stuPlanRet temRet = vecFetResult[k].get();
			fVecTimeLong.emplace_back(static_cast<float>(temRet.nDayCount));
			fVecTotalDayMer.emplace_back(static_cast<float>(temRet.nMertTotalDay));
			fVecTotalWorkPlace.emplace_back(static_cast<float>(temRet.nWorkTotalDay));

			//�������н��
			vecResult.emplace_back(std::move(temRet));
		
			if(0 == vecResult[k].nMaxDayLen)
			{
				isFailed = true;
				//break;//�Ƿ��б�Ҫ�������н����
			}
			//��ʾ����
			strTempMsg.Format("����ɡ���%d %%", (100 * k / m_nTotalProdNum));
			GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA(strTempMsg);
		}
		
		if(true == isFailed)
		{
			break;
		}

		//����������н��
		nBestNum = m_ResultDlg.CalculatBestSolution(fVecTimeLong, fVecTotalDayMer, fVecTotalWorkPlace);
		if (nBestNum == 0)
		{
			isFailed = true;
			break;
		}
	} while (false);
	
	//�Ƿ�ʧ��
	if(true == isFailed )
	{
		MessageBox("����ʧ�ܣ����������ò�����", "��ʾ", MB_ICONSTOP);//���ᷢ��
		m_bTreadIsRunning = false;//����߳�����״̬
		GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA("");
		return;
	}

	//��ȡ������н��
	stuPlanRet temRet = std::move(vecResult[nBestNum - 1]);

	m_fArrWorkPlaceUsed = std::move(temRet.arrWorkUsed);	//��¼��λʹ��
	DynaArray2D<uint16_t> arrTemResult = std::move(temRet.arrResult);	//���н��
	m_nMaxDayUsed = temRet.nMaxDayLen;	//�ۼ�ʱ��
	//��ʾ���н�����б�ؼ�1��
	for (int i = 0; i < arrTemResult.getRowCount(); ++i)
		for (int j = 8; j < m_nMaxDayUsed + 8; ++j)//����������ڵĶ���0��
		{
			strTempMsg = arrTemResult[i][j - 8] == 0 ? "" : std::to_string(arrTemResult[i][j - 8]).c_str();
			m_grid1.SetItemText(i, j, strTempMsg);
		}

	//����ʱ��
	ends = clock();
	float fInterval = static_cast<float>(ends - start);

	strTempMsg.Format("����������ѽ������ʱ %.3f s����������%d�����ϣ���������%d̨��������ÿ���Ų�%d̨����������������%d��", fInterval / 1000, arrTemResult.getRowCount(), m_nTotalProdNum, nBestNum, m_nMaxDayUsed);
	GetDlgItem(IDC_EDIT_RESUALT_MSG)->SetWindowTextA(strTempMsg);
	GetDlgItem(IDC_BTN_SHOW_RESULT)->EnableWindow(TRUE);//��ʾ�鿴�����ť
	GetDlgItem(IDC_BTN_SHOW_WORK_PLACE_USED)->EnableWindow(TRUE);//�鿴��λʹ�������ť
	GetDlgItem(IDC_BTN_PLAN_AND_WORK_TO_DB)->EnableWindow(TRUE);//���湤λʹ����������ݿⰴť
	m_bTreadIsRunning = false;//����߳�����״̬
}


bool CTechnolProductionPlanDlg::getWorkPlaceUsedInfo(const uint16_t index, const std::map<CString, uint16_t>& mapWorkPlaceInfo, std::map<uint16_t, float>& mapWorkPlaceUse, float& fTimeCount)
{
	//�洢��ʱ����͹�ʱ����������
	std::vector<CString> strTemProcess;	//��������
	std::vector<float> fTemWorkTime;	//����ʱ��
	strTemProcess.reserve(10);
	fTemWorkTime.reserve(10);//һ�㹤�����̲��ᳬ��10��

	CString	strProcess = m_grid1.GetItemText(index, 6);//��ȡ����
	CString strWorkTime = m_grid1.GetItemText(index, 7);//��ȡ��ʱ

	strProcess = strProcess + "->";
	while (strProcess.GetLength() > 2)//��ֹ�������
	{
		int posi = strProcess.Find("->");
		strTemProcess.emplace_back(strProcess.Left(posi));
		strProcess = strProcess.Right(strProcess.GetLength() - posi - 2);
	}
	strWorkTime = strWorkTime + "+";
	while (strWorkTime.GetLength() > 1)//��ֹ�ʱ
	{
		int pos1 = strWorkTime.Find("+");
		fTemWorkTime.emplace_back(atof(strWorkTime.Left(pos1)));
		strWorkTime = strWorkTime.Right(strWorkTime.GetLength() - pos1 - 1);
	}
	//��鹤��͹�ʱ�����Ƿ�һ��
	if (strTemProcess.size() != fTemWorkTime.size())
	{
		MessageBox("�������̱����󣬹���������Ϣ�빤ʱ��Ϣ��������", "��ʾ", MB_ICONSTOP);
		return false;
	}
	//���������͹�ʱ��ֵ
	for (int j = 0; j < strTemProcess.size(); ++j)
	{
		auto iter = mapWorkPlaceInfo.find(strTemProcess[j]);
		if (iter == mapWorkPlaceInfo.end())//��鹤�������Ƿ����
		{
			CString temStr;
			temStr.Format("�� %d �У��������� %s �����ڣ�", index, strTemProcess[j]);
			MessageBox(temStr, "��ʾ", MB_ICONSTOP);
			return false;
		}
		uint16_t nIndex = iter->second;
		auto iterMapUse = mapWorkPlaceUse.find(nIndex);
		if (iterMapUse == mapWorkPlaceUse.end())//�״���ӹ�λ
		{
			mapWorkPlaceUse.insert(std::make_pair(nIndex, fTemWorkTime[j]));
		}
		else
		{
			iterMapUse->second += fTemWorkTime[j];
		}

		fTimeCount += fTemWorkTime[j];
	}
	return true;
}

//�鿴��λʹ�����
void CTechnolProductionPlanDlg::OnBnClickedBtnShowWorkPlaceUsedToList2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (true == m_bTreadIsRunning)	//����߳��Ƿ���������
	{
		MessageBox("�������������ƻ���", "��ʾ", MB_ICONSTOP);
		return;
	}
	//��ȡ��λ������Ϣ
	CHeaderCtrl* pHeaderCtrl2 = m_grid2.GetHeaderCtrl();
	int nColCount2 = pHeaderCtrl2-> GetItemCount() - 2;//���2������,1-2��Ϊ������Ϣ
	int nRowCount2 = m_grid2.GetItemCount();//���2������
	if (m_fArrWorkPlaceUsed.getRowCount() != nRowCount2 || m_fArrWorkPlaceUsed.getColCount() != nColCount2)

	{
		MessageBox("��λ��Ϣ����", "��ʾ", MB_ICONSTOP);
		return;
	}

	CString temWorkUsed;
	for (int i = 0; i < nRowCount2; ++i)
	{
		for (int j = 0; j < nColCount2; ++j)
		{
			m_fArrWorkPlaceUsed[i][j] == 0 ? temWorkUsed = "" : temWorkUsed.Format("%.2f", static_cast<float>(m_fArrWorkPlaceUsed[i][j]));
			m_grid2.SetItemText(i, j + 2, temWorkUsed);
		}
	}
}

//����λ��Ϣ���浽���ݿ�
void CTechnolProductionPlanDlg::OnBnClickedBtnSavePlanAndWorkplaceUsedInfoToDb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (true == m_bTreadIsRunning)	//����߳��Ƿ���������
	{
		MessageBox("�������������ƻ���", "��ʾ", MB_ICONSTOP);
		return;
	}
	ASSERT(m_nMaxDayUsed > 0);//�����������ƻ�����ֵ

	//ȷ��������ƣ��������򴴽�һ�����������滻��
	CString strTableName;//�����ı������
	CString strDate;
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(strTableName);
	GetDlgItem(IDC_DATETIMEPICKER1)->GetWindowTextA(strDate);
	CTechnolProceDBNameSaveDlg dlg("WorkPlaceUsedInfo");//�������ݿ�������
	dlg.m_strTableName = strDate + strTableName;
	if (dlg.DoModal() != IDOK)
	{
		return;
	}
	strTableName = dlg.m_strTableName;

	uint16_t mRowCount1 = m_grid1.GetItemCount();//��������������
	uint16_t nRowCount2 = m_grid2.GetItemCount();//��λ������

	//ȷ�������ƻ��͹�λʹ����Ϣ�ı�������
	uint16_t nDayMin = 0;//��¼��С�������������λ�������ƻ�һ��
	uint16_t	nDayMax = m_nMaxDayUsed;
	std::vector<CString> strVecWorkPlaceName;	//��¼�ĸ���λ��ʹ���ˣ���λ����
	std::vector<uint16_t> nVecWorkPlaceUsedIndex;	//��¼�ĸ���λ��ʹ���ˣ���λ����
	GetPlanWorkPlaceUsedInfo(strVecWorkPlaceName, nVecWorkPlaceUsedIndex, nDayMin, nDayMax);

	uint32_t nDaysCount = nDayMax - nDayMin;//ռ�õ���������

	//���湤λʹ����Ϣ
	{
		//�������SQL���
		CString strSQL = "CREATE TABLE " + strTableName + "(Num TEXT,	DataInfo TEXT";
		for (auto iter : strVecWorkPlaceName)
		{
			strSQL += ",";
			strSQL += " " + iter + " TEXT";
		}
		strSQL += ")";
		//��ȡ�С��������������ڴ�
		int nColumuCount = strVecWorkPlaceName.size() + 2;//������

		DynaArray2D<STRING> temValues(nDaysCount, nColumuCount);
		//д��������
		for (int i = 0; i < nDaysCount; ++i)//����������Ϊ��
		{
			temValues[i][0] = std::to_string(i + 1).c_str();//���
			temValues[i][1] = m_VecDataName[nDayMin + i];//����
			for (int j = 2; j < nColumuCount; ++j)
			{
				if (m_fArrWorkPlaceUsed[nVecWorkPlaceUsedIndex[j - 2]][nDayMin + i] > 0)
				{
					temValues[i][j].Format("%.2f", m_fArrWorkPlaceUsed[nVecWorkPlaceUsedIndex[j - 2]][nDayMin + i]);
				}
			}
		}
		//��ӱ������
		//uint32_t cmdID = m_dbService->createTableCmd("WorkPlaceUsedInfo", strTableName, strSQL, std::move(temValues));
		//g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable));
		if (!m_dbService->createTableSyn("WorkPlaceUsedInfo", strTableName, strSQL, std::move(temValues)))//ʹ��ͬ������
		{
			MessageBox("���湤λʹ����Ϣʧ�ܣ�", "��ʾ", MB_ICONERROR);
			return;
		}
	}

	//�������е������ƻ�
	{
		//�������SQL���
		CString strSQL = "CREATE TABLE " + strTableName + "(��� TEXT, �㼶 TEXT, ��ʶ TEXT, ���ϱ�� TEXT,\
																�������� TEXT, ���� TEXT, �������� TEXT, ��ʱ TEXT";
		for (int i = 0; i < nDaysCount; ++i)
		{
			strSQL += ",";
			strSQL += " " + m_VecDataName[nDayMin + i] + " TEXT";
		}
		strSQL += ")";
		strSQL.Replace('-', '_');//�����ַ������򴴽����Ƿ�

		//�����ڴ�,������д��ֵ
		DynaArray2D<STRING> temValues(mRowCount1, nDaysCount + 8); 
		for (int i = 0; i < mRowCount1; ++i)
		{
			for (int j = 0; j < 8; ++j)//д�������Ϣ
			{
				temValues[i][j] = m_grid1.GetItemText(i, j);
			}
			for (int j = nDayMin + 8 ; j < nDayMax + 8; ++j)//д�������ƻ���Ϣ
			{
				temValues[i][j - nDayMin] = m_grid1.GetItemText(i, j);
			}
			//д�������ƻ���Ϣ
		}

		//uint32_t cmdID = m_dbService->createTableCmd("ProductionPlanedInfo", strTableName, strSQL, std::move(temValues));
		//g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable));
		if (!m_dbService->createTableSyn("ProductionPlanedInfo", strTableName, strSQL, std::move(temValues)))//ʹ��ͬ������
		{
			m_dbService->deleteTableSyn("WorkPlaceUsedInfo", strTableName);//ɾ����λ��Ϣ�еı��
			MessageBox("���湤λʹ�ü������ƻ���Ϣʧ�ܣ�", "��ʾ", MB_ICONERROR);
			return;
		}
	}

	OnBnClickedBtnShowWorkPlaceUsedToList2();//��ʾ���ù�λ

	//��ʾ�����״̬��
	{
		CString strStatus;
		strStatus.Format("��ӱ�� �ɹ� ��������� %s ��", strTableName);
		m_StatusBar.SetPaneText(1, strStatus);
		//��ʾ����
		g_statuData.strInfo = strStatus;
		g_statuData.nTimeCount = 15;//��ʾ15s
	}
}

//��ȡ��λʹ����Ϣ
bool CTechnolProductionPlanDlg::GetPlanWorkPlaceUsedInfo(std::vector<CString> & strVecWorkPlaName, std::vector<uint16_t> & nVecWorkPlaRowUsed, uint16_t & nDaysMin, const uint16_t nDaysMax)
{
	uint16_t nRowCount2 = m_grid2.GetItemCount();//���2������
	strVecWorkPlaName.reserve(nRowCount2);
	nVecWorkPlaRowUsed.reserve(nRowCount2);

	//��¼��ռ�õĹ�λ
	for (int i = 0; i < nRowCount2; ++i)
	{
		for (int j = 0; j < nDaysMax; ++j)
		{
			if(m_fArrWorkPlaceUsed[i][j] != 0)//��λ��ռ����
			{
				nVecWorkPlaRowUsed.emplace_back(i);//��ռ�ù�λ����
				strVecWorkPlaName.emplace_back(m_grid2.GetItemText(i,1));//��λ����
				break;
			}
		}
	}

	//�ҵ����翪ʼʱ��
	int nWorkPlaceUsedCount = nVecWorkPlaRowUsed.size();
	bool bIsBreak = false;
	for (int j = 0; j < nDaysMax; ++j)//����������ʼ����
	{
		for (int i = 0; i < nWorkPlaceUsedCount; ++i)
		{
			if(m_fArrWorkPlaceUsed[nVecWorkPlaRowUsed[i]][j] != 0)
			{
				nDaysMin = j;
				bIsBreak = true;
				break;
			}
		}
		if (bIsBreak)
			break;
	}

	return true;
}



void CTechnolProductionPlanDlg::OnBnClickedBtnShowResult()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (true == m_bTreadIsRunning)	//����߳��Ƿ���������
	{
		MessageBox("�������������ƻ���", "��ʾ", MB_ICONSTOP);
		return;
	}
	//////��ģʽ�Ի���
	if(!IsWindow(m_ResultDlg.m_hWnd))//����Ի������ڣ����ٴ���
	{
		m_ResultDlg.Create(IDD_TEC_PROD_PLAN_SHOW,NULL);
	}
	m_ResultDlg.ShowWindow(SW_SHOW);
}

