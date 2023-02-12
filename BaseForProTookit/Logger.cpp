#include "stdafx.h"
#include "Logger.h"
#include <iostream>


#include <fstream>  //读写文件流
#include <stdarg.h> //构造format文件
#include <string.h>
#include <string>

#include <corecrt_io.h>	//_access头文件
#include <iomanip> //std::setfill头文件


#ifndef LOG_FILE_SIZE
#define LOG_FILE_SIZE  1024 * 1024 //默认log文件大小
#endif

#ifndef LOG_FILE_NUM
#define LOG_FILE_NUM 15 //默认log文件数量
#endif

Logger* Logger::m_LogerInstance = nullptr;

#define LOG_BY_LEVEL(logLevel,logTag)\
if (nullptr == format)\
{\
	return;\
}\
std::lock_guard<std::mutex> lockLog(m_mutLog);/*线程锁*/\
if (!LogCheck(logLevel))\
{\
	return;\
}\
va_list arglist;\
va_start(arglist, format);\
uint32_t nstrLen = vsnprintf(m_szBuffer, NORNAL_LOG_BUF_LEN, format, arglist);\
va_end(arglist);\
char* szBufPtr = nullptr;\
bool bIsTooLong = false;/*字符串长度是否超过最大值*/\
if (nstrLen > NORNAL_LOG_BUF_LEN)\
{\
	nstrLen = nstrLen > MAX_LOG_BUF_LEN ? MAX_LOG_BUF_LEN : nstrLen;/*最大不超过MAX_LOG_BUF_LEN长度*/\
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
SYSTEMTIME st;/*打印时间*/\
GetLocalTime(&st);\
m_logOut <<"["<< GetCurrentProcessId()<<"-"<< GetCurrentThreadId() <<"]"/*进程ID-线程ID*/\
<< std::setfill('0') << std::setw(4) << st.wYear << "-"/*年*/\
<< std::setfill('0') << std::setw(2) << st.wMonth << "-"/*月*/\
<< std::setfill('0') << std::setw(2) << st.wDay << " "/*日*/\
<< std::setfill('0') << std::setw(2) << st.wHour << ":"/*时*/\
<< std::setfill('0') << std::setw(2) << st.wMinute << ":"/*分*/\
<< std::setfill('0') << std::setw(2) << st.wSecond << ":"/*秒*/\
<< std::setfill('0') << std::setw(3) << st.wMilliseconds << " "/*毫秒*/\
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
	//调用时候在外面锁
	//std::lock_guard<std::mutex> lockLog(m_mutLog);//线程锁
	SYSTEMTIME st;//打印时间
	GetLocalTime(&st);
	m_logOut <<"["<< GetCurrentProcessId()<<"-"<< GetCurrentThreadId() <<"]"//进程ID-线程ID
		<< std::setfill('0') << std::setw(4) << st.wYear << "-"//年
		<< std::setfill('0') << std::setw(2) << st.wMonth << "-"//月
		<< std::setfill('0') << std::setw(2) << st.wDay << " "//日
		<< std::setfill('0') << std::setw(2) << st.wHour << ":"//时
		<< std::setfill('0') << std::setw(2) << st.wMinute << ":"//分
		<< std::setfill('0') << std::setw(2) << st.wSecond << ":"//秒
		<< std::setfill('0') << std::setw(3) << st.wMilliseconds << " "//毫秒
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
	//获取进程路径及名称
	int nlen = GetModuleFileName(NULL, path, MAX_STR_LEN);
	while (path[nlen - 1] != '\\')
	{
		nlen--; 
	}
	//获取进程名称
	m_strLogFile = &path[nlen];
	int pos = m_strLogFile.find_last_of(".");
	if (-1 != pos)
	{
		m_strLogFile.erase(pos);
	}
	path[nlen - 1] = 0; 

	m_strFilePath = path;
	m_strFilePath += "\\log";//日志路径文件夹
	m_strLogFile = m_strFilePath + "\\"+ m_strLogFile +".log";//文件全路径+文件名称

	if (_access(m_strFilePath.c_str(), 0) == -1)//如果数据库文件夹不存在，则创建
	{
		BOOL NewFile1 = CreateDirectory(m_strFilePath.c_str(), 0);//则创建
		if (NewFile1 == 0)
		{
			m_LogLevel = EnumLogLevel::LOG_OFF;//创建文件夹失败，不输出日志
			std::cout << "Logger Init failed ! can not creat directory:" << m_strFilePath.c_str() << std::endl;
			return false;
		}
	}

	m_LogLevel = EnumLogLevel::LOG_DEBUG;
	m_nFileMaxSize = LOG_FILE_SIZE;      //写单个文件大小
	//m_nFileMaxSize = 200;
	m_nFileMaxNum = LOG_FILE_NUM;//记录日志数量

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
	if (!m_logOut.is_open()|| !m_logOut.seekp(0, std::ios::end))//如果没有打开
	{
		m_LogLevel = EnumLogLevel::LOG_OFF;
		std::cout << "log file open failed!"<< std::endl;
		return false;
	}

	if (m_logOut.tellp() > m_nFileMaxSize)//文件大于最大文件大小
	{
		//修改文件名称
		m_logOut.close();

		std::string strOldName; //旧名称
		std::string strNewName; //新名称
		int32_t nLogNumIndex = m_nFileMaxNum;
		strNewName = m_strLogFile + "." + std::to_string(nLogNumIndex); //最大序号的日志
		if (_access(strNewName.c_str(), 0) != -1)                                                 //检查文件夹是否存在，不存在则创建
		{
			remove(strNewName.c_str()); //删除
		}
		while (--nLogNumIndex > 0) //减小序号，以此更改日志名称
		{
			strOldName = m_strLogFile + "." + std::to_string(nLogNumIndex);
			if (_access(strOldName.c_str(), 0) != -1) //检查文件夹是否存在，不存在则创建
			{
				rename(strOldName.c_str(), strNewName.c_str()); //删除
			}
			strNewName = m_strLogFile + "." + std::to_string(nLogNumIndex);
		}
		rename(m_strLogFile.c_str(), strNewName.c_str()); //重命名
		m_logOut.open(m_strLogFile, std::fstream::app);
	}
	return true;
}