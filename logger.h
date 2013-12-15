/*
 * logger.h
 *
 *  Created on: 2013年12月14日
 *      Author: jimm
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "logger_namespace.h"
#include "../common/common_typedef.h"
#include "../common/common_string.h"
#include "../common/common_cyclebuffer.h"

LOGGER_NAMESPACE_BEGIN

#define DECLARE_LOGLEVELLIST	\
	const char * arrLogLevel[] = {\
	"[Debug]   ",\
	"[Info]    ",\
	"[Warning] ",\
	"[Error]   "}

typedef uint8_t						LogLevel;			//日志级别
enum
{
	enmLogLevel_Debug				= 0x00,				//调试信息
	enmLogLevel_Info				= 0x01,				//一般日志信息
	enmLogLevel_Notice				= 0x02,				//通知
	enmLogLevel_Warning				= 0x03,				//警告
	enmLogLevel_Error				= 0x04,				//错误
};

enum
{
	enmMaxLogStringLength		= 256 * 1024,
	enmMaxDateTimeLength		= 16,
	enmDateStringLength			= 8,
	enmMaxLogFileNameLength		= 1024,				//日志文件名的最大长度
	enmDefaultRollingSize		= 100 * 1024 * 1024,//默认分卷大小
	enmDefaultBufSize			= 64 * 1024,		//默认缓存大小
	enmMaxLogFileNameCount		= 1000,			//日志文件名最大数量
	enmWriteLogPeriod			= 1000 * 60,	//日志文件的最长非活跃时间(ms),暂时设为1分钟
	enmFlushPeriod				= 1000,			//固定刷新周期，暂时设为1s
	enmInvalidLogFileIndex		= 0xFFFFFFFF,	//无效日志文件序号
	enmMaxFullPathLength		= 1024,			//全路径
};

//日志文件状态
class LogFileState
{
public:
	LogFileState()
	{
		pFile = NULL;
		nActiveTime = 0;
		nLastFlushTime = 0;
		memset(szFileName, 0, sizeof(szFileName));
	}

	FILE*				pFile;								//文件描述符
	int64_t				nActiveTime;						//文件上次活跃时间
	int64_t				nLastFlushTime;						//上次刷新时间
	char				szFileName[enmMaxFullPathLength];	//文件名
};

class CLogger
{
public:
	CLogger(const char *strLogName);

	CLogger(const char *strLogName, int32_t nRollingSize);

	virtual ~CLogger();

	int32_t Debug(const char* szFormat, ...);

	int32_t Info(const char* szFormat, ...);

	int32_t Warn(const char* szFormat, ...);

	int32_t Error(const char* szFormat, ...);

	int32_t Serialize();
protected:
	//将日志写入到文件
	void WriteToLogFile(const char* szDate, const char* szLog);
	//将日志超出限制处理
	void MoveLogFile(int32_t max_file_size, const char* file_name, FILE*& pf);
	virtual int32_t WriteToLogBuffer(LogLevel logLevel, const char* szFormat, va_list vaList);

protected:
	CString<enmMaxLogFileNameLength>	m_stLogName;			//日志文件名
	int32_t								m_nRollingSize;			//日志文件分卷大小
	CycleBuffer<enmDefaultBufSize>		m_stLoggerBuffer;		//日志缓存
	//各日志文件状态
	LogFileState						m_arrLogFileState[enmMaxLogFileNameCount];
};

LOGGER_NAMESPACE_END

#endif /* LOGGER_H_ */
