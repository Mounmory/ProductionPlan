#pragma once
#include "stdafx.h"

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include "atlstr.h"
#include <memory>
#include "DynaArray.h"


//�������ݿ��������
enum class EnumDBCreatType
{
	CREAT_UNKNOW = 0,		//δ֪���ݿ�����
	CREAT_DB = 1,			//���������ݿ�
	CREAT_TABLE	= 2,		//���������
	CREAT_DB_AND_TABLE = 3	//�������ݿ⼰���
};
//��¼���򿪷�ʽ
enum class RecordSetOpenType
{
	AD_OPEN_DANAMIC = 0,
	AD_OPEN_STATIC = 1,
	AD_OPEN_KEYSET = 2
};

//�������ʱ������
#define COLUMN_TYPE_BIT			"BIT"			//����
#define COLUMN_TYPE_INTEGER		"INTEGER"		//����
#define COLUMN_TYPE_REAL		"REAL"			//ʵ��
#define COLUMN_TYPE_TEXT		"TEXT"			//�ı�
#define COLUMN_TYPE_DATE		"DATE"		//����
#define COLUMN_TYPE_TIME		"TIME"			//ʱ��
#define COLUMN_TYPE_DATETIME	"DATETIME"		//ʱ��

//#ifdef USING_STD_STRING
//#define STRING std::string 
//#else
//#define STRING CString 
//#endif // WIN_CSTRING

////�ڶ��ַ���
//#ifdef USING_STD_STRING
//typedef std::string STRING;
//#else
//typedef CString	STRING;
//#endif

////�����ַ���
//#ifdef USING_STD_STRING
//using STRING = std::string;
//#else
//using STRING = CString;
//#endif

typedef CString	STRING;
//��¼��Ϣ
typedef struct ADOLogInfo
{
	STRING strUserID ;//���û���
	STRING strPassword ;//�û�����
	STRING	strDBName ;//���ݿ�����
	//STRING	strServerIP;//������IP
	STRING strDataSource ;//����Դ��Ϣ
}DBLOGINFO,*PDBLOGINFO;

//����д���ݿ����Ϣ
typedef struct ValueInfo
{
	//�����ݿ���Ϣ
	STRING  strInReadSQL;//SQL���
	std::vector<STRING> szInReadColumnName;//�洢��ȡ��������
	DynaArray2D<STRING> szOutReadValue;//��ȡ��Ϣ

	//д���ݿ���Ϣ
	STRING  strInWriteSQL;//SQL���
	std::vector<STRING> szInWtriteColuName;//Ҫд����
	DynaArray2D<STRING> szInWriteValue;

	//�������ݿ���Ϣ
	STRING  strUpdateSQL;//SQL���
	std::vector<STRING> szUpdateColuName;//���µ���
	std::vector<STRING> szUpdateValue;//���µ�ֵ
	int nUpdateColumnNum = 0;//�����е�����

	//ɾ�����ݿ�������
	//STRING  strInDeletSQL;
	STRING  strInDelTableName;

}VALUEINFO,*PVALUEINFO;

//�½���ѯ���ݿ���Ϣ
typedef struct DBOperInfo
{
	//�½����ݿ⼰��������Ϣ
	STRING strInDBname;	//���������ݿ�����
	STRING strInDBPath;	//���ݿ�·��
	STRING strInCreatTableSQL;		//���������
	EnumDBCreatType EnuCreatType = EnumDBCreatType::CREAT_UNKNOW;		//��������
	//ɾ�������Ϣ
	STRING strInDelTableName;

	//��ѯ�����
	std::vector<STRING> strOutQueryTableResult;//��ѯ���ݿ��б������

	//��ѯ����������ƽ��
	STRING strInQueryedColTableName;
	std::vector<STRING> strOutQueryColumnResult;//��ѯ�����������

	//�����Ϣ
	void ClearCreatInfo()
	{
		EnuCreatType = EnumDBCreatType::CREAT_UNKNOW;
	}
}DBOPERATEINFO,*PDBOPERATEINFO;

//���ݿ���Ϣ�ṹ��

typedef struct ADODatabaseInfo
{
	DBLOGINFO stuLogInfo;		//��¼��Ϣ
	VALUEINFO stuValueInfo;		//�������ݿ���Ϣ
	DBOPERATEINFO stuDBOperate;	//�½����ݿ���Ϣ

}COMMONDBINFO,ADODBINFO,*PADODBINFO,SQLITEDBINFO,*PSQLITEDBINFO;