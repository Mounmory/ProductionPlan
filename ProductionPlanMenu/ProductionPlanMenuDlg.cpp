
// ProductionPlanMenuDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProductionPlanMenu.h"
#include "ProductionPlanMenuDlg.h"
#include "afxdialogex.h"
#include <exception>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProductionPlanMenuDlg �Ի���
typedef void(*initProdPlanDBService)(void);
typedef void(*terminateProdPlanDBService)(void);
typedef void(*TecnProcessPlanAndWorkPlaceViewDlg)(void);
typedef void(*TecnProcessPlanDlg)(void);
typedef void(*TecnProcessPrepareDlg)(void);

CProductionPlanMenuDlg::CProductionPlanMenuDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PRODUCTIONPLANMENU_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	try
	{
		m_dllHandel = LoadLibrary("ProductionPlan");
		if (m_dllHandel == NULL)
		{
			MessageBox("load library ProductionPlan failed!");
		}
		else
		{
			initProdPlanDBService Func;											//���庯��ָ��
			Func = (initProdPlanDBService)GetProcAddress(m_dllHandel, "initProdPlanDBService");		//��ȡ��̬���ӿ��еĺ���
			Func();
		}
	}
	catch (...)
	{
		MessageBox("load library ProductionPlan failed!");
	}
}

CProductionPlanMenuDlg::~CProductionPlanMenuDlg() 
{
	if (m_dllHandel != NULL)
	{
		terminateProdPlanDBService Func;											//���庯��ָ��
		Func = (terminateProdPlanDBService)GetProcAddress(m_dllHandel, "terminateProdPlanDBService");		//��ȡ��̬���ӿ��еĺ���
		Func();
		FreeLibrary(m_dllHandel);
	}
}

void CProductionPlanMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProductionPlanMenuDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_WORK_PLACE_MANAGE, &CProductionPlanMenuDlg::OnClickedBtnWorkPlaceManage)
	ON_BN_CLICKED(IDC_BTN_MAKE_PLAN, &CProductionPlanMenuDlg::OnBnClickedBtnMakePlan)
	ON_BN_CLICKED(IDC_BTN_MAKE_PROCESS, &CProductionPlanMenuDlg::OnBnClickedBtnMakeProcess)
END_MESSAGE_MAP()


// CProductionPlanMenuDlg ��Ϣ�������

BOOL CProductionPlanMenuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CProductionPlanMenuDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CProductionPlanMenuDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProductionPlanMenuDlg::OnClickedBtnWorkPlaceManage()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TecnProcessPlanAndWorkPlaceViewDlg Func;											//���庯��ָ��
	Func = (TecnProcessPlanAndWorkPlaceViewDlg)GetProcAddress(m_dllHandel, "TecnProcessPlanAndWorkPlaceViewDlg");		//��ȡ��̬���ӿ��еĺ���
	Func();

}


void CProductionPlanMenuDlg::OnBnClickedBtnMakePlan()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TecnProcessPlanDlg Func;											//���庯��ָ��
	Func = (TecnProcessPlanDlg)GetProcAddress(m_dllHandel, "TecnProcessPlanDlg");		//��ȡ��̬���ӿ��еĺ���
	Func();
}

void CProductionPlanMenuDlg::OnBnClickedBtnMakeProcess()
{
	TecnProcessPrepareDlg Func;											//���庯��ָ��
	Func = (TecnProcessPrepareDlg)GetProcAddress(m_dllHandel, "TecnProcessPrepareDlg");		//��ȡ��̬���ӿ��еĺ���
	Func();
}
