// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE ��
#include <afxodlgs.h>       // MFC OLE �Ի�����
#include <afxdisp.h>        // MFC �Զ�����
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC ���ݿ���
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO ���ݿ���
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

//C++��׼��ͷ�ļ�
#include <io.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>

//������
#pragma comment(lib,"BaseForProTookit.lib")
#include "DynaArray.h"
#include "threadpool.h"
#include "Logger.h"

//���ݿ����
#pragma comment(lib,"DataBaseService.lib")
#include "DataType.h"
#include "DBService.h"
#include "DBCommonAPI.h"
#include "DBAccessConn.h"

//�����ƻ����㷽����
#pragma comment(lib,"ProductionPlanCalculateLib.lib")

////���ܺ����ӿ���
//ȫ�ֹ��ܺ���ͷ�ļ�
#include "CreoFuntion/CreoCommonFunction.h"
#include "CreoFuntion/CDpview.h"//Creo View�ؼ�Ԥ���ļ�

using namespace DataBase;

std::shared_ptr<DBService>& getProPlanDBService();

struct StatusData 
{
	CString strInfo;//״̬����Ϣ
	int32_t nTimeCount;//��ʾ��ʱ
};
#ifdef PRO_USE_VAR_ARGS
////ProTookitͷ�ļ�
//*************���Pro/Tookitͷ�ļ�
#include <ProToolkit.h>
#include <ProUtil.h>
#include <ProArray.h>
#include <ProDimension.h>
#include <ProWindows.h>
#include <ProSolid.h>
#include <ProParameter.h>
#include <ProParamval.h>
#include <ProMessage.h>
#include <ProMenu.h>
#include <ProMenuBar.h>
#include <ProMdl.h>
#include <ProModelitem.h>
#include <ProDrawing.h>
#include <ProFeature.h>
#include <ProFeatType.h>
#include <ProDrawing.h>
#include <ProNotify.h>
#include <ProRefInfo.h>
#include <ProRelSet.h>
#include <ProSelection.h>
#include <ProUICmd.h>
#include <ProNotify.h>
#include <ProPopupmenu.h>
#include <ProSelbuffer.h>
#include <ProAsmcomp.h>
#include <ProAsmcomppath.h>
#include <afxdisp.h>
#include <afxcontrolbars.h>
//////�������Ŀ¼�ظ��ļ�ר��
#include <ProUICmd.h>
#include <ProUIMessage.h>
#include <ProToolkitDll.h>
////һ����ɫר��
#include <ProSurface.h>
#include <afxdisp.h>

#endif
//ȫ�ֺ���

