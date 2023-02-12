// TechnolProceTableViewDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolProceTableViewDlg.h"
#include "afxdialogex.h"


// CTechnolProceTableViewDlg 对话框

IMPLEMENT_DYNAMIC(CTechnolProceTableViewDlg, CDialog)

CTechnolProceTableViewDlg::CTechnolProceTableViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTechnolProceTableViewDlg::IDD, pParent)
{

}

CTechnolProceTableViewDlg::~CTechnolProceTableViewDlg()
{

}

void CTechnolProceTableViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTechnolProceTableViewDlg, CDialog)
END_MESSAGE_MAP()


// CTechnolProceTableViewDlg 消息处理程序


BOOL CTechnolProceTableViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
