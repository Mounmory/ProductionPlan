// TechnolProceDBNameSaveDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolProceDBNameSaveDlg.h"
#include "afxdialogex.h"


// CTechnolProceDBNameSaveDlg �Ի���

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


// CTechnolProceDBNameSaveDlg ��Ϣ�������


BOOL CTechnolProceDBNameSaveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	////���öԻ���ͼ��
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CREO);//IDR_ICONΪͼ����Դ��
	SetIcon(m_hIcon, FALSE);

	//�����ݿ��б����ʾ����Ͽ�ؼ�
	std::vector<STRING> temTableName = (*m_dbService)[m_strDataBaseName].m_TableNames;

	//�����ݿ��б����ʾ����Ͽ�ؼ�
	std::vector<CString>::iterator iter = temTableName.begin();
	int i = 0 ;
	while (iter != temTableName.end())
	{
		m_Combo1.InsertString(i++,*iter++);
	}
	m_Combo1.SetCurSel(-1);

	GetDlgItem(IDC_COMBO1)->SetWindowTextA(m_strTableName);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CTechnolProceDBNameSaveDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_COMBO1)->GetWindowTextA(m_strTableName);
	if (m_strTableName == "")
	{
		return;
	}
	//��һ�μ�����ֺϷ��ԣ������в��ܳ��ֵ��ַ�

	CDialog::OnOK();
}
