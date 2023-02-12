
// ProductionPlanMenuDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProductionPlanMenu.h"
#include "ProductionPlanMenuDlg.h"
#include "afxdialogex.h"
#include <exception>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CProductionPlanMenuDlg 对话框
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
			initProdPlanDBService Func;											//定义函数指针
			Func = (initProdPlanDBService)GetProcAddress(m_dllHandel, "initProdPlanDBService");		//获取动态链接库中的函数
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
		terminateProdPlanDBService Func;											//定义函数指针
		Func = (terminateProdPlanDBService)GetProcAddress(m_dllHandel, "terminateProdPlanDBService");		//获取动态链接库中的函数
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


// CProductionPlanMenuDlg 消息处理程序

BOOL CProductionPlanMenuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CProductionPlanMenuDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CProductionPlanMenuDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProductionPlanMenuDlg::OnClickedBtnWorkPlaceManage()
{
	// TODO: 在此添加控件通知处理程序代码
	TecnProcessPlanAndWorkPlaceViewDlg Func;											//定义函数指针
	Func = (TecnProcessPlanAndWorkPlaceViewDlg)GetProcAddress(m_dllHandel, "TecnProcessPlanAndWorkPlaceViewDlg");		//获取动态链接库中的函数
	Func();

}


void CProductionPlanMenuDlg::OnBnClickedBtnMakePlan()
{
	// TODO: 在此添加控件通知处理程序代码
	TecnProcessPlanDlg Func;											//定义函数指针
	Func = (TecnProcessPlanDlg)GetProcAddress(m_dllHandel, "TecnProcessPlanDlg");		//获取动态链接库中的函数
	Func();
}

void CProductionPlanMenuDlg::OnBnClickedBtnMakeProcess()
{
	TecnProcessPrepareDlg Func;											//定义函数指针
	Func = (TecnProcessPrepareDlg)GetProcAddress(m_dllHandel, "TecnProcessPrepareDlg");		//获取动态链接库中的函数
	Func();
}
