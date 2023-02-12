#pragma once
#include <fstream>
#include <mutex>    //��׼���߳���ͷ�ļ�

#ifndef LOGGER_API
#define LOGGER_API __declspec(dllexport)

#ifndef MAX_LOG_BUF_LEN
#define  MAX_LOG_BUF_LEN 4096 //һ��д����������ֽ���
#endif

#ifndef NORNAL_LOG_BUF_LEN
#define  NORNAL_LOG_BUF_LEN 1024 //���ڲ������ַ�����С
#endif

#ifdef WIN32
#define __STRING_FUNCTION__ __FUNCTION__
#endif

#define LOGFORCE(format, ...) \
    Logger::getLogger()->LogForce(format, ##__VA_ARGS__)

#define LOGFATAL(format, ...) \
    Logger::getLogger()->LogFatal(format, ##__VA_ARGS__)

#define LOGERROR(format, ...) \
    Logger::getLogger()->LogError(format, ##__VA_ARGS__)

#define LOGWARN(format, ...) \
    Logger::getLogger()->LogWarn(format, ##__VA_ARGS__)

#define LOGINFO(format, ...) \
    Logger::getLogger()->LogInfo(format, ##__VA_ARGS__)

#define LOGDEBUG(format, ...) \
    Logger::getLogger()->LogDebug(format, ##__VA_ARGS__)

//ֱ��ʹ���ļ������
#define LOGFORCE_BYSTREAM(logInfo)\
{\
	std::lock_guard<std::mutex> lockLog(Logger::getLogger()->getMutex());\
	if (Logger::getLogger()->getLevel() >= Logger::EnumLogLevel::LOG_FORCE)\
		Logger::getLogger()->LogByOstream("O") << logInfo <<std::endl;\
}

#define LOGFATAL_BYSTREAM(logInfo)\
{\
	std::lock_guard<std::mutex> lockLog(Logger::getLogger()->getMutex());\
	if (Logger::getLogger()->getLevel() >= Logger::EnumLogLevel::LOG_FATAL)\
		Logger::getLogger()->LogByOstream("F") << logInfo <<std::endl;\
}

#define LOGERROR_BYSTREAM(logInfo)\
{\
	std::lock_guard<std::mutex> lockLog(Logger::getLogger()->getMutex());\
	if (Logger::getLogger()->getLevel() >= Logger::EnumLogLevel::LOG_ERROR)\
	Logger::getLogger()->LogByOstream("E") << logInfo <<std::endl;\
}

#define LOGWARN_BYSTREAM(logInfo)\
{\
	std::lock_guard<std::mutex> lockLog(Logger::getLogger()->getMutex());\
	if (Logger::getLogger()->getLevel() >= Logger::EnumLogLevel::LOG_WARN)\
	Logger::getLogger()->LogByOstream("W") << logInfo <<std::endl;\
}

#define LOGINFO_BYSTREAM(logInfo)\
{\
	std::lock_guard<std::mutex> lockLog(Logger::getLogger()->getMutex());\
	if (Logger::getLogger()->getLevel() >= Logger::EnumLogLevel::LOG_INFO)\
	Logger::getLogger()->LogByOstream("I") << logInfo <<std::endl;\
}

#define LOGDEBUG_BYSTREAM(logInfo)\
{\
	std::lock_guard<std::mutex> lockLog(Logger::getLogger()->getMutex());\
	if (Logger::getLogger()->getLevel() >= Logger::EnumLogLevel::LOG_DEBUG)\
		Logger::getLogger()->LogByOstream("D") << logInfo <<std::endl;\
}

class LOGGER_API Logger
{
private:
	Logger();
	~Logger();

public:
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

public:
	enum class EnumLogLevel
	{
		LOG_OFF = -1,
		LOG_FORCE = 0,
		LOG_FATAL,
		LOG_ERROR,
		LOG_WARN,
		LOG_INFO,
		LOG_DEBUG
	};
public:
	static Logger* getLogger();
	
	void LogForce(const char *format, ...);
	void LogFatal(const char *format, ...);
	void LogError(const char *format, ...);
	void LogWarn(const char *format, ...);
	void LogInfo(const char *format, ...);
	void LogDebug(const char *format, ...);

	std::ofstream &LogByOstream(const char* logTag = "");
	std::mutex& getMutex();
	void setLevel(const EnumLogLevel& enmLevel);
	EnumLogLevel getLevel();

private:
	EnumLogLevel m_LogLevel;

private:
	std::string m_strFilePath;	//��־�ļ�·��
	std::string m_strLogName;	//��־�ļ�����
	std::string m_strLogFile;	//����ļ�ȫ·��

	uint64_t m_nFileMaxSize;	//д�����ļ���С
	uint32_t m_nFileMaxNum;//��¼��־����
private:
	std::ofstream m_logOut;   //д�ļ���
	char m_szBuffer[NORNAL_LOG_BUF_LEN];  //��ʽ���ַ�������

private:
	std::mutex m_mutLog;//��־�ļ��޸��߳���
	static Logger* m_LogerInstance;

private:
	bool initLogger();
	bool LogCheck(const EnumLogLevel& enmLevel);
};

typedef Logger* LoggerPtr;
#endif
