// CDpview.h : �� Microsoft Visual C++ ������ ActiveX �ؼ���װ�������

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDpview

class CDpview : public CWnd
{
protected:
	DECLARE_DYNCREATE(CDpview)
// ����
public:
	void ShowModelToCreoView(CString mdlpath);//Ԥ��Creo View Ԥ����
};
