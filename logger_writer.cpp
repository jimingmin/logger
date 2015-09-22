/*
 * logger.cpp
 *
 *  Created on: 2013年12月14日
 *      Author: jimm
 */

#include "logger_writer.h"
#include "../common/common_datetime.h"
#include <stdarg.h>
#include <stddef.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

LOGGER_NAMESPACE_BEGIN

CLoggerWriter::CLoggerWriter(const char *strLogName)
{
	m_stLogName = strLogName;
	m_nRollingSize = enmDefaultRollingSize;
}

CLoggerWriter::CLoggerWriter(const char *strLogName, int32_t nRollingSize)
{
	m_stLogName = strLogName;
	m_nRollingSize = nRollingSize;
}

CLoggerWriter::~CLoggerWriter()
{
}

int32_t CLoggerWriter::Debug(const char* szFormat, const char *szFile, int32_t nLineNo, va_list vaList)
{
	//获取日志信息
	return WriteToLogBuffer(enmLogLevel_Debug, szFormat, szFile, nLineNo, vaList);
}

int32_t CLoggerWriter::Info(const char* szFormat, const char *szFile, int32_t nLineNo, va_list vaList)
{
	//获取日志信息
	return WriteToLogBuffer(enmLogLevel_Info, szFormat, szFile, nLineNo, vaList);
}

int32_t CLoggerWriter::Warn(const char* szFormat, const char *szFile, int32_t nLineNo, va_list vaList)
{
	//获取日志信息
	return WriteToLogBuffer(enmLogLevel_Warning, szFormat, szFile, nLineNo, vaList);
}

int32_t CLoggerWriter::Error(const char* szFormat, const char *szFile, int32_t nLineNo, va_list vaList)
{
	//获取日志信息
	return WriteToLogBuffer(enmLogLevel_Error, szFormat, szFile, nLineNo, vaList);
}

int32_t CLoggerWriter::Serialize(const char *szLogDir)
{
	char szLog[enmMaxLogStringLength];
	szLog[0] = '\0';

	char szDate[enmMaxDateTimeLength];
	szDate[0] = '\0';

	bool bIdle = false;
	int32_t nLogSize = 0;
	if(m_stLoggerBuffer.Size() < sizeof(nLogSize))
	{
		bIdle = true;
	}
	else
	{
		m_stLoggerBuffer.PeekRead((uint8_t *)&nLogSize, sizeof(nLogSize));
		if(m_stLoggerBuffer.Size() < (int32_t)(nLogSize + enmDateStringLength + sizeof(nLogSize)))
		{
			bIdle = true;
		}
	}

	if(!bIdle)
	{
		int32_t nSize = 0;
		nSize += m_stLoggerBuffer.Read((uint8_t *)&nLogSize, sizeof(nLogSize));

		nSize += m_stLoggerBuffer.Read((uint8_t *)szDate, enmDateStringLength);
		szDate[enmDateStringLength] = '\0';//szDate是16字节的，这里只用了8字节，后面追加了1个'\0'

		nSize += m_stLoggerBuffer.Read((uint8_t *)szLog, nLogSize);

		if(nLogSize < enmMaxLogStringLength)
		{
			szLog[nLogSize] = '\0';
		}
		else
		{
			szLog[enmMaxLogStringLength - 1] = '\0';
		}
		WriteToLogFile(szLogDir, szDate, szLog);

		return nSize;
	}

	int64_t now_time = CTimeValue::CurrentTime().Milliseconds();
	for (uint32_t i = 0; i < enmMaxLogFileNameCount; ++i)
	{
		if (NULL == m_arrLogFileState[i].pFile)
		{
			continue;
		}

		//定期刷新
		if (now_time > m_arrLogFileState[i].nLastFlushTime + enmFlushPeriod)
		{
			fflush(m_arrLogFileState[i].pFile);
			m_arrLogFileState[i].nLastFlushTime = now_time;
			MoveLogFile(m_nRollingSize, m_arrLogFileState[i].szFileName, m_arrLogFileState[i].pFile);
		}

		//日志文件是否需要关闭
		if (now_time > m_arrLogFileState[i].nActiveTime + enmWriteLogPeriod)
		{
			fclose(m_arrLogFileState[i].pFile);
			m_arrLogFileState[i].pFile = NULL;
			MoveLogFile(m_nRollingSize, m_arrLogFileState[i].szFileName, m_arrLogFileState[i].pFile);
			memset(&m_arrLogFileState[i], 0, sizeof(LogFileState));
		}
	}

	return 0;
}

//写日志
int32_t CLoggerWriter::WriteToLogBuffer(LogLevel logLevel, const char* szFormat, const char *szFile, int32_t nLineNo, va_list vaList)
{
	DECLARE_LOGLEVELLIST;

	char szLog[enmMaxLogStringLength];
	szLog[0] = '\0';
	CDateTime dt = CDateTime::CurrentDateTime();

	char szDate[enmMaxDateTimeLength];
	szDate[0] = '\0';
	sprintf(szDate, "%04d%02d%02d", dt.Year(), dt.Month(), dt.Day());

	//获取日志信息
	uint32_t nHeadLen = 0;
	sprintf(szLog, "%04d-%02d-%02d %02d:%02d:%02d %s [%s][%d] ", dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second(), arrLogLevel[logLevel], szFile, nLineNo);
	nHeadLen = (uint32_t)strlen(szLog);
	int32_t nContentLen = vsprintf(szLog + nHeadLen, szFormat, vaList);
	if(nContentLen < 0)
	{
		return 0;
	}

	//先写入本条日志的总长度(不包括自己和enmDateStringLength字节时间字符串的长度)
	uint32_t nLogLen = nHeadLen + nContentLen;//strlen(szLog);
	m_stLoggerBuffer.Write((uint8_t *)&nLogLen, sizeof(nLogLen));
	//写入日期字符串
	m_stLoggerBuffer.Write((uint8_t *)szDate, enmDateStringLength);
	//写入日志内容
	m_stLoggerBuffer.Write((uint8_t *)szLog, nLogLen);

	return nLogLen + sizeof(nLogLen) + enmDateStringLength;
}

//将日志写入到文件
void CLoggerWriter::WriteToLogFile(const char *szLogDir, const char* szDate, const char* szLog)
{
	char szFileName[enmMaxFullPathLength] = { 0 };
	if((szLogDir == NULL) || (szLogDir[0] == '\0'))
	{
		sprintf(szFileName, "./log/%s%s_%u.log", m_stLogName.c_str(), szDate, getpid());
	}
	else
	{
		sprintf(szFileName, "%s/%s%s.log", szLogDir, m_stLogName.c_str(), szDate);
	}

	//从日志文件状态列表中查找文件
	bool bWritten = false;
	uint32_t nIndex = enmInvalidLogFileIndex;
	int64_t now_time = CTimeValue::CurrentTime().Milliseconds();
	for (uint32_t i = 0; i < enmMaxLogFileNameCount; ++i)
	{
		if (NULL == m_arrLogFileState[i].pFile)
		{
			//找到第一个空位序号
			if (enmInvalidLogFileIndex == nIndex)
			{
				nIndex = i;
			}
			continue;
		}
		//日志文件是否已经打开
		if (0 == strcmp(szFileName, m_arrLogFileState[i].szFileName))
		{
			fwrite(szLog, 1, strlen(szLog), m_arrLogFileState[i].pFile);
			bWritten = true;
			//写了文件后修改活跃时间
			m_arrLogFileState[i].nActiveTime = now_time;
		}
		else
		{
			//日志文件是否需要关闭
			if (now_time > m_arrLogFileState[i].nActiveTime + enmWriteLogPeriod)
			{
				fclose(m_arrLogFileState[i].pFile);
				m_arrLogFileState[i].pFile = NULL;
				MoveLogFile(m_nRollingSize, m_arrLogFileState[i].szFileName, m_arrLogFileState[i].pFile);
				memset(&m_arrLogFileState[i], 0, sizeof(LogFileState));
			}
		}
	}

	//日志文件未打开则打开日志文件
	if ((!bWritten) && (enmInvalidLogFileIndex != nIndex))
	{
		m_arrLogFileState[nIndex].pFile = fopen(szFileName, "a+");
		if (NULL != m_arrLogFileState[nIndex].pFile)
		{
			m_arrLogFileState[nIndex].nActiveTime = now_time;
			m_arrLogFileState[nIndex].nLastFlushTime = now_time;
			uint32_t len = (uint32_t)strlen(szFileName);
			if(enmMaxFullPathLength < len )
			{
				len = enmMaxFullPathLength;
			}
			strncpy(m_arrLogFileState[nIndex].szFileName, szFileName, len);
			fwrite(szLog, 1, strlen(szLog), m_arrLogFileState[nIndex].pFile);
		}
	}
}

void CLoggerWriter::MoveLogFile(int32_t max_file_size, const char* file_name, FILE*& pf)
{
#ifndef WIN32
	struct stat curr_stat = { 0 };

	int32_t ret = stat(file_name, &curr_stat);
	if (0 != ret)
	{
		return;
	}
	if (curr_stat.st_size < max_file_size)
	{
		return;
	}

	char new_name[enmMaxFullPathLength] = { 0 };

	//获取合适的文件扩展名
	int32_t num = 1;
	for (num = 1; num < enmMaxLogFileNameCount; ++num)
	{
		sprintf(new_name, "%s.%d", file_name, num);
		if (access(new_name, F_OK) < 0)
		{
			if(errno == ENOENT)
			{
				break;
			}
		}
	}

	//日志文件数量达到上线
	if (num == enmMaxLogFileNameCount)
	{
		return;
	}

	//关闭当前文件
	if (NULL != pf)
	{
		fclose(pf);
	}

	//更改当前文件名
	ret = rename(file_name, new_name);
	if (0 == ret)
	{
		//打开新文件
		if (NULL != pf)
		{
			pf = fopen(file_name, "a+");
		}
	}
	else
	{
		pf = NULL;
	}
#endif
}

LOGGER_NAMESPACE_END

