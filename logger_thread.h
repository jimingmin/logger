/*
 * logger_thread.h
 *
 *  Created on: 2013年12月14日
 *      Author: jimm
 */

#ifndef LOGGER_THREAD_H_
#define LOGGER_THREAD_H_

#include "logger_namespace.h"
#include "../common/common_thread.h"

LOGGER_NAMESPACE_BEGIN

class CLoggerThread : public CThread
{
public:
	//线程入口函数
	virtual void Execute();
};


LOGGER_NAMESPACE_END

#endif /* LOGGER_THREAD_H_ */
