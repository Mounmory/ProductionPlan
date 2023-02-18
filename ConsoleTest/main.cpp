#include <iostream>

//���ݿ�����ͷ�ļ�
#include <afx.h>

#pragma comment(lib,"BaseForProTookit.lib")//���ADO���ӿ�
#include "DynaArray.h"
#include "DynaArrayEx.h"
#include "Logger.h"
#include "threadpool.h"

#pragma comment(lib,"DataBaseService.lib")//���ADO���ӿ�
#include "DataType.h"
#include "DBService.h"
#include "DBCommonAPI.h"
#include "DBAccessConn.h"

void testCaseDynamicArrayStr();

void testCaseDynamicArrayInt();

void testCaseDynamicArrayIntEx();

void testCaseDataBaseService();

void testCaseThreadPool();

int main() 
{

	testCaseDynamicArrayIntEx();//��̬�������

	//testCaseThreadPool();

	system("pause");
	return 0;
}


void testCaseDynamicArrayStr()
{
	std::cout << "test CString begin!" << std::endl;
	try
	{
		int count = 0;
		while (count++ < 10000)
		{
			LoggerPtr logPtr = Logger::getLogger();
			logPtr->LogForce("[%s][%d] log force test!", __STRING_FUNCTION__, __LINE__);
			logPtr->LogForce("");
			//string ����
			DynaArray2D<CString> emptyInt;//Ĭ�Ϲ��캯�������������
			DynaArray2D<CString> emptyInt1(0,2);//Ĭ�Ϲ��캯�������������
			DynaArray2D<CString> emptyInt2(2, 0);
			emptyInt1 = "hello";
			emptyInt2 = "ok";
			emptyInt2.reSizeArray(3, 0);
			emptyInt2 = "ok";
			emptyInt1 = emptyInt2;

			DynaArray2D<CString> arrayInt1(2, 2);//���������С

			std::cout << "1������1Ĭ��ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;//��ӡĬ�Ϲ���ֵ
			logPtr->LogByOstream() << "1������1Ĭ��ֵ" << std::endl;
			logPtr->LogByOstream() << arrayInt1;//��ӡĬ�Ϲ���ֵ

			arrayInt1.zeroArray();//��ֵ����
			std::cout << "2������1����ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;//��ӡ����

			arrayInt1 = "arrayInt1";//���帳ֵ
			arrayInt1[1][0] = "change10";
			std::cout << "3������1�޸�ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;

			DynaArray2D<CString> arrayInt2(arrayInt1);//�������캯��
			std::cout << "4������2ʹ������1��������ֵ" << std::endl;
			std::cout << arrayInt2 << std::endl;

			arrayInt2.reSizeArray(3, 4);

			arrayInt2 = "changeInt2";
			std::cout << "5������2ʹ�ø�ֵ����������ڳ���" << std::endl;
			std::cout << arrayInt2 << std::endl;

			arrayInt1 = arrayInt2;
			arrayInt2[0][0] = "changearray2";
			std::cout << "6������1ʹ�ø�ֵ���������������2" << std::endl;
			std::cout << arrayInt1 << std::endl;

			arrayInt2 = arrayInt1;
			arrayInt1 = arrayInt2;
			arrayInt1 = arrayInt1;
			std::cout << "7������1ʹ�ø�ֵ��������Ը�ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;

			DynaArray2D<CString> arrayInt3(std::move(arrayInt1));
			std::cout << "8������3ʹ���ƶ����캯��" << std::endl;
			std::cout << arrayInt3 << std::endl;

			arrayInt3.reSizeArray(5, 6);
			arrayInt3 = "arrayInt3";
			std::cout << "9����������2��3" << std::endl;
			std::cout << "����ǰ2��3��" << std::endl;
			std::cout << arrayInt2 << std::endl;
			std::cout << arrayInt3 << std::endl;
			std::swap(arrayInt3, arrayInt2);
			std::cout << "������2��3��" << std::endl;
			std::cout << arrayInt2 << std::endl;
			std::cout << arrayInt3 << std::endl;

			std::cout << "10��3�����ƶ���ֵ�����" << std::endl;
			arrayInt3 = std::move(arrayInt3);
			std::cout << arrayInt3 << std::endl;

			std::cout << "11��2��3�����ƶ���ֵ�����" << std::endl;
			arrayInt3 = std::move(arrayInt2);
			std::cout << "3��ֵ" << std::endl;
			std::cout << arrayInt3 << std::endl;
			std::cout << "2��ֵ" << std::endl;
			std::cout << arrayInt2 << std::endl;
		}
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}

void testCaseDynamicArrayInt()
{
	//����Int����
	std::cout << "test int begin!" << std::endl;
	try
	{
		int count = 0;
		while (count++ < 10000)
		{
			LoggerPtr logPtr = Logger::getLogger();
			logPtr->LogForce("[%s][%d] log force test!", __STRING_FUNCTION__, __LINE__);
			logPtr->LogForce("");
			DynaArray2D<int> emptyInt;//Ĭ�Ϲ��캯�������������
			DynaArray2D<int> emptyInt1(0, 2);//Ĭ�Ϲ��캯�������������
			DynaArray2D<int> emptyInt2(2, 0);
			emptyInt1 = 10;
			emptyInt2 = 10;
			emptyInt2.reSizeArray(3, 0);
			emptyInt1 = emptyInt2;

			DynaArray2D<int> arrayInt1(2, 2);//���������С

			std::cout << "1������1Ĭ��ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;//��ӡĬ�Ϲ���ֵ
			logPtr->LogByOstream() << "1������1Ĭ��ֵ" << std::endl;
			logPtr->LogByOstream() << arrayInt1;//��ӡĬ�Ϲ���ֵ

			arrayInt1.zeroArray();//��ֵ����
			std::cout << "2������1����ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;//��ӡ����

			arrayInt1 = 2;//���帳ֵ
			arrayInt1[1][0] = 3;
			std::cout << "3������1�޸�ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;

			DynaArray2D<int> arrayInt2(arrayInt1);//�������캯��
			std::cout << "4������2ʹ������1��������ֵ" << std::endl;
			std::cout << arrayInt2 << std::endl;

			arrayInt2.reSizeArray(3, 4);

			arrayInt2 = 5;
			std::cout << "5������2ʹ�ø�ֵ����������ڳ���" << std::endl;
			std::cout << arrayInt2 << std::endl;

			arrayInt1 = arrayInt2;
			std::cout << "6������1ʹ�ø�ֵ���������������2" << std::endl;
			std::cout << arrayInt1 << std::endl;

			arrayInt1 = arrayInt1;
			std::cout << "7������1ʹ�ø�ֵ��������Ը�ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;

			DynaArray2D<int> arrayInt3(std::move(arrayInt1));
			std::cout << "8������3ʹ���ƶ����캯��" << std::endl;
			std::cout << arrayInt3 << std::endl;

			arrayInt3.reSizeArray(5, 6);
			arrayInt3 = 8;
			std::cout << "9����������2��3" << std::endl;
			std::cout << "����ǰ2��3��" << std::endl;
			std::cout << arrayInt2 << std::endl;
			std::cout << arrayInt3 << std::endl;
			std::swap(arrayInt3, arrayInt2);
			std::cout << "������2��3��" << std::endl;
			std::cout << arrayInt2 << std::endl;
			std::cout << arrayInt3 << std::endl;

			std::cout << "10��3�����ƶ���ֵ�����" << std::endl;
			arrayInt3 = std::move(arrayInt3);
			std::cout << arrayInt3 << std::endl;
		}
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}

void testCaseDynamicArrayIntEx()
{
	//����Int����
	std::cout << "test int begin!" << std::endl;
	try
	{
		int count = 0;
		while (count++ < 10000)
		{
			LoggerPtr logPtr = Logger::getLogger();
			logPtr->LogForce("[%s][%d] log force test!", __STRING_FUNCTION__, __LINE__);
			logPtr->LogForce("");
			DynaArray2DEx<int> emptyInt;//Ĭ�Ϲ��캯�������������
			DynaArray2DEx<int> emptyInt1(0, 2);//Ĭ�Ϲ��캯�������������
			DynaArray2DEx<int> emptyInt2(2, 0);
			emptyInt1 = 10;
			emptyInt2 = 10;
			emptyInt2.reSizeArray(3, 0);
			emptyInt1 = emptyInt2;

			DynaArray2DEx<int> arrayInt1(2, 2);//���������С

			std::cout << "1������1Ĭ��ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;//��ӡĬ�Ϲ���ֵ
			logPtr->LogByOstream() << "1������1Ĭ��ֵ" << std::endl;
			logPtr->LogByOstream() << arrayInt1;//��ӡĬ�Ϲ���ֵ

			arrayInt1.zeroArray();//��ֵ����
			std::cout << "2������1����ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;//��ӡ����

			arrayInt1 = 2;//���帳ֵ
			arrayInt1[1][0] = 3;
			std::cout << "3������1�޸�ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;

			DynaArray2DEx<int> arrayInt2(arrayInt1);//�������캯��
			std::cout << "4������2ʹ������1��������ֵ" << std::endl;
			std::cout << arrayInt2 << std::endl;

			arrayInt2.reSizeArray(3, 4);

			arrayInt2 = 5;
			std::cout << "5������2ʹ�ø�ֵ����������ڳ���" << std::endl;
			std::cout << arrayInt2 << std::endl;

			arrayInt1 = arrayInt2;
			std::cout << "6������1ʹ�ø�ֵ���������������2" << std::endl;
			std::cout << arrayInt1 << std::endl;

			arrayInt1 = arrayInt1;
			std::cout << "7������1ʹ�ø�ֵ��������Ը�ֵ" << std::endl;
			std::cout << arrayInt1 << std::endl;

			DynaArray2DEx<int> arrayInt3(std::move(arrayInt1));
			std::cout << "8������3ʹ���ƶ����캯��" << std::endl;
			std::cout << arrayInt3 << std::endl;

			arrayInt3.reSizeArray(5, 6);
			arrayInt3 = 8;
			std::cout << "9����������2��3" << std::endl;
			std::cout << "����ǰ2��3��" << std::endl;
			std::cout << arrayInt2 << std::endl;
			std::cout << arrayInt3 << std::endl;
			std::swap(arrayInt3, arrayInt2);
			std::cout << "������2��3��" << std::endl;
			std::cout << arrayInt2 << std::endl;
			std::cout << arrayInt3 << std::endl;

			std::cout << "10��3�����ƶ���ֵ�����" << std::endl;
			arrayInt3 = std::move(arrayInt3);
			std::cout << arrayInt3 << std::endl;
		}
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
	}
}

void testCaseDataBaseService() 
{
	//DBService inst(DATA_SOURCE_ACCESS);
	//inst.getDBType();
	//std::cout << inst.getDBType() << std::endl;
	//std::cout << "hello��" << std::endl;

	//std::shared_ptr<DBService> g_pDBService = std::make_shared<DBService>(DATA_SOURCE_ACCESS);

	//DataBaseInfo dbInfo;

	//TableInfo tbInfo;
	//tbInfo = dbInfo["test"];

}




DynaArray2D<int> sum1(DynaArray2D<int> d1)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	d1 = 2;
	return d1;
}

int sum2(int a, int b, int c)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	return a + b + c;
}
void testCaseThreadPool() 
{
	ThreadPool pool;
	pool.start();
	DynaArray2D<int> dArr(2, 3);
	dArr = 6;
	std::cout << "dArr init value is " << dArr << std::endl;

	std::future<DynaArray2D<int>> res1 = pool.submit(sum1,dArr);

	std::future<int> res2 = pool.submit(sum2, 1, 2, 3);

	DynaArray2D<int> rt = res1.get();
	Sleep(2000);
	std::cout << "1 value" << rt << std::endl;

	std::cout << res2.get() << std::endl;
}