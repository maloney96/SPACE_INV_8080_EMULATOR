#ifndef DEBUG_WRAPPER_H
#define DEBUG_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

void qdebug_log(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif // DEBUG_WRAPPER_H
