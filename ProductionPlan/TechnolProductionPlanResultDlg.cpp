// TechnolProductionPlanResultDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProductionPlan.h"
#include "TechnolProductionPlanResultDlg.h"
#include "afxdialogex.h"
//////保存到EXCEL文件
//#include "CApplication.h"
//#include "CFont0.h"
//#include "CRange.h"
//#include "CWorkbook.h"
//#include "CWorksheet.h"
//#include "CWorkbooks.h"
//#include "CWorksheets.h"

//#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

// TechnolProductionPlanResultDlg 对话框

IMPLEMENT_DYNAMIC(TechnolProductionPlanResultDlg, CDialog)

TechnolProductionPlanResultDlg::TechnolProductionPlanResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(TechnolProductionPlanResultDlg::IDD, pParent)
	,m_fTimeWeight(-1)
	,m_nColNum(5)//总列数，初始化为5
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


// TechnolProductionPlanResultDlg 消息处理程序


BOOL TechnolProductionPlanResultDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	////设置对话框图标
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CREO);//IDR_ICON为图标资源名
	SetIcon(m_hIcon, FALSE);
	/////设置列表控件1标题
	m_list.InsertColumn(1,_T("每组排产数"),LVCFMT_CENTER,80,0);
	m_list.InsertColumn(2,_T("评价分数"),LVCFMT_CENTER,60,0);
	m_list.InsertColumn(3,_T("周期累计天数"),LVCFMT_CENTER,85,0);
	m_list.InsertColumn(4,_T("物料累计天数"),LVCFMT_CENTER,85,0);
	m_list.InsertColumn(5,_T("工位累计天数"),LVCFMT_CENTER,85,0);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	//////显示列表控件
	for(int i = 0; i < m_strShowListValue.getRowCount(); ++i)
	{
		m_list.InsertItem(i,m_strShowListValue[i][0]);//序号
		m_list.SetItemText(i,1,m_strShowListValue[i][1]);//评价分数
		m_list.SetItemText(i,2,m_strShowListValue[i][2]);//物料累计生产天数
		m_list.SetItemText(i,3,m_strShowListValue[i][3]);//工位累计工作天数
		m_list.SetItemText(i,4,m_strShowListValue[i][4]);//工位累计工作天数
	}

	GetDlgItem(IDC_SHOW_RESULT)->SetWindowTextA(m_strShowEditOne);
	GetDlgItem(IDC_SHOW_RESULT1)->SetWindowTextA(m_StrShowEditTwo);
	//最优一行加亮显示
	if (m_list.GetItemCount())
	{
		m_list.SetItemState(m_nBestReualt,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void TechnolProductionPlanResultDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	DestroyWindow();
	CDialog::OnClose();
}

int TechnolProductionPlanResultDlg::CalculatBestSolution(const std::vector<float>& fVecTimeLong, const std::vector<float>& fVecTotalDay, const std::vector<float>& fVecTotalWorkPlace)
{
	if (fVecTimeLong.size() != fVecTotalDay.size() || fVecTimeLong.size() != fVecTotalWorkPlace.size())
	{
		return 0;
	}
	int nRowCount = fVecTimeLong.size() ;//总的需要排列的数量

	//定义临时变量，记录vector中最大和最小值
	float	fDayLongMax(fVecTimeLong[0]),//最长工作周期
			fDayLongMin(fVecTimeLong[0]),//最短工作周期
			fWorkDayMax(fVecTotalDay[0]),//最大累计天数
			fWorkDayMin(fVecTotalDay[0]),//最小累计天数
			fWorkPlaceMax(fVecTotalWorkPlace[0]),//最大占用工位数量
			fWorkPlaceMin(fVecTotalWorkPlace[0]);//最小占用工位数量
	//找最大值和最小值
	for(int i = 0; i < nRowCount; ++i)
	{
		if (fDayLongMax < fVecTimeLong[i])	fDayLongMax =  fVecTimeLong[i];
		if (fDayLongMin > fVecTimeLong[i])	fDayLongMin = fVecTimeLong[i];
		if (fWorkDayMax < fVecTotalDay[i])	fWorkDayMax = fVecTotalDay[i];
		if (fWorkDayMin > fVecTotalDay[i])	fWorkDayMin = fVecTotalDay[i];
		if (fWorkPlaceMax < fVecTotalWorkPlace[i])	fWorkPlaceMax = fVecTotalWorkPlace[i];
		if (fWorkPlaceMin > fVecTotalWorkPlace[i])	fWorkPlaceMin = fVecTotalWorkPlace[i];
	}
	////**关于时间权的问题，可根据周期最大值与最小值的差值来确定，以差值为15天为0.5，每差一天，权重相差0.5/15***///////
	//计算时间权重
	if( m_fTimeWeight < 0 || m_fTimeWeight > 1)//时间权重为默认,计算时间权重
	{
		 m_fTimeWeight = ( fDayLongMax- fDayLongMin)/ fDayLongMax;//权重=（最大周期-最小周期）/最大周期
	}
	//计算物料排列权重及工位权重
	float	fWorkDayWeight(0),
			fWorkPlaceWeight(0);
	float tmpWorkDay = ( fWorkDayMax - fWorkDayMin)/ fWorkDayMax;//物料累计天数权重
	float tmpWoekPlace =( fWorkPlaceMax - fWorkPlaceMin)/ fWorkPlaceMax;//工位累计天数权重
	 fWorkDayWeight = (1- m_fTimeWeight)*tmpWorkDay/(tmpWorkDay+tmpWoekPlace);
	 tmpWoekPlace = (1- m_fTimeWeight)*tmpWoekPlace/(tmpWorkDay+tmpWoekPlace);

	////计算各排列情况下的得分
	 m_strShowListValue.reSizeArray(nRowCount,m_nColNum);//初始化显示到列表控件中的内容
	 float fScoreMax = 0;//记录最高得分
	 std::vector<float> fVecScore(nRowCount, 0);//记录各项得分

	for(int i = 0; i < nRowCount; ++i)
	{
		//计算各种排序结果的分数
		float	fscore1(0),	fscore2(0),	fscore3(0);//初始化为0
		//分母不能为0，若分母为0，则权重也为0
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
		fVecScore[i] = fscore1 +fscore2 +fscore3;//当前行分数
		if (fScoreMax < fVecScore[i])//记录最高得分
		{
			m_nBestReualt = i;
			fScoreMax = fVecScore[i];
		}
		m_strShowListValue[i][0].Format("%d",i+1);//每次排产数量
		m_strShowListValue[i][1].Format("%.2f", fVecScore[i]);//得分
		m_strShowListValue[i][2].Format("%.0f", fVecTimeLong[i]);//生产周期
		m_strShowListValue[i][3].Format("%g", fVecTotalDay[i]);//物料累计天数
		m_strShowListValue[i][4].Format("%g", fVecTotalWorkPlace[i]);//工位累计天数
	}
	//显示到第一个编辑框控件内容
	m_strShowEditOne.Format("最佳方案为第%d项，评价分数为%.2f。",m_nBestReualt + 1, fScoreMax);

	//////重新整理，消除显示误差，使三个权重系数和为1
	CString strTimeWeight,strDayWeight;
	strTimeWeight.Format("%.2f",m_fTimeWeight);//周期权重
	strDayWeight.Format("%.2f",fWorkDayWeight);//零件*生产天数权重
	float fTmp = static_cast<float>(atof(strTimeWeight)+atof(strDayWeight));
	fWorkPlaceWeight = 1 - fTmp;//工时权重

	//显示到第二个编辑框控件内容
	m_StrShowEditTwo.Format("生产周期权重%.2f，物料排列权重%.2f，工位使用权重%.2f",m_fTimeWeight,fWorkDayWeight,fWorkPlaceWeight);
	
	//设置权重为不可用
	m_fTimeWeight = -1;

	//返回最佳排产数量
	return  m_nBestReualt +1;
	
}

//extern void GetCellName(int nRow, int nCol, CString &strName);//////获得EXCEL表各行与列

////保存到EXCEL
//void TechnolProductionPlanResultDlg::OnBnClickedSaveExcel()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	CFileDialog dlg(FALSE,_T("(*.xlsx)"),NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("(*.xlsx)|*.xlsx||"),NULL);
//	if (dlg.DoModal() == IDOK)
//	{
//		//获取路径
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
//			MessageBox(_T("创建失败！"));
//			return;
//		}
//		//Get a new workbook.
//		books = app.get_Workbooks();
//		book = books.Add(covOptional);
//		sheets = book.get_Worksheets();
//		sheet = sheets.get_Item(COleVariant((short)1));
//////////////////////////////////////CListCtrl 控件report 风格
//		CHeaderCtrl *pmyHeaderCtrl;
//		pmyHeaderCtrl = m_list.GetHeaderCtrl();//此句取得CListCtrl 控件的列表头
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
//		if(m_cols>78)//最高不能超过52列
//		{
//			MessageBox("总列数超过52列，请联系计算机管理员更改源程序");
//			return;
//		}
//		for(iCol=0; iCol <m_cols; iCol++)//将列表的标题头写入EXCEL
//		{
//			GetCellName(1 ,iCol + 1, colname);
//			range = sheet.get_Range(COleVariant(colname),COleVariant(colname));
//			pmyHeaderCtrl-> GetItem(iCol, &hdi);
//			range.put_Value2(COleVariant(hdi.pszText));
//			int nWidth = m_list.GetColumnWidth(iCol)/6;
//			//得到第iCol+1 列
//			range.AttachDispatch(range.get_Item(_variant_t((long)(iCol+1)),vtMissing).pdispVal,true);
//			//设置列宽
//			range.put_ColumnWidth(_variant_t((long)nWidth));
//		}
//		range = sheet.get_Range(COleVariant( _T("A1")), COleVariant(colname));
//		range.put_RowHeight(_variant_t((long)50));//设置行的高度
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
//		/////显示进度条
//		for ( iRow = 1; iRow <= m_rows; iRow++)//将列表内容写入EXCEL
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
//				//////显示进度
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
//		///询问是否打开
//		CString messa;
//		messa.Format("保存成功！\n文件已成功保存至%s\n是否打开文件？",strFile);
//		if(MessageBox(messa,"恭喜！",MB_YESNO)==IDYES)
//		{
//			ShellExecute(nullptr, _T("open"), strFile, _T(""), _T(""), SW_SHOW);
//		}
//	}
//}


void TechnolProductionPlanResultDlg::OnBnClickedSaveExcel()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox("维护中……", "sorry", MB_OK);
}

void TechnolProductionPlanResultDlg::setTimeWeight(float ftimeWeigth) 
{
	m_fTimeWeight = ftimeWeigth;
}