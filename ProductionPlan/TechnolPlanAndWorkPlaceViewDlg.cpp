// TechnolPlanAndWorkPlaceViewDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolPlanAndWorkPlaceViewDlg.h"
#include "afxdialogex.h"

static std::map<uint32_t, STRING> g_mapDealResult;//<key:cmdID,value:strCmd>����ִ�н��������һ������
static const STRING updateProcTable[2] = { "UpCom1", "UpLis2"};//������Ͽ�ؼ�1���б�ؼ�2.
static std::atomic_bool g_bWaitResult = false;	//�ȴ����
//����״̬��
static StatusData g_statuData;

#define CALL_BACK_WAIT_CHECK \
if (true == g_bWaitResult)\
{\
	MessageBox("�ȴ���Ϣ�ص������Ժ������");\
}\
g_bWaitResult = true;

// CTechnolPlanAndWorkPlaceViewDlg �Ի���

IMPLEMENT_DYNAMIC(CTechnolPlanAndWorkPlaceViewDlg, CDialog)

CTechnolPlanAndWorkPlaceViewDlg::CTechnolPlanAndWorkPlaceViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTechnolPlanAndWorkPlaceViewDlg::IDD, pParent)
	, m_dbService(getProPlanDBService())
{
	m_dbService->addListener(this);
}

CTechnolPlanAndWorkPlaceViewDlg::~CTechnolPlanAndWorkPlaceViewDlg()
{
	m_dbService->removeListener(this);
}

void CTechnolPlanAndWorkPlaceViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo1);
	DDX_Control(pDX, IDC_LIST1, m_grid1);
	DDX_Control(pDX, IDC_LIST2, m_grid2);
	DDX_Control(pDX, IDC_LIST3, m_grid3);
}


BEGIN_MESSAGE_MAP(CTechnolPlanAndWorkPlaceViewDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SHOW_PLAN_TO_LIST, &CTechnolPlanAndWorkPlaceViewDlg::OnClickedBtnShowPlanAndWorkplaceUsedToList)
	ON_BN_CLICKED(IDC_BTN_DELETE_PLAN, &CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnDeleteCurrentPlan)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST3, &CTechnolPlanAndWorkPlaceViewDlg::OnDblclkList3)
	ON_BN_CLICKED(IDC_BTN_ADD_WORKPLACE, &CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnAddWorkplace)
	ON_BN_CLICKED(IDC_BTN_DELETE_WORKPLAC, &CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnDeleteWorkplac)
END_MESSAGE_MAP()


// CTechnolPlanAndWorkPlaceViewDlg ��Ϣ�������

void CTechnolPlanAndWorkPlaceViewDlg::dealDBResult(std::queue<DBResult>& rst)
{
	LOGDEBUG("[%s][%d] begin, result size %d !", __STRING_FUNCTION__, __LINE__, rst.size());

	while (rst.size() > 0)
	{
		auto iterRst = rst.front();
		if (g_mapDealResult.find(iterRst.cmdID) != g_mapDealResult.end())
		{
			//����״̬��
			CString strStatus;
			if (updateProcTable[0] == g_mapDealResult[iterRst.cmdID]) 
			{
				ShowPlanAndPlaceUsedTableToCombo1();//������Ͽ�ؼ�
				
				strStatus.Format("ɾ����� %s �����ݿ����� %s ������� %s ��"
					, iterRst.bResult ? "�ɹ�" : "ʧ��"
					, iterRst.strDBName
					, iterRst.strTableName);

			}
			else if (updateProcTable[1] == g_mapDealResult[iterRst.cmdID])
			{
				//ShowWorkPlaceInfoToList3();

				strStatus.Format("���±�� %s �����ݿ����� %s ������� %s ��"
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

			g_bWaitResult = false;
		}
		rst.pop();
	}
	LOGDEBUG("[%s][%d] CTechnolPlanAndWorkPlaceViewDlg::dealDBResult end!", __STRING_FUNCTION__, __LINE__);
}


BOOL CTechnolPlanAndWorkPlaceViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	////���öԻ���ͼ��
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CREO);//IDR_ICONΪͼ����Դ��
	SetIcon(m_hIcon, FALSE);
	//����״̬��
	UINT array[8];
	for(int i=0;i<8;i++)
	{
		array[i] = 1001 + i;
	}
	m_StatusBar.Create(this); //����״̬������
	m_StatusBar.SetIndicators(array,sizeof(array)/sizeof(UINT)); //������
	m_StatusBar.SetPaneInfo(0,array[0],0,70); //���������
	m_StatusBar.SetPaneInfo(1,array[1],0,600); //���������
	m_StatusBar.SetPaneInfo(2,array[2],0,150); //
	m_StatusBar.SetPaneInfo(3,array[3],0,150); 
	m_StatusBar.SetPaneInfo(4,array[4],0,110); //
	m_StatusBar.SetPaneInfo(5,array[5],0,110); 

	CTime Time = CTime::GetCurrentTime();
	m_StatusBar.SetPaneText(0,"״̬��");//��������ı�
	m_StatusBar.SetPaneText(1,"");//��������ı�
	m_StatusBar.SetPaneText(2,"");//��������ı�
	m_StatusBar.SetPaneText(3,"");//��������ı�
	m_StatusBar.SetPaneText(4,Time.Format("%Y-%m-%d"));
	m_StatusBar.SetPaneText(5,Time.Format("%H:%M:%S"));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);
	SetTimer(1,1000,NULL); //���ö�ʱ��	
	//����ͼ��
	m_ImageList.Create(14, 14, ILC_COLOR24|ILC_MASK, 1, 1);
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_ASM)));
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PRT)));
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
	//�����б�ؼ�2����
	m_grid3.InsertColumn(0,_T("���"),LVCFMT_LEFT,50,0);
	m_grid3.InsertColumn(1,_T("��λ"),LVCFMT_LEFT,80,0);
	m_grid3.InsertColumn(2,_T("����"),LVCFMT_LEFT,80,0);
	m_grid3.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	ShowPlanAndPlaceUsedTableToCombo1();//�������ƻ�����ʾ����Ͽ�ؼ�
	ShowWorkPlaceInfoToList3();//����λ��Ϣ��ӵ���3
	return TRUE;  // return TRUE unless you set the focus to a control

	// �쳣: OCX ����ҳӦ���� FALSE
}


void CTechnolPlanAndWorkPlaceViewDlg::ShowPlanAndPlaceUsedTableToCombo1(void)
{
	//�������ʾ����Ͽ�ؼ�
	m_combo1.ResetContent();
	std::vector<STRING> temTableName = (*m_dbService)["WorkPlaceUsedInfo"].m_TableNames;

	auto iterCol = temTableName.begin();
	size_t index = 0;
	while (iterCol != temTableName.end())
	{
		m_combo1.InsertString(index++, *(iterCol++));//����ؼ�
	}
	m_combo1.SetCurSel(0);
}


void CTechnolPlanAndWorkPlaceViewDlg::ShowWorkPlaceInfoToList3(void)
{
	m_grid3.DeleteAllItems();

	TableInfo temTable = (*m_dbService)["WorkPlaceInfo"]["WorkPlaceDef"];
	int32_t nRowCount = temTable.m_Values.getRowCount();

	for (int i = 0; i < nRowCount ;++i)
	{
		m_grid3.InsertItem(i, std::to_string(i + 1).c_str());
		m_grid3.SetItemText(i,1, temTable.m_Values[i][1]);
		m_grid3.SetItemText(i,2, temTable.m_Values[i][2]);
	}
}

//�������ƻ��͹�λʹ����ʾ���б�ؼ�
void CTechnolPlanAndWorkPlaceViewDlg::OnClickedBtnShowPlanAndWorkplaceUsedToList()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strTableName;
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(strTableName);//���ɼƻ�������
	if ("" == strTableName)
	{
		return;
	}
	//����б�ؼ��е�����
	m_grid1.DeleteAllItems();
	m_grid2.DeleteAllItems();
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

	//��ȡ��λʹ����Ϣ���
	{
		TableInfo temTable = (*m_dbService)["WorkPlaceUsedInfo"][strTableName];
		size_t nValueRowCount = temTable.m_Values.getRowCount();
		size_t nValueColCount = temTable.m_Values.getColCount();
		//�����б�ؼ�ǰ2��
		for (int i = 0; i < nValueColCount - 2; ++i)
		{
			m_grid2.InsertItem(i, std::to_string(i).c_str());//�����һ��
			m_grid2.SetItemText(i, 1, temTable.m_vevCols[i + 2]);
		}
		//����������
		for (int i = 0; i < nValueRowCount; ++i)
		{
			m_grid1.InsertColumn(i + 8, temTable.m_Values[i][1], LVCFMT_LEFT, 70, 0);//����������
			m_grid2.InsertColumn(i + 2, temTable.m_Values[i][1], LVCFMT_LEFT, 70, 0);//����������
			for (int j = 2; j < nValueColCount; ++j)
			{
				m_grid2.SetItemText(j - 2, i + 2, temTable.m_Values[i][j]);//��ʾ��λʹ�����
			}
		}
	}

	//��ȡ�����ƻ�
	{
		TableInfo temTable = (*m_dbService)["ProductionPlanedInfo"][strTableName];
		size_t nValueRowCount = temTable.m_Values.getRowCount();
		size_t nValueColCount = temTable.m_Values.getColCount();
		//�����б�ؼ�1
		for (int i = 0; i < nValueRowCount; ++i)
		{
			m_grid1.InsertItem(i, temTable.m_Values[i][0]);
			for (int j = 1; j < nValueColCount; ++j)
			{
				m_grid1.SetItemText(i, j, temTable.m_Values[i][j]);
			}
		}
	}
}


void CTechnolPlanAndWorkPlaceViewDlg::OnTimer(UINT_PTR nIDEvent)
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

void CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnDeleteCurrentPlan()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CALL_BACK_WAIT_CHECK

	CString strTableName;
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(strTableName);
	if ("" == strTableName)
	{
		return;
	}

	//ɾ����λʹ����Ϣ�б��
	uint32_t cmdID = m_dbService->deleteTableCmd("WorkPlaceUsedInfo", strTableName);
	g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable[0]));

	//ɾ�������ƻ��б��
	cmdID = m_dbService->deleteTableCmd("ProductionPlanedInfo", strTableName);
	g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable[0]));

	m_grid1.DeleteAllItems();
	m_grid2.DeleteAllItems();
}


void CTechnolPlanAndWorkPlaceViewDlg::OnDblclkList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int i=m_grid3.GetSelectionMark();
	//���û��ѡ�У�����
	if(i==-1)
	{
		return;
	}
	GetDlgItem(IDC_EDIT1)->SetWindowTextA(m_grid3.GetItemText(i,1));
	GetDlgItem(IDC_EDIT2)->SetWindowTextA(m_grid3.GetItemText(i,2));

	*pResult = 0;
}


void CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnAddWorkplace()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CALL_BACK_WAIT_CHECK//���ǰһ����Ϣ�Ƿ�����
	
	CString strName,strNum;
	int nNum(0);
	GetDlgItem(IDC_EDIT1)->GetWindowTextA(strName);
	GetDlgItem(IDC_EDIT2)->GetWindowTextA(strNum);
	nNum = atoi(strNum);
	if (strName == "" || nNum <= 0)
	{
		MessageBox("��������","��ʾ",MB_ICONERROR);
		return;
	}
	int nRowCount = m_grid3.GetItemCount();
	//��������Ƿ����
	strNum = std::to_string(nNum).c_str();
	for (int i = 0; i < nRowCount; ++i)
	{
		if (m_grid3.GetItemText(i,1) == strName)//�޸�
		{
			m_grid3.SetItemText(i,2,strNum);
			return SaveWorkPlaceInfoToDBList3();
		}
	}
	
	m_grid3.InsertItem(nRowCount,std::to_string(nRowCount + 1).c_str());
	m_grid3.SetItemText(nRowCount,1,strName);
	m_grid3.SetItemText(nRowCount,2,strNum);
	SaveWorkPlaceInfoToDBList3();
}


void CTechnolPlanAndWorkPlaceViewDlg::OnBnClickedBtnDeleteWorkplac()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CALL_BACK_WAIT_CHECK

	int nRowIndex=m_grid3.GetSelectionMark();
	//���û��ѡ�У�����
	if(nRowIndex == -1)
	{
		return;
	}
	m_grid3.DeleteItem(nRowIndex);
	//��������
	int nRowCount = m_grid3.GetItemCount();
	for (size_t index = 0 ; index < nRowCount; )
	{
		m_grid3.SetItemText(index, 0, std::to_string(++index).c_str());
	}
	SaveWorkPlaceInfoToDBList3();

}


void CTechnolPlanAndWorkPlaceViewDlg::SaveWorkPlaceInfoToDBList3(void)
{
	int nRowCount = m_grid3.GetItemCount();
	int nColumuCount = 3;//������
	DynaArray2D<STRING> temValues(nRowCount, nColumuCount);
	for (int i = 0; i < nRowCount; ++i)
	{
		for (int j = 0; j < nColumuCount; ++j)
		{
			temValues[i][j] = m_grid3.GetItemText(i, j);
			//temValues = m_grid1.GetItemText(i, j + 1);
		}
	}

	uint32_t cmdID = m_dbService->updateTableCmd("WorkPlaceInfo", "WorkPlaceDef", std::move(temValues));
	g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable[1]));
}
