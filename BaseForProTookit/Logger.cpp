#include "stdafx.h"
#include "Logger.h"
#include <iostream>


#include <fstream>  //��д�ļ���
#include <stdarg.h> //����format�ļ�
#include <string.h>
#include <string>

#include <corecrt_io.h>	//_accessͷ�ļ�
#include <iomanip> //std::setfillͷ�ļ�


#ifndef LOG_FILE_SIZE
#define LOG_FILE_SIZE  1024 * 1024 //Ĭ��log�ļ���С
#endif

#ifndef LOG_FILE_NUM
#define LOG_FILE_NUM 15 //Ĭ��log�ļ�����
#endif

Logger* Logger::m_LogerInstance = nullptr;

#define LOG_BY_LEVEL(logLevel,logTag)\
if (nullptr == format)\
{\
	return;\
}\
std::lock_guard<std::mutex> lockLog(m_mutLog);/*�߳���*/\
if (!LogCheck(logLevel))\
{\
	return;\
}\
va_list arglist;\
va_start(arglist, format);\
uint32_t nstrLen = vsnprintf(m_szBuffer, NORNAL_LOG_BUF_LEN, format, arglist);\
va_end(arglist);\
char* szBufPtr = nullptr;\
bool bIsTooLong = false;/*�ַ��������Ƿ񳬹����ֵ*/\
if (nstrLen > NORNAL_LOG_BUF_LEN)\
{\
	nstrLen = nstrLen > MAX_LOG_BUF_LEN ? MAX_LOG_BUF_LEN : nstrLen;/*��󲻳���MAX_LOG_BUF_LEN����*/\
	szBufPtr = new char[nstrLen + 1];\
	va_start(arglist, format);\
	vsnprintf(szBufPtr, nstrLen + 1, format, arglist);\
	va_end(arglist);\
	bIsTooLong = true;\
}\
else\
{\
	szBufPtr = m_szBuffer;\
}\
SYSTEMTIME st;/*��ӡʱ��*/\
GetLocalTime(&st);\
m_logOut <<"["<< GetCurrentProcessId()<<"-"<< GetCurrentThreadId() <<"]"/*����ID-�߳�ID*/\
<< std::setfill('0') << std::setw(4) << st.wYear << "-"/*��*/\
<< std::setfill('0') << std::setw(2) << st.wMonth << "-"/*��*/\
<< std::setfill('0') << std::setw(2) << st.wDay << " "/*��*/\
<< std::setfill('0') << std::setw(2) << st.wHour << ":"/*ʱ*/\
<< std::setfill('0') << std::setw(2) << st.wMinute << ":"/*��*/\
<< std::setfill('0') << std::setw(2) << st.wSecond << ":"/*��*/\
<< std::setfill('0') << std::setw(3) << st.wMilliseconds << " "/*����*/\
<< "["<< #logTag <<"]" << szBufPtr << std::endl;\
if (true == bIsTooLong)\
{\
	delete[] szBufPtr;\
}\

Logger::Logger()
{
	initLogger();
}

Logger::~Logger()
{
	if (m_logOut.is_open())
	{
		m_logOut.close();
	}
	if (m_LogerInstance != nullptr) 
	{
		delete m_LogerInstance;
	}
}

Logger* Logger::getLogger()
{
	if (m_LogerInstance == nullptr)
	{
		m_LogerInstance = new Logger;
	}
	return m_LogerInstance;
}

void Logger::LogForce(const char *format, ...)
{
	LOG_BY_LEVEL(EnumLogLevel::LOG_FORCE, O)
}

void Logger::LogFatal(const char *format, ...) 
{
	LOG_BY_LEVEL(EnumLogLevel::LOG_FATAL, F)
}

void Logger::LogError(const char *format, ...) 
{
	LOG_BY_LEVEL(EnumLogLevel::LOG_ERROR, E)
}

void Logger::LogWarn(const char *format, ...)
{
	LOG_BY_LEVEL(EnumLogLevel::LOG_ERROR, W)
}

void Logger::LogInfo(const char *format, ...) 
{
	LOG_BY_LEVEL(EnumLogLevel::LOG_INFO, I)
}

void Logger::LogDebug(const char *format, ...)
{
	LOG_BY_LEVEL(EnumLogLevel::LOG_DEBUG, D)
}

std::ofstream& Logger::LogByOstream(const char* logTag)
{
	//����ʱ����������
	//std::lock_guard<std::mutex> lockLog(m_mutLog);//�߳���
	SYSTEMTIME st;//��ӡʱ��
	GetLocalTime(&st);
	m_logOut <<"["<< GetCurrentProcessId()<<"-"<< GetCurrentThreadId() <<"]"//����ID-�߳�ID
		<< std::setfill('0') << std::setw(4) << st.wYear << "-"//��
		<< std::setfill('0') << std::setw(2) << st.wMonth << "-"//��
		<< std::setfill('0') << std::setw(2) << st.wDay << " "//��
		<< std::setfill('0') << std::setw(2) << st.wHour << ":"//ʱ
		<< std::setfill('0') << std::setw(2) << st.wMinute << ":"//��
		<< std::setfill('0') << std::setw(2) << st.wSecond << ":"//��
		<< std::setfill('0') << std::setw(3) << st.wMilliseconds << " "//����
		<< "[" << logTag << "]";
	return m_logOut;
}

std::mutex& Logger::getMutex() 
{
	return m_mutLog;
}

void Logger::setLevel(const EnumLogLevel & enmLevel)
{
	m_LogLevel = enmLevel;
}

Logger::EnumLogLevel Logger::getLevel()
{
	return m_LogLevel;
}

bool Logger::initLogger()
{
	char path[MAX_STR_LEN] = { 0 };
	//��ȡ����·��������
	int nlen = GetModuleFileName(NULL, path, MAX_STR_LEN);
	while (path[nlen - 1] != '\\')
	{
		nlen--; 
	}
	//��ȡ��������
	m_strLogFile = &path[nlen];
	int pos = m_strLogFile.find_last_of(".");
	if (-1 != pos)
	{
		m_strLogFile.erase(pos);
	}
	path[nlen - 1] = 0; 

	m_strFilePath = path;
	m_strFilePath += "\\log";//��־·���ļ���
	m_strLogFile = m_strFilePath + "\\"+ m_strLogFile +".log";//�ļ�ȫ·��+�ļ�����

	if (_access(m_strFilePath.c_str(), 0) == -1)//������ݿ��ļ��в����ڣ��򴴽�
	{
		BOOL NewFile1 = CreateDirectory(m_strFilePath.c_str(), 0);//�򴴽�
		if (NewFile1 == 0)
		{
			m_LogLevel = EnumLogLevel::LOG_OFF;//�����ļ���ʧ�ܣ��������־
			std::cout << "Logger Init failed ! can not creat directory:" << m_strFilePath.c_str() << std::endl;
			return false;
		}
	}

	m_LogLevel = EnumLogLevel::LOG_DEBUG;
	m_nFileMaxSize = LOG_FILE_SIZE;      //д�����ļ���С
	//m_nFileMaxSize = 200;
	m_nFileMaxNum = LOG_FILE_NUM;//��¼��־����

	m_logOut.open(m_strLogFile.c_str(), std::ios::app);
	if (m_logOut.fail())
	{
		throw std::logic_error("open log file failed: " + m_strLogFile);
	}
	m_logOut.seekp(0, std::ios::end);

	std::cout << "Logger Init success!" << std::endl;
	std::cout << "Log file full path :" << m_strLogFile.c_str() <<std::endl;
	std::cout << "Log file Max size :" << m_nFileMaxSize <<" byte("<< (float(m_nFileMaxSize) /1048576)<<"MB)"<< std::endl;
	std::cout << "Log file Max number :" << m_nFileMaxNum << std::endl;
	std::cout << "Log level :" << static_cast<int32_t>(m_LogLevel) << std::endl;
	LogForce("Logger Init success!");
	LogForce("Log file full path : %s", m_strLogFile.c_str());
	LogForce("Log file Max size : %lld byte(%gMB)", m_nFileMaxSize, (float(m_nFileMaxSize) / 1048576));
	LogForce("Log file Max number : %d", m_nFileMaxNum);
	LogForce("Log level :%d", (uint32_t)m_LogLevel);
	return true;
}

bool Logger::LogCheck(const EnumLogLevel& enmLevel)
{
	if (enmLevel > m_LogLevel)
	{
		return false;
	}
	if (!m_logOut.is_open()|| !m_logOut.seekp(0, std::ios::end))//���û�д�
	{
		m_LogLevel = EnumLogLevel::LOG_OFF;
		std::cout << "log file open failed!"<< std::endl;
		return false;
	}

	if (m_logOut.tellp() > m_nFileMaxSize)//�ļ���������ļ���С
	{
		//�޸��ļ�����
		m_logOut.close();

		std::string strOldName; //������
		std::string strNewName; //������
		int32_t nLogNumIndex = m_nFileMaxNum;
		strNewName = m_strLogFile + "." + std::to_string(nLogNumIndex); //�����ŵ���־
		if (_access(strNewName.c_str(), 0) != -1)                                                 //����ļ����Ƿ���ڣ��������򴴽�
		{
			remove(strNewName.c_str()); //ɾ��
		}
		while (--nLogNumIndex > 0) //��С��ţ��Դ˸�����־����
		{
			strOldName = m_strLogFile + "." + std::to_string(nLogNumIndex);
			if (_access(strOldName.c_str(), 0) != -1) //����ļ����Ƿ���ڣ��������򴴽�
			{
				rename(strOldName.c_str(), strNewName.c_str()); //ɾ��
			}
			strNewName = m_strLogFile + "." + std::to_string(nLogNumIndex);
		}
		rename(m_strLogFile.c_str(), strNewName.c_str()); //������
		m_logOut.open(m_strLogFile, std::fstream::app);
	}
	return true;
}