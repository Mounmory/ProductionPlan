// CDpview.cpp : �� Microsoft Visual C++ ������ ActiveX �ؼ���װ��Ķ���

#include "stdafx.h"
#include "CDpview.h"

/////////////////////////////////////////////////////////////////////////////
// CDpview

IMPLEMENT_DYNCREATE(CDpview, CWnd)

// CDpview ����

// CDpview ����
void CDpview::ShowModelToCreoView(CString mdlpath)//Ԥ��PTC Creo View Ԥ����
{
	SetProperty(0x1, VT_BSTR, mdlpath);
}
