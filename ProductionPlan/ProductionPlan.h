// ProductionPlan.h : ProductionPlan DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CProductionPlanApp
// �йش���ʵ�ֵ���Ϣ������� ProductionPlan.cpp
//

class CProductionPlanApp : public CWinApp
{
public:
	CProductionPlanApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
