#ifndef _RGE_LOG_H_
#define _RGE_LOG_H_

/* º¯ÊıÉùÃ÷ */
void log_init  (char *file);
void log_done  (void);
int  log_printf(char *format, ...);

#endif
