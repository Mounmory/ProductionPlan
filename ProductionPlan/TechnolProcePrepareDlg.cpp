// TechnolProcePrepareDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolProcePrepareDlg.h"
#include "TechnolProceDBNameSaveDlg.h"

#include "afxdialogex.h"

static std::map<uint32_t, STRING> g_mapDealResult;//<key:cmdID,value:strCmd>����ִ�н��������һ������
static const STRING updateProcTable = "UpCom2";//�����б�ؼ�2
//����״̬��
static StatusData g_statuData;

// CTechnolProcePrepareDlg �Ի���

IMPLEMENT_DYNAMIC(CTechnolProcePrepareDlg, CDialog)

CTechnolProcePrepareDlg::CTechnolProcePrepareDlg(std::shared_ptr<DBService> dbService, CWnd* pParent /*=NULL*/)
	: CDialog(CTechnolProcePrepareDlg::IDD, pParent)
	, m_dbService(getProPlanDBService())
	, m_fWorkTime(0)
{
	m_dbService->addListener(this);
}

CTechnolProcePrepareDlg::~CTechnolProcePrepareDlg()
{
	m_dbService->removeListener(this);
}

void CTechnolProcePrepareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
#ifdef PRO_USE_VAR_ARGS
	DDX_Control(pDX, IDC_PVIEWCTRL1, m_CDprivew);//Creoģ��Ԥ���ؼ�
#endif
	DDX_Control(pDX, IDC_TREE1, m_Tree);
	DDX_Control(pDX, IDC_LIST1, m_grid1);
	DDX_Control(pDX, IDC_LIST2, m_grid2);
	DDX_Control(pDX, IDC_COMBO1, m_combo1);
	DDX_Control(pDX, IDC_COMBO2, m_combo2);
	DDX_Text(pDX, IDC_EDIT6, m_fWorkTime);
}


BEGIN_MESSAGE_MAP(CTechnolProcePrepareDlg, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_SAVE_TO_BD, &CTechnolProcePrepareDlg::OnClickedBtnSaveList1ToBd)
	ON_BN_CLICKED(IDC_BTN_SHOW_DB, &CTechnolProcePrepareDlg::OnBnClickedBtnViewTableListInDB)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CTechnolProcePrepareDlg::OnDblclkList1)
	ON_BN_CLICKED(IDC_BTN_ADD_PROGR, &CTechnolProcePrepareDlg::OnBnClickedBtnAddSingleProgrToList2)
	ON_BN_CLICKED(IDC_BTN_ADD_TO_LIST1, &CTechnolProcePrepareDlg::OnBnClickedBtnAddEditProcToList1)
	ON_BN_CLICKED(IDC_BTN_ADD_PRO_TO_TABLE, &CTechnolProcePrepareDlg::OnBnClickedBtnShowProcessInfoFromDBToTable)
	ON_BN_CLICKED(IDC_BTN_DEL_PROGR, &CTechnolProcePrepareDlg::OnBnClickedBtnDelProgInList2)
	ON_BN_CLICKED(IDC_BTN_DELE_PROG_IN_LISET2, &CTechnolProcePrepareDlg::OnBnClickedBtnDeleSelectedProgInLiset2)
	ON_BN_CLICKED(IDC_BTN_UPER, &CTechnolProcePrepareDlg::OnBnClickedBtnMoveUperInList2)
	ON_BN_CLICKED(IDC_BTN_DOWN, &CTechnolProcePrepareDlg::OnBnClickedBtnMoveDownInList2)
END_MESSAGE_MAP()

// CTechnolProcePrepareDlg ��Ϣ�������
void CTechnolProcePrepareDlg::dealDBResult(std::queue<DBResult>& rst)
{
	LOGDEBUG("[%s][%d] begin, result size %d !", __STRING_FUNCTION__, __LINE__, rst.size());

	while (rst.size() > 0)
	{
		auto iterRst = rst.front();
		if (g_mapDealResult.find(iterRst.cmdID) != g_mapDealResult.end())
		{
			if (updateProcTable == g_mapDealResult[iterRst.cmdID]) //������Ͽ�ؼ�2
			{
				ShowProcessTableListToCombo2();
				//����״̬��
				CString strStatus;
				strStatus.Format("��ӱ�� %s �����ݿ����� %s ������� %s ��"
					, iterRst.bResult ? "�ɹ�" : "ʧ��"
					, iterRst.strDBName
					, iterRst.strTableName);
				m_StatusBar.SetPaneText(1, strStatus);
				//��ʾ����
				g_statuData.strInfo = strStatus;
				g_statuData.nTimeCount = 15;//��ʾ15s
			}
			else
			{
			}
			g_mapDealResult.erase(iterRst.cmdID);
		}
		rst.pop();
	}
	
	
}

BOOL CTechnolProcePrepareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	////���öԻ���ͼ��
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CREO);//IDR_ICONΪͼ����Դ��
	SetIcon(m_hIcon, FALSE);
	///////////�������ؼ��ڵ�ͼ��
	m_ImageList.Create(14, 14, ILC_COLOR24|ILC_MASK, 1, 1);
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_ASM)));
	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_ICON_PRT)));
//	m_ImageList.Add(LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_PRT)));
	m_Tree.SetImageList(&m_ImageList, TVSIL_NORMAL);

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

#ifdef PRO_USE_VAR_ARGS
	//��װ�����������Ϊ���ؼ����ڵ�
	ProName     name;
	ProSolid asm_solid;
	asm_solid=(ProSolid)GlobalFunction::GetCurrentMdl();
	ProMdlNameGet(asm_solid,name);
	m_Tree.InsertItem(CString(name).MakeLower()+".asm",0,0,TVI_ROOT);
	p_asmfeatlist.RemoveAll();	
	GlobalFunction::ShowModelTreeWhithoutFeature(asm_solid,m_Tree,m_Tree.GetRootItem(),p_asmfeatlist);
    m_Tree.Expand(m_Tree.GetRootItem(),TVE_EXPAND);
	////����ǰģ����ʾ��Preview�ؼ�
	ProMdl		mdl;
	ProMdldata	data;
	ProError	status;
	status = ProMdlCurrentGet(&mdl);
	/////��ȡ·��
	if (ProMdlDataGet(mdl, &data) == PRO_TK_NO_ERROR)
	{
		m_strModelPath = CString(data.path);
		if (CString(data.device) != "")
		{
			m_strModelPath = CString(data.device) + ":" + m_strModelPath;
		}
	}
	//��ȡģ������
	m_strRootMdlName = name;
#endif

	//�����б�ؼ�
	m_grid1.InsertColumn(0,_T("���"),LVCFMT_LEFT,40,0);
	m_grid1.InsertColumn(1,_T("�㼶"),LVCFMT_LEFT,60,0);
	m_grid1.InsertColumn(2,_T("���ϱ��"),LVCFMT_LEFT,100,0);
	m_grid1.InsertColumn(3,_T("��������"),LVCFMT_LEFT,100,0);
	m_grid1.InsertColumn(4,_T("ģ������"),LVCFMT_LEFT,100,0);
	m_grid1.InsertColumn(5,_T("����"),LVCFMT_LEFT,40,0);
	m_grid1.InsertColumn(6,_T("����"),LVCFMT_LEFT,40,0);
	m_grid1.InsertColumn(7,_T("��װ����"),LVCFMT_LEFT,150,0);
	m_grid1.InsertColumn(8,_T("��������"),LVCFMT_LEFT,150,0);
	m_grid1.InsertColumn(9,_T("��ʱ(h)"),LVCFMT_LEFT,150,0);
	m_grid1.InsertColumn(10,_T("�ϼ�(h)"),LVCFMT_LEFT,50,0);
	m_grid1.InsertColumn(11,_T("��������"),LVCFMT_LEFT,75,0);
	m_grid1.InsertColumn(12,_T("��ע"),LVCFMT_LEFT,150,0);
	m_grid1.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	//�����б�ؼ�2����
	m_grid2.InsertColumn(0,_T("���"),LVCFMT_LEFT,40,0);
	m_grid2.InsertColumn(1,_T("��λ"),LVCFMT_LEFT,85,0);
	m_grid2.InsertColumn(2,_T("��ʱ(h)"),LVCFMT_LEFT,50,0);
	m_grid2.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);
	//��ʾ������ϸ
	ShowMdlTreeToBillOfMaterial();
	//��Ͽ�ؼ���ʾ��λ
	ShowWorkplaceToCombo1();
	ShowProcessTableListToCombo2();

	return TRUE; 
}

//��ģ�����ṹ��ʾ�����ؼ�
void CTechnolProcePrepareDlg::ShowMdlTreeToBillOfMaterial(void)
{
#ifdef PRO_USE_VAR_ARGS
	m_grid1.SetImageList(&m_ImageList,LVSIL_SMALL);
	m_grid1.DeleteAllItems();
	m_vecStrParents.clear();
	////�����һ�У���Ŀ¼��
	HTREEITEM root=m_Tree.GetRootItem();
	CString Asmname,mdlname,mdltype;
	Asmname=m_Tree.GetItemText(root);
	int pos=Asmname.Find(".");
	mdlname=Asmname.Left(pos);
	mdltype=Asmname.Right(Asmname.GetLength()-pos-1);
	m_grid1.InsertItem(0,"1",0);/////����ͼ��asm
	m_grid1.SetItemText(0,1,"0");
	m_grid1.SetItemText(0,4,mdlname);
	m_grid1.SetItemText(0,5,mdltype);
	m_grid1.SetItemText(0,6,"1");
//	m_grid1.SetItemText(0,7,"null");
	m_vecStrParents.push_back(Asmname);
	////������Ŀ¼
	ShowChildTreeList(root);
#endif
}

void CTechnolProcePrepareDlg::ShowChildTreeList(HTREEITEM Hitem)
{
#ifdef PRO_USE_VAR_ARGS
	HTREEITEM hchild=m_Tree.GetChildItem(Hitem);
	while(hchild!=NULL)
	{
		HTREEITEM fuxiang=m_Tree.GetParentItem(Hitem);
		CString strParent = m_Tree.GetItemText(hchild);//��¼����洢�����1��
		CString strFullName = strParent;//���ڻ�ȡģ�͵����ڵ��ȫ·��
		int pos1=strFullName.Find(".");
		CString mdlname=strFullName.Left(pos1);//ģ������
		CString mdltype=strFullName.Mid(pos1+1,3);//ģ������;

		/////�жϲ㼶
		HTREEITEM hRoot=m_Tree.GetRootItem();
		CString strLevel = "   ";
		int nLevel = 1;
		HTREEITEM hGetParent = Hitem;//��ȡ����
		strFullName = m_Tree.GetItemText(hGetParent) + strFullName;//�ۼӸ���������
		while (hGetParent != hRoot)
		{
			//��ȡ�ۼƸ���
			strFullName = m_Tree.GetItemText(hGetParent) + strFullName;
			//��ȡ�㼶
			hGetParent = m_Tree.GetParentItem(hGetParent);
			strLevel += "    ";
			nLevel++;
		}
		strLevel.Format("%s%d",strLevel,nLevel);
		//////�ж�����Ƿ��Ѿ����б���
		int nVecSize = m_vecStrParents.size();
		bool bIsExist = false;//��ģ��ʱ�����ڱ������
		int nRowNum= 0 ;
		for (int i = 0 ; i < nVecSize ; ++i)
		{
			if (strFullName == m_vecStrParents.at(i))
			{
				nRowNum = i;//��ȡ�к�
				bIsExist = true;
				break;
			}
		}
		//�������д�����ͬ����ˣ������������+1
		if (true == bIsExist)//
		{
			int num=atoi(m_grid1.GetItemText(nRowNum,6));
			num++;
			CString shuliang;
			shuliang.Format("%d",num);
			m_grid1.SetItemText(nRowNum,6,shuliang);
		}
		else
		{
			m_vecStrParents.push_back(strFullName);//���ִ浽vector

			nRowNum = m_grid1.GetItemCount();
			CString xuhao;
			xuhao.Format("%d",nRowNum+1);///�����
			if(mdltype=="asm")/////����ͼ��
			{
				m_grid1.InsertItem(nRowNum,xuhao,0);	//ͼƬ������(��һ��ͼƬ)
			}
			else
			{
				m_grid1.InsertItem(nRowNum,xuhao,1);
			}
			m_grid1.SetItemText(nRowNum,1,strLevel);///�㼶
			m_grid1.SetItemText(nRowNum,4,mdlname);
			m_grid1.SetItemText(nRowNum,5,mdltype);
			m_grid1.SetItemText(nRowNum,6,"1");//����
			m_grid1.SetItemText(nRowNum,7,strParent);//����
		}
		ShowChildTreeList(hchild);//�ݹ飬����
		hchild=m_Tree.GetNextItem(hchild,TVGN_NEXT);
	}
#endif
}

//����λ��Ϣ��ʾ���б�ؼ�2
void CTechnolProcePrepareDlg::ShowWorkplaceToCombo1(void)
{
	//��ȡ��λ��Ϣ��
	TableInfo temTable = (*m_dbService)["WorkPlaceInfo"]["WorkPlaceDef"];
	//��ȡ����
	size_t nRowCount = temTable.m_Values.getRowCount();
	//��ȡ������
	size_t nCol = temTable.m_mapCols.at("WorkPlaceName");

	//��ʾ���б�ؼ�
	for (int i = nRowCount - 1; i >= 0; --i)
	{
		m_combo1.InsertString(0, temTable.m_Values[i][nCol]);
	}
	m_combo1.SetCurSel(-1);
}

//���������̱���ʾ����Ͽ�ؼ�2
void CTechnolProcePrepareDlg::ShowProcessTableListToCombo2(void)
{
	//��տؼ�����
	m_combo2.ResetContent();
	//��ȡ�������
	std::vector<STRING> temCols = (*m_dbService)["ProductionProcessInfo"].m_TableNames;

	auto iterCol = temCols.begin();
	size_t index = 0 ;
	while (iterCol != temCols.end())
	{
		m_combo2.InsertString(index++,*(iterCol++));//����ؼ�
	}
	m_combo2.SetCurSel(-1);
}

//�鿴���ݿⰴť
void CTechnolProcePrepareDlg::OnBnClickedBtnViewTableListInDB()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MessageBox("ά���С���", "sorry", MB_OK);
}

//���������ݿⰴť
void CTechnolProcePrepareDlg::OnClickedBtnSaveList1ToBd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_grid1.GetItemCount() == 0)
	{
		MessageBox("û�пɱ������Ϣ��", "����", MB_ICONERROR);
		return;
	}

	//���ñ������
	CTechnolProceDBNameSaveDlg dlg("ProductionProcessInfo");
	dlg.m_strTableName = m_strRootMdlName;
	if (dlg.DoModal() != IDOK)
	{
		return;
	}
	CString strTableName = dlg.m_strTableName;
	//��ȡ�������
	int nColumuCount = 13;//������
	int nRowCount = m_grid1.GetItemCount();//��ȡд�������
	DynaArray2D<STRING> temValues(nRowCount, nColumuCount);
	for (int i = 0; i < nRowCount; ++i)
	{
		for (int j = 0; j < nColumuCount; ++j)
		{
			temValues[i][j] = m_grid1.GetItemText(i, j);
			//temValues = m_grid1.GetItemText(i, j + 1);
		}
	}
	//CString strSQL = "CREATE TABLE " + strTableName + "(Num INTEGER not null identity(1,1),	\
		ProcLevel TEXT, MertID TEXT, MaterialName TEXT, MdlName TEXT, MdlType TEXT, CountNum INTEGER, ParentMdl TEXT, \
		ProcInfo TEXT, WorkTime TEXT, CountTime TEXT, DateInfo TEXT, AttachInfo1 TEXT)";
	CString strSQL = "CREATE TABLE " + strTableName + "(Num TEXT ,	\
		ProcLevel TEXT,	MertID TEXT,MaterialName TEXT,MdlName TEXT,MdlType TEXT,CountNum TEXT,ParentMdl TEXT,	\
		ProcInfo TEXT,WorkTime TEXT,CountTime TEXT,DateInfo TEXT,AttachInfo1 TEXT)";

	//��ӱ������
	uint32_t cmdID = m_dbService->createTableCmd("ProductionProcessInfo", strTableName, strSQL, std::move(temValues));
	g_mapDealResult.insert(std::make_pair(cmdID, updateProcTable));
	return;
}


void CTechnolProcePrepareDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CTime Time;
	Time = CTime::GetCurrentTime();
	m_StatusBar.SetPaneText(4,Time.Format("%Y-%m-%d"));
	m_StatusBar.SetPaneText(5,Time.Format("%H:%M:%S"));
	//����״̬
	if (g_statuData.nTimeCount > 0)
	{
		CString temStr = g_statuData.strInfo + "("+ std::to_string(g_statuData.nTimeCount--).c_str() +")";
		m_StatusBar.SetPaneText(1, temStr);
	}
	else 
	{
		m_StatusBar.SetPaneText(1, "");
	}
	CDialog::OnTimer(nIDEvent);
}

//˫���б�ؼ����б༭
void CTechnolProcePrepareDlg::OnDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int i=m_grid1.GetSelectionMark();
	//���û��ѡ�У�����
	if(i==-1)
	{
		return;
	}
	m_grid2.DeleteAllItems();
	////��ȡѡ��������
	CString wuliaohao,woliaoming,moxingming,leixing,liucheng,gongshi,hejigongshi,beizhu;
	wuliaohao=m_grid1.GetItemText(i,2);///���Ϻ�
	if (wuliaohao == "")
	{
		wuliaohao.Format("%s-%03d",m_strRootMdlName,i + 1);
	}
	woliaoming=m_grid1.GetItemText(i,3);////��������
	moxingming=m_grid1.GetItemText(i,4);///ģ������
	leixing=m_grid1.GetItemText(i,5);////����

	liucheng=m_grid1.GetItemText(i,8);//��������
	gongshi=m_grid1.GetItemText(i,9);//��ʱ
	hejigongshi=m_grid1.GetItemText(i,10);//�ϼƹ�ʱ
	beizhu=m_grid1.GetItemText(i,12);//��ע
	/////���ñ༭��ؼ�����
	SetDlgItemText(IDC_EDIT1,wuliaohao);
	SetDlgItemText(IDC_EDIT2,woliaoming);
	SetDlgItemText(IDC_EDIT3,moxingming);
	SetDlgItemText(IDC_EDIT4,leixing);
	SetDlgItemText(IDC_EDIT5,hejigongshi);
	SetDlgItemText(IDC_EDIT7,beizhu);
	//���ù���򣬹�ʱ��Ϊ��
	GetDlgItem(IDC_COMBO1)->SetWindowTextA("");
	GetDlgItem(IDC_EDIT6)->SetWindowTextA("");
	////�����б�ؼ�������
	int j=0;
	liucheng=liucheng+"->";
	while(liucheng.GetLength()>4)
	{
		CString xuhao,gongxu;
		xuhao.Format("%d",j+1);
		m_grid2.InsertItem(j,xuhao);
		int posi=liucheng.Find("->");
		gongxu=liucheng.Left(posi);
		m_grid2.SetItemText(j,1,gongxu);
		liucheng=liucheng.Right(liucheng.GetLength()-posi-2);
		j++;
	}
	gongshi=gongshi+"+";
	while(j>0)//������ע���¾���j��ֵ
	{
		CString dangongshi;//������ʱ
		int pos1=gongshi.Find("+");
		dangongshi=gongshi.Left(pos1);
		pos1++;
		gongshi=gongshi.Right(gongshi.GetLength()-pos1);
		m_grid2.SetItemText(m_grid2.GetItemCount()-j,2,dangongshi);
		j--;
	}
	///Ԥ��ģ��
#ifdef PRO_USE_VAR_ARGS
	CString strPath = m_strModelPath + moxingming+"."+leixing;
	m_CDprivew.ShowModelToCreoView(strPath);
#endif
	*pResult = 0;
}

//��ӹ���
void CTechnolProcePrepareDlg::OnBnClickedBtnAddSingleProgrToList2()
{
	UpdateData(true);
	CString gongwei,gongshi;
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(gongwei);
	GetDlgItem(IDC_EDIT6)->GetWindowTextA(gongshi);

	if (gongwei == "�⹺")
	{
		gongshi = "0";
		GetDlgItem(IDC_EDIT7)->SetWindowTextA(gongshi);
		m_grid2.DeleteAllItems();//��չ�������
	}
	else if(gongwei=="" || gongshi=="" || gongshi == "0")
	{
		MessageBox("��λ��ʱ����Ϊ�գ�","��ʾ");
		return;
	}
	else
	{
		gongshi.Format("%5.2f", m_fWorkTime);
		int pos = gongshi.Find(".");
		CString checkvalue = gongshi.Right(gongshi.GetLength() - pos - 1);
		if (checkvalue.GetLength() > 2)
		{
			MessageBox("��������ʱ�侫��Ϊ0.01Сʱ", "��ʾ");
			return;
		}
	}
	int i=m_grid2.GetItemCount();
	CString row(std::to_string(i + 1).c_str());
	row.Format("%d",i+1);
	m_grid2.InsertItem(i,row);
	m_grid2.SetItemText(i,1,gongwei);
	m_grid2.SetItemText(i,2,gongshi);
	////�����������ڶԻ���
	float zong=0;
	for(int j=0;j<m_grid2.GetItemCount();j++)
	{
		float k=static_cast<float>(atof(m_grid2.GetItemText(j,2)));
		zong+=k;
	}
	CString total;
	total.Format("%5.2f",zong);
	GetDlgItem(IDC_EDIT5)->SetWindowTextA(total);
}

//���±༭�õĹ������̵��������̱�
void CTechnolProcePrepareDlg::OnBnClickedBtnAddEditProcToList1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString moxingming,moxingleixing,zonggongshi,beizhu;
	GetDlgItem(IDC_EDIT3)->GetWindowTextA(moxingming);//��ȡģ����
	GetDlgItem(IDC_EDIT4)->GetWindowTextA(moxingleixing);//��ȡģ������

	if(moxingming=="")
	{
		MessageBox("û�пɸ��µ����ݣ�");
		return;
	}
	for(int i=0;i<m_grid1.GetItemCount();i++)
	{
		if(m_grid1.GetItemText(i,4)==moxingming && m_grid1.GetItemText(i,5)==moxingleixing)
		{
			CString bianghao,mingcheng,liucheng,gongshi;//���ϱ�š��������ơ����̺͹�ʱ
			GetDlgItem(IDC_EDIT1)->GetWindowTextA(bianghao);//���ϱ��
			GetDlgItem(IDC_EDIT2)->GetWindowTextA(mingcheng);//��������
			GetDlgItem(IDC_EDIT5)->GetWindowTextA(zonggongshi);//�ܹ�ʱ
			GetDlgItem(IDC_EDIT7)->GetWindowTextA(beizhu);//��ע
			//�ܹ�ʱ
			m_grid1.SetItemText(i,2,bianghao);//���ϱ��
			m_grid1.SetItemText(i,3,mingcheng);//��������
			m_grid1.SetItemText(i,10,zonggongshi);//�ܹ�ʱ
			//��������
			for(int j=0;j<m_grid2.GetItemCount();j++)
			{
				liucheng+=m_grid2.GetItemText(j,1)+"->";
			}
			liucheng=liucheng.Left(liucheng.GetLength()-2);
			m_grid1.SetItemText(i,8,liucheng);
			//��ʱ
			for(int k=0;k<m_grid2.GetItemCount();k++)
			{
				gongshi+=m_grid2.GetItemText(k,2)+"+";
			}
			gongshi=gongshi.Left(gongshi.GetLength()-1);
			m_grid1.SetItemText(i,9,gongshi);
			//��������
			CString timeStr;
			CTime day;
			day=CTime::GetCurrentTime();
			int filenameday,filenamemonth,filenameyear;
			filenameday=day.GetDay();
			filenamemonth=day.GetMonth();
			filenameyear=day.GetYear();
			timeStr.Format(_T("%04d-%02d-%02d"),filenameyear,filenamemonth,filenameday);
			m_grid1.SetItemText(i,11,timeStr);
			//���ñ�ע
			m_grid1.SetItemText(i,12,beizhu);
		}
	}
}

//��ʾ�������̱��б�ؼ�1
void CTechnolProcePrepareDlg::OnBnClickedBtnShowProcessInfoFromDBToTable()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strTableName;
	GetDlgItem(IDC_COMBO2)->GetWindowTextA(strTableName);//���ݿ��б������
	if ("" == strTableName)
	{
		return;
	}

	TableInfo temTable = (*m_dbService)["ProductionProcessInfo"][strTableName];
	int32_t nRowCount = temTable.m_Values.getRowCount();
	
#ifdef PRO_USE_VAR_ARGS
	 //����ģ������
	std::map<CString, size_t> mapIndex;//<key:Modelname.type,value:index>
	for (size_t i = 0 ; i < nRowCount ; ++i)
	{
		CString strKey = temTable.m_Values[i][4] + "." + temTable.m_Values[i][5];//ģ������+����
		mapIndex.insert(std::make_pair(strKey,i));
	}
	//�����б����嵥���������
	int nGridRowCount = m_grid1.GetItemCount();//��Ҫ���µ��б�����
	for (int i = 0; i < nGridRowCount; ++i)//ѭ����ȡ��ӵ��б�ؼ���
	{
		CString strMdlName = m_grid1.GetItemText(i, 4) + "." + m_grid1.GetItemText(i, 5);
		if (mapIndex.count(strMdlName) != 0)
		{
			size_t nIndex = mapIndex[strMdlName];
			m_grid1.SetItemText(i, 2, temTable.m_Values[nIndex][2]);
			m_grid1.SetItemText(i, 3, temTable.m_Values[nIndex][3]);
			m_grid1.SetItemText(i, 8, temTable.m_Values[nIndex][8]);
			m_grid1.SetItemText(i, 9, temTable.m_Values[nIndex][9]);
			m_grid1.SetItemText(i, 10, temTable.m_Values[nIndex][10]);
			m_grid1.SetItemText(i, 11, temTable.m_Values[nIndex][11]);
			m_grid1.SetItemText(i, 12, temTable.m_Values[nIndex][12]);
		}
	}
#else
	//�������������̱���ʾ���б���
	m_grid1.DeleteAllItems();
	for (int i = 0; i < nRowCount; ++i)
	{
		m_grid1.InsertItem(i, temTable.m_Values[i][0]);
		m_grid1.SetItemText(i, 1, temTable.m_Values[i][1]);
		m_grid1.SetItemText(i, 2, temTable.m_Values[i][2]);
		m_grid1.SetItemText(i, 3, temTable.m_Values[i][3]);
		m_grid1.SetItemText(i, 4, temTable.m_Values[i][4]);
		m_grid1.SetItemText(i, 5, temTable.m_Values[i][5]);
		m_grid1.SetItemText(i, 6, temTable.m_Values[i][6]);
		m_grid1.SetItemText(i, 7, temTable.m_Values[i][7]);
		m_grid1.SetItemText(i, 8, temTable.m_Values[i][8]);
		m_grid1.SetItemText(i, 9, temTable.m_Values[i][9]);
		m_grid1.SetItemText(i, 10, temTable.m_Values[i][10]);
		m_grid1.SetItemText(i, 11, temTable.m_Values[i][11]);
		m_grid1.SetItemText(i, 12, temTable.m_Values[i][12]);
	}
#endif
	//����״̬��
	CString strStatus;
	strStatus.Format("��ȡ�������̱� %s �ɹ�", strTableName);
	m_StatusBar.SetPaneText(1, strStatus);
	g_statuData.strInfo = strStatus;
	g_statuData.nTimeCount = 15;
}

//�б�ؼ�2�������̱༭
void CTechnolProcePrepareDlg::OnBnClickedBtnDelProgInList2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_grid2.DeleteAllItems();
	GetDlgItem(IDC_EDIT5)->SetWindowTextA("0.00");
}


void CTechnolProcePrepareDlg::OnBnClickedBtnDeleSelectedProgInLiset2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nRowIndex=m_grid2.GetSelectionMark();
	//���û��ѡ�У�����
	if(nRowIndex == -1)
	{
		return;
	}
	m_grid2.DeleteItem(nRowIndex);
	float fTotal = 0;
	for(int j = 0; j < m_grid2.GetItemCount(); j++)
	{
		float k=static_cast<float>(atof(m_grid2.GetItemText(j,2)));
		fTotal += k;
	}
	CString total;
	total.Format("%5.2f",fTotal);
	GetDlgItem(IDC_EDIT5)->SetWindowTextA(total);
	////���±��
	for(int k=0;k<m_grid2.GetItemCount();k++)
	{
		CString xuhao;
		xuhao.Format("%d",k+1);
		m_grid2.SetItemText(k,0,xuhao);
	}
}


void CTechnolProcePrepareDlg::OnBnClickedBtnMoveUperInList2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!m_grid2.GetSelectedCount())
	{
		MessageBox("��ѡ��Ҫ�ƶ��Ĺ���","��ʾ",MB_ICONEXCLAMATION);
		return;
	}
	int i=m_grid2.GetSelectionMark();
	CString str;
	str.Format("%d",i);
	//MessageBox(str);
	if(i==0)
	{
		MessageBox("�Ѿ�Ϊ��һ�У��������ƣ�","��ʾ");
		return;
	}
	CString tem1,tem2;
	tem1=m_grid2.GetItemText(i,1);
	tem2=m_grid2.GetItemText(i,2);
	m_grid2.DeleteItem(i);
	m_grid2.InsertItem(i-1,str);
	m_grid2.SetItemText(i-1,1,tem1);
	m_grid2.SetItemText(i-1,2,tem2);
	////���±��
	for(int k=0;k<m_grid2.GetItemCount();k++)
	{
		CString xuhao;
		xuhao.Format("%d",k+1);
		m_grid2.SetItemText(k,0,xuhao);
	}
}


void CTechnolProcePrepareDlg::OnBnClickedBtnMoveDownInList2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!m_grid2.GetSelectedCount())
	{
		MessageBox("��ѡ��Ҫ�ƶ��Ĺ���","��ʾ",MB_ICONEXCLAMATION);
		return;
	}
	int i=m_grid2.GetSelectionMark();
	CString str;
	str.Format("%d",i);
	//MessageBox(str);
	if(i==m_grid2.GetItemCount()-1)
	{
		MessageBox("�Ѿ�Ϊ���һ�У��������ƣ�","��ʾ");
		return;
	}
	CString tem1,tem2;
	tem1=m_grid2.GetItemText(i,1);
	tem2=m_grid2.GetItemText(i,2);
	m_grid2.DeleteItem(i);
	m_grid2.InsertItem(i+1,str);
	m_grid2.SetItemText(i+1,1,tem1);
	m_grid2.SetItemText(i+1,2,tem2);
	////���±��
	for(int k=0;k<m_grid2.GetItemCount();k++)
	{
		CString xuhao;
		xuhao.Format("%d",k+1);
		m_grid2.SetItemText(k,0,xuhao);
	}
}
