/*
 * logger.h
 *
 *  Created on: 2014年4月13日
 *      Author: Administrator
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "../common/common_export.h"
#include "../common/common_typedef.h"
#include "logger_namespace.h"

LOGGER_NAMESPACE_BEGIN

class CLoggerWriter;

class CLogger
{
public:
	EXPORT CLogger();
	EXPORT virtual ~CLogger();

	EXPORT static void RegistLogger(const char *strLoggerName, CLoggerWriter *pLogger);

	EXPORT static CLoggerWriter *GetLogger(const char *strLoggerName);

	EXPORT static int32_t Start();

	EXPORT static int32_t Stop();

	EXPORT static void Debug(const char *strLoggerName, const char *szFile, int32_t nLineNo, const char* szFormat, ...);

	EXPORT static void Info(const char *strLoggerName, const char *szFile, int32_t nLineNo, const char* szFormat, ...);

	EXPORT static void Warn(const char *strLoggerName, const char *szFile, int32_t nLineNo, const char* szFormat, ...);

	EXPORT static void Error(const char *strLoggerName, const char *szFile, int32_t nLineNo, const char* szFormat, ...);

protected:
	static bool		m_bStartddd;
};

#define WRITE_DEBUG_LOG(LOGGERNAME, FORMAT, ...)		CLogger::Debug(LOGGERNAME, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)

#define WRITE_INFO_LOG(LOGGERNAME, FORMAT, ...)			CLogger::Info(LOGGERNAME, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)

#define WRITE_WARN_LOG(LOGGERNAME, FORMAT, ...)			CLogger::Warn(LOGGERNAME, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)

#define WRITE_ERROR_LOG(LOGGERNAME, FORMAT, ...)		CLogger::Error(LOGGERNAME, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)

LOGGER_NAMESPACE_END

#endif /* LOGGER_H_ */
