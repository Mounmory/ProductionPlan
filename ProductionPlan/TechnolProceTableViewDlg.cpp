// TechnolProceTableViewDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolProceTableViewDlg.h"
#include "afxdialogex.h"


// CTechnolProceTableViewDlg �Ի���

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


// CTechnolProceTableViewDlg ��Ϣ�������


BOOL CTechnolProceTableViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
