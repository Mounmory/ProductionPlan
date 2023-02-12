// TechnolProductionPlanResultDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolProductionPlanResultDlg.h"
#include "afxdialogex.h"
//////���浽EXCEL�ļ�
//#include "CApplication.h"
//#include "CFont0.h"
//#include "CRange.h"
//#include "CWorkbook.h"
//#include "CWorksheet.h"
//#include "CWorkbooks.h"
//#include "CWorksheets.h"

//#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

// TechnolProductionPlanResultDlg �Ի���

IMPLEMENT_DYNAMIC(TechnolProductionPlanResultDlg, CDialog)

TechnolProductionPlanResultDlg::TechnolProductionPlanResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TechnolProductionPlanResultDlg::IDD, pParent)
	,m_fTimeWeight(-1)
	,m_nColNum(5)//����������ʼ��Ϊ5
	,m_nBestReualt(0)
{

}

TechnolProductionPlanResultDlg::~TechnolProductionPlanResultDlg()
{
}

void TechnolProductionPlanResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);
}


BEGIN_MESSAGE_MAP(TechnolProductionPlanResultDlg, CDialog)
	ON_WM_CLOSE()
	//ON_BN_CLICKED(IDC_SAVE_EXCEL, &TechnolProductionPlanResultDlg::OnBnClickedSaveExcel)
	ON_BN_CLICKED(IDC_SAVE_EXCEL, &TechnolProductionPlanResultDlg::OnBnClickedSaveExcel)
END_MESSAGE_MAP()


// TechnolProductionPlanResultDlg ��Ϣ�������


BOOL TechnolProductionPlanResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	////���öԻ���ͼ��
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CREO);//IDR_ICONΪͼ����Դ��
	SetIcon(m_hIcon, FALSE);
	/////�����б�ؼ�1����
	m_list.InsertColumn(1,_T("ÿ���Ų���"),LVCFMT_CENTER,80,0);
	m_list.InsertColumn(2,_T("���۷���"),LVCFMT_CENTER,60,0);
	m_list.InsertColumn(3,_T("�����ۼ�����"),LVCFMT_CENTER,85,0);
	m_list.InsertColumn(4,_T("�����ۼ�����"),LVCFMT_CENTER,85,0);
	m_list.InsertColumn(5,_T("��λ�ۼ�����"),LVCFMT_CENTER,85,0);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	//////��ʾ�б�ؼ�
	for(int i = 0; i < m_strShowListValue.getRowCount(); ++i)
	{
		m_list.InsertItem(i,m_strShowListValue[i][0]);//���
		m_list.SetItemText(i,1,m_strShowListValue[i][1]);//���۷���
		m_list.SetItemText(i,2,m_strShowListValue[i][2]);//�����ۼ���������
		m_list.SetItemText(i,3,m_strShowListValue[i][3]);//��λ�ۼƹ�������
		m_list.SetItemText(i,4,m_strShowListValue[i][4]);//��λ�ۼƹ�������
	}

	GetDlgItem(IDC_SHOW_RESULT)->SetWindowTextA(m_strShowEditOne);
	GetDlgItem(IDC_SHOW_RESULT1)->SetWindowTextA(m_StrShowEditTwo);
	//����һ�м�����ʾ
	if (m_list.GetItemCount())
	{
		m_list.SetItemState(m_nBestReualt,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void TechnolProductionPlanResultDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	DestroyWindow();
	CDialog::OnClose();
}

int TechnolProductionPlanResultDlg::CalculatBestSolution(const std::vector<float>& fVecTimeLong, const std::vector<float>& fVecTotalDay, const std::vector<float>& fVecTotalWorkPlace)
{
	if (fVecTimeLong.size() != fVecTotalDay.size() || fVecTimeLong.size() != fVecTotalWorkPlace.size())
	{
		return 0;
	}
	int nRowCount = fVecTimeLong.size() ;//�ܵ���Ҫ���е�����

	//������ʱ��������¼vector��������Сֵ
	float	fDayLongMax(fVecTimeLong[0]),//���������
			fDayLongMin(fVecTimeLong[0]),//��̹�������
			fWorkDayMax(fVecTotalDay[0]),//����ۼ�����
			fWorkDayMin(fVecTotalDay[0]),//��С�ۼ�����
			fWorkPlaceMax(fVecTotalWorkPlace[0]),//���ռ�ù�λ����
			fWorkPlaceMin(fVecTotalWorkPlace[0]);//��Сռ�ù�λ����
	//�����ֵ����Сֵ
	for(int i = 0; i < nRowCount; ++i)
	{
		if (fDayLongMax < fVecTimeLong[i])	fDayLongMax =  fVecTimeLong[i];
		if (fDayLongMin > fVecTimeLong[i])	fDayLongMin = fVecTimeLong[i];
		if (fWorkDayMax < fVecTotalDay[i])	fWorkDayMax = fVecTotalDay[i];
		if (fWorkDayMin > fVecTotalDay[i])	fWorkDayMin = fVecTotalDay[i];
		if (fWorkPlaceMax < fVecTotalWorkPlace[i])	fWorkPlaceMax = fVecTotalWorkPlace[i];
		if (fWorkPlaceMin > fVecTotalWorkPlace[i])	fWorkPlaceMin = fVecTotalWorkPlace[i];
	}
	////**����ʱ��Ȩ�����⣬�ɸ����������ֵ����Сֵ�Ĳ�ֵ��ȷ�����Բ�ֵΪ15��Ϊ0.5��ÿ��һ�죬Ȩ�����0.5/15***///////
	//����ʱ��Ȩ��
	if( m_fTimeWeight < 0 || m_fTimeWeight > 1)//ʱ��Ȩ��ΪĬ��,����ʱ��Ȩ��
	{
		 m_fTimeWeight = ( fDayLongMax- fDayLongMin)/ fDayLongMax;//Ȩ��=���������-��С���ڣ�/�������
	}
	//������������Ȩ�ؼ���λȨ��
	float	fWorkDayWeight(0),
			fWorkPlaceWeight(0);
	float tmpWorkDay = ( fWorkDayMax - fWorkDayMin)/ fWorkDayMax;//�����ۼ�����Ȩ��
	float tmpWoekPlace =( fWorkPlaceMax - fWorkPlaceMin)/ fWorkPlaceMax;//��λ�ۼ�����Ȩ��
	 fWorkDayWeight = (1- m_fTimeWeight)*tmpWorkDay/(tmpWorkDay+tmpWoekPlace);
	 tmpWoekPlace = (1- m_fTimeWeight)*tmpWoekPlace/(tmpWorkDay+tmpWoekPlace);

	////�������������µĵ÷�
	 m_strShowListValue.reSizeArray(nRowCount,m_nColNum);//��ʼ����ʾ���б�ؼ��е�����
	 float fScoreMax = 0;//��¼��ߵ÷�
	 std::vector<float> fVecScore(nRowCount, 0);//��¼����÷�

	for(int i = 0; i < nRowCount; ++i)
	{
		//��������������ķ���
		float	fscore1(0),	fscore2(0),	fscore3(0);//��ʼ��Ϊ0
		//��ĸ����Ϊ0������ĸΪ0����Ȩ��ҲΪ0
		if ( fDayLongMax- fDayLongMin != 0)
		{
			fscore1=( fDayLongMax- fVecTimeLong[i])/( fDayLongMax- fDayLongMin)*( m_fTimeWeight)*100;
		}
		if ( fWorkDayMax- fWorkDayMin !=0)
		{
			fscore2=( fWorkDayMax - fVecTotalDay[i])/( fWorkDayMax- fWorkDayMin)* fWorkDayWeight*100;
		}
		if ( fWorkPlaceMax- fWorkPlaceMin !=0)
		{
			fscore3=( fWorkPlaceMax- fVecTotalWorkPlace[i])/( fWorkPlaceMax- fWorkPlaceMin)* tmpWoekPlace*100;
		}
		fVecScore[i] = fscore1 +fscore2 +fscore3;//��ǰ�з���
		if (fScoreMax < fVecScore[i])//��¼��ߵ÷�
		{
			m_nBestReualt = i;
			fScoreMax = fVecScore[i];
		}
		m_strShowListValue[i][0].Format("%d",i+1);//ÿ���Ų�����
		m_strShowListValue[i][1].Format("%.2f", fVecScore[i]);//�÷�
		m_strShowListValue[i][2].Format("%.0f", fVecTimeLong[i]);//��������
		m_strShowListValue[i][3].Format("%g", fVecTotalDay[i]);//�����ۼ�����
		m_strShowListValue[i][4].Format("%g", fVecTotalWorkPlace[i]);//��λ�ۼ�����
	}
	//��ʾ����һ���༭��ؼ�����
	m_strShowEditOne.Format("��ѷ���Ϊ��%d����۷���Ϊ%.2f��",m_nBestReualt + 1, fScoreMax);

	//////��������������ʾ��ʹ����Ȩ��ϵ����Ϊ1
	CString strTimeWeight,strDayWeight;
	strTimeWeight.Format("%.2f",m_fTimeWeight);//����Ȩ��
	strDayWeight.Format("%.2f",fWorkDayWeight);//���*��������Ȩ��
	float fTmp = static_cast<float>(atof(strTimeWeight)+atof(strDayWeight));
	fWorkPlaceWeight = 1 - fTmp;//��ʱȨ��

	//��ʾ���ڶ����༭��ؼ�����
	m_StrShowEditTwo.Format("��������Ȩ��%.2f����������Ȩ��%.2f����λʹ��Ȩ��%.2f",m_fTimeWeight,fWorkDayWeight,fWorkPlaceWeight);
	
	//����Ȩ��Ϊ������
	m_fTimeWeight = -1;

	//��������Ų�����
	return  m_nBestReualt +1;
	
}

//extern void GetCellName(int nRow, int nCol, CString &strName);//////���EXCEL���������

////���浽EXCEL
//void TechnolProductionPlanResultDlg::OnBnClickedSaveExcel()
//{
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	CFileDialog dlg(FALSE,_T("(*.xlsx)"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("(*.xlsx)|*.xlsx||"),NULL);
//	if (dlg.DoModal() == IDOK)
//	{
//		//��ȡ·��
//		CString strFile = dlg.GetPathName();
//		COleVariant
//		covTrue((short)TRUE),
//		covFalse((short)FALSE),
//		covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
//		CApplication app;
//		CWorkbooks books;
//		CWorkbook book;
//		CWorksheets sheets;
//		CWorksheet sheet;
//		CRange range;
//		CFont0 font;
//		if (!app.CreateDispatch(_T("Excel.Application")))
//		{
//			MessageBox(_T("����ʧ�ܣ�"));
//			return;
//		}
//		//Get a new workbook.
//		books = app.get_Workbooks();
//		book = books.Add(covOptional);
//		sheets = book.get_Worksheets();
//		sheet = sheets.get_Item(COleVariant((short)1));
//////////////////////////////////////CListCtrl �ؼ�report ���
//		CHeaderCtrl *pmyHeaderCtrl;
//		pmyHeaderCtrl = m_list.GetHeaderCtrl();//�˾�ȡ��CListCtrl �ؼ����б�ͷ
//
//		int iRow,iCol;
//		int m_cols = pmyHeaderCtrl-> GetItemCount();
//		int m_rows = m_list.GetItemCount();
//		
//		HDITEM hdi;
//		TCHAR lpBuffer[256];
//		bool fFound = false;
//		hdi.mask = HDI_TEXT;
//		hdi.pszText = lpBuffer;
//		hdi.cchTextMax = 256;
//		CString colname;
//		CString strTemp;
//		if(m_cols>78)//��߲��ܳ���52��
//		{
//			MessageBox("����������52�У�����ϵ���������Ա����Դ����");
//			return;
//		}
//		for(iCol=0; iCol <m_cols; iCol++)//���б�ı���ͷд��EXCEL
//		{
//			GetCellName(1 ,iCol + 1, colname);
//			range = sheet.get_Range(COleVariant(colname),COleVariant(colname));
//			pmyHeaderCtrl-> GetItem(iCol, &hdi);
//			range.put_Value2(COleVariant(hdi.pszText));
//			int nWidth = m_list.GetColumnWidth(iCol)/6;
//			//�õ���iCol+1 ��
//			range.AttachDispatch(range.get_Item(_variant_t((long)(iCol+1)),vtMissing).pdispVal,true);
//			//�����п�
//			range.put_ColumnWidth(_variant_t((long)nWidth));
//		}
//		range = sheet.get_Range(COleVariant( _T("A1")), COleVariant(colname));
//		range.put_RowHeight(_variant_t((long)50));//�����еĸ߶�
//		font = range.get_Font();
//		font.put_Bold(covTrue);
//		range.put_VerticalAlignment(COleVariant((short)-4108));//xlVAlignCenter =-4108
//		COleSafeArray saRet;
//		DWORD numElements[]={m_rows,m_cols}; //5x2 element array
//		saRet.Create(VT_BSTR, 2, numElements);
//		range = sheet.get_Range(COleVariant( _T("A2 ")),covOptional);
//		range =
//		range.get_Resize(COleVariant((short)m_rows),COleVariant((short)m_cols));
//		long index[2];
//		range = sheet.get_Range(COleVariant( _T("A2 ")),covOptional);
//		range = range.get_Resize(COleVariant((short)m_rows),COleVariant((short)	m_cols));
//		/////��ʾ������
//		for ( iRow = 1; iRow <= m_rows; iRow++)//���б�����д��EXCEL
//		{
//			for ( iCol = 1; iCol <= m_cols; iCol++)
//			{
//				index[0]=iRow-1;
//				index[1]=iCol-1;
//				CString szTemp;
//				szTemp=m_list.GetItemText(iRow-1,iCol-1);
//				BSTR bstr = szTemp.AllocSysString();
//				saRet.PutElement(index,bstr);
//				SysFreeString(bstr);
//				//////��ʾ����
//			}
//		}
//		range.put_Value2(COleVariant(saRet));
//		saRet.Detach();
//		book.SaveCopyAs(COleVariant(strFile));
//		book.put_Saved(true);
//		book.ReleaseDispatch();
//		books.ReleaseDispatch();
//		app.Quit();
//		app.ReleaseDispatch();
//		///ѯ���Ƿ��
//		CString messa;
//		messa.Format("����ɹ���\n�ļ��ѳɹ�������%s\n�Ƿ���ļ���",strFile);
//		if(MessageBox(messa,"��ϲ��",MB_YESNO)==IDYES)
//		{
//			ShellExecute(nullptr, _T("open"), strFile, _T(""), _T(""), SW_SHOW);
//		}
//	}
//}


void TechnolProductionPlanResultDlg::OnBnClickedSaveExcel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MessageBox("ά���С���", "sorry", MB_OK);
}

void TechnolProductionPlanResultDlg::setTimeWeight(float ftimeWeigth) 
{
	m_fTimeWeight = ftimeWeigth;
}