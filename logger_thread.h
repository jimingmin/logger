/*
 * logger_thread.h
 *
 *  Created on: 2013年12月14日
 *      Author: jimm
 */

#ifndef LOGGER_THREAD_H_
#define LOGGER_THREAD_H_

#include "logger_namespace.h"
#include "logger_writer.h"
#include "../common/common_singleton.h"
#include "../common/common_thread.h"

LOGGER_NAMESPACE_BEGIN

class CLoggerThread : public CThread
{
public:
	int32_t SetLogDir(const char *szLogDir);

	//线程入口函数
	virtual void Execute();

protected:
	char					m_szLogDir[enmMaxFullPathLength];
};

#define g_LoggerThread		CSingleton<CLoggerThread>::GetInstance()

LOGGER_NAMESPACE_END

#endif /* LOGGER_THREAD_H_ */
