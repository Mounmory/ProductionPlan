// TechnolProceDBNameSaveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolProceDBNameSaveDlg.h"
#include "afxdialogex.h"


// CTechnolProceDBNameSaveDlg 对话框

IMPLEMENT_DYNAMIC(CTechnolProceDBNameSaveDlg, CDialog)


CTechnolProceDBNameSaveDlg::CTechnolProceDBNameSaveDlg(CString strDBName, CWnd* pParent /*=NULL*/)
	: CDialog(CTechnolProceDBNameSaveDlg::IDD, pParent)
	,m_strDataBaseName(strDBName)
	, m_dbService(getProPlanDBService())
{

}

CTechnolProceDBNameSaveDlg::~CTechnolProceDBNameSaveDlg()
{

}

void CTechnolProceDBNameSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_Combo1);
}


BEGIN_MESSAGE_MAP(CTechnolProceDBNameSaveDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CTechnolProceDBNameSaveDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTechnolProceDBNameSaveDlg 消息处理程序


BOOL CTechnolProceDBNameSaveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	////设置对话框图标
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CREO);//IDR_ICON为图标资源名
	SetIcon(m_hIcon, FALSE);

	//将数据库中表格显示到组合框控件
	std::vector<STRING> temTableName = (*m_dbService)[m_strDataBaseName].m_TableNames;

	//将数据库中表格显示到组合框控件
	std::vector<CString>::iterator iter = temTableName.begin();
	int i = 0 ;
	while (iter != temTableName.end())
	{
		m_Combo1.InsertString(i++,*iter++);
	}
	m_Combo1.SetCurSel(-1);

	GetDlgItem(IDC_COMBO1)->SetWindowTextA(m_strTableName);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CTechnolProceDBNameSaveDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(m_strTableName);
	if (m_strTableName == "")
	{
		return;
	}
	//加一段检查名字合法性，不许有不能出现的字符

	CDialog::OnOK();
}
