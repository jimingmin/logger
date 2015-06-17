#ifndef LOGGER_CAPI_H_
#define LOGGER_CAPI_H_

#include "logger.h"

#ifdef __cplusplus
extern "C"
{
#endif

EXPORT int32_t start_log_thread();

EXPORT int32_t stop_log_thread();

EXPORT int32_t set_log_dir(const char *szLogDir);

EXPORT void write_debug_log(const char *szFileName, int32_t nLineNo, const char *szLoggerName, const char *szFormat, ...);

#ifdef __cplusplus
}
#endif

#endif /* LOGGER_CAPI_H_ */