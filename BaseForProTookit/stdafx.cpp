// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// ADOConn.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������

#ifdef PRO_USE_VAR_ARGS	//��Creo��ע��ʹ��dll
#include <ProMessage.h>

//ע��Creo��dll�ӿ�
extern "C" int user_initialize(int argc, char *argv[])
{
	ProMessageDisplay(L"message.txt", "Starting CreTookitDevelop Success!");//ͷ�ļ� ProMessage.h.�˴�������һ��ProE��������
	return PRO_TK_NO_ERROR;
}
extern "C" void user_terminate()
{

}

#endif
