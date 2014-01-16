/* 包含头文件 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "log.h"

static FILE *s_log_fp = NULL;

/* 函数实现 */
void log_init(char *file)
{
    if (!s_log_fp) {
        s_log_fp = fopen(file, "w");
    }
}

void log_done(void)
{
    if (s_log_fp) {
        fflush(s_log_fp);
        fclose(s_log_fp);
        s_log_fp = NULL;
    }
}

#define MAX_LOG_BUF 1024
int log_printf(char *format, ...)
{
    char buf[MAX_LOG_BUF];
    int  retv = 0;
    va_list valist;

    if (s_log_fp) {
        va_start(valist, format);
        vsnprintf(buf, MAX_LOG_BUF, format, valist);
        fprintf(s_log_fp, "%s", buf);
        fflush (s_log_fp);
        va_end(valist);
    }
    return retv;
}
