/*
 * logger_mgt.h
 *
 *  Created on: 2013年12月14日
 *      Author: jimm
 */

#ifndef LOGGER_MGT_H_
#define LOGGER_MGT_H_

#include "logger_namespace.h"
#include "logger.h"
#include "../common/common_typedef.h"
#include "../common/common_singleton.h"

#include <map>
#include <string>
using namespace std;

LOGGER_NAMESPACE_BEGIN

typedef map<string, CLogger *>		LoggerMap;

class CLoggerMgt
{
public:
	void RegistLogger(const char *strLoggerName, CLogger *pLogger);

	CLogger *GetLogger(const char *strLoggerName);

	LoggerMap::iterator GetBeginIterator();

	LoggerMap::iterator GetEndIterator();

private:
	LoggerMap				m_stLoggerMgt;
};

#define g_LoggerMgt			CSingleton<LOGGER::CLoggerMgt>::GetInstance()

LOGGER_NAMESPACE_END

#endif /* LOGGER_MGT_H_ */
