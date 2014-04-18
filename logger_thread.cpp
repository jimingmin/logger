/*
 * logger_thread.cpp
 *
 *  Created on: 2013年12月14日
 *      Author: jimm
 */

#include "logger_thread.h"
#include "logger_mgt.h"
#include "../common/common_api.h"

LOGGER_NAMESPACE_BEGIN

void CLoggerThread::Execute()
{
	bool bIdle = true;

	while ((!GetTerminated()) || (!bIdle))
	{
		bIdle = true;
		LoggerMap::iterator it = g_LoggerMgt.GetBeginIterator();
		for(; it != g_LoggerMgt.GetEndIterator(); ++it)
		{
			CLoggerWriter *pLogger = it->second;
			if(pLogger->Serialize() > 0)
			{
				bIdle = false;
			}
		}

		//所有队列中都没有日志
		if (bIdle)
		{
			Delay(10000);
		}
	}
}

LOGGER_NAMESPACE_END
