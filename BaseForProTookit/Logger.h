#pragma once
#include <fstream>
#include <mutex>    //标准库线程锁头文件

#ifndef LOGGER_API
#define LOGGER_API __declspec(dllexport)

#ifndef MAX_LOG_BUF_LEN
#define  MAX_LOG_BUF_LEN 4096 //一次写入数据最大字节数
#endif

#ifndef NORNAL_LOG_BUF_LEN
#define  NORNAL_LOG_BUF_LEN 1024 //类内部分配字符串大小
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

//直接使用文件流输出
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
	std::string m_strFilePath;	//日志文件路径
	std::string m_strLogName;	//日志文件名称
	std::string m_strLogFile;	//输出文件全路径

	uint64_t m_nFileMaxSize;	//写单个文件大小
	uint32_t m_nFileMaxNum;//记录日志数量
private:
	std::ofstream m_logOut;   //写文件流
	char m_szBuffer[NORNAL_LOG_BUF_LEN];  //格式化字符串内容

private:
	std::mutex m_mutLog;//日志文件修改线程锁
	static Logger* m_LogerInstance;

private:
	bool initLogger();
	bool LogCheck(const EnumLogLevel& enmLevel);
};

typedef Logger* LoggerPtr;
#endif
