/*
 * logger.cpp
 *
 *  Created on: 2014年4月13日
 *      Author: Administrator
 */

#include "logger.h"
#include "logger_writer.h"
#include "logger_mgt.h"
#include "logger_thread.h"

#include <stdarg.h>
#include <stddef.h>

LOGGER_NAMESPACE_BEGIN

bool CLogger::m_bStart = false;

CLogger::CLogger()
{
}

CLogger::~CLogger()
{

}

int32_t CLogger::SetLogDir(const char *szLogDir)
{
	return g_LoggerThread.SetLogDir(szLogDir);
}

void CLogger::RegistLogger(const char *strLoggerName, CLoggerWriter *pLogger)
{
	g_LoggerMgt.RegistLogger(strLoggerName, pLogger);
}

CLoggerWriter *CLogger::GetLogger(const char *strLoggerName)
{
	return g_LoggerMgt.GetLogger(strLoggerName);
}

int32_t CLogger::Start()
{
	if(m_bStart)
	{
		return true;
	}
	m_bStart = true;
	return g_LoggerThread.Start();
}

int32_t CLogger::Stop()
{
	m_bStart = false;
	return g_LoggerThread.Terminate();
}

void CLogger::Debug(const char *strLoggerName, const char *szFile, int32_t nLineNo, const char* szFormat, ...)
{
	CLoggerWriter *pLoggerWriter = GetLogger(strLoggerName);
	va_list vaList;
	va_start(vaList, szFormat);
	pLoggerWriter->Debug(szFormat, szFile, nLineNo, vaList);
	va_end(vaList);
}

void CLogger::Info(const char *strLoggerName, const char *szFile, int32_t nLineNo, const char* szFormat, ...)
{
	CLoggerWriter *pLoggerWriter = GetLogger(strLoggerName);
	va_list vaList;
	va_start(vaList, szFormat);
	pLoggerWriter->Info(szFormat, szFile, nLineNo, vaList);
	va_end(vaList);
}

void CLogger::Warn(const char *strLoggerName, const char *szFile, int32_t nLineNo, const char* szFormat, ...)
{
	CLoggerWriter *pLoggerWriter = GetLogger(strLoggerName);
	va_list vaList;
	va_start(vaList, szFormat);
	pLoggerWriter->Info(szFormat, szFile, nLineNo, vaList);
	va_end(vaList);
}

void CLogger::Error(const char *strLoggerName, const char *szFile, int32_t nLineNo, const char* szFormat, ...)
{
	CLoggerWriter *pLoggerWriter = GetLogger(strLoggerName);
	va_list vaList;
	va_start(vaList, szFormat);
	pLoggerWriter->Info(szFormat, szFile, nLineNo, vaList);
	va_end(vaList);
}

LOGGER_NAMESPACE_END

