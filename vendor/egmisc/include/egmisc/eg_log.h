#pragma once

#define EG_ENABLE_LOG
#ifdef EG_ENABLE_LOG
void eg_log_(char const *fmt, ...);
#define eg_log(...) eg_log_(__VA_ARGS__)
#else
#define eg_log(...)
#endif

