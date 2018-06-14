#ifndef __RGE_LOG_H__
#define __RGE_LOG_H__

/* º¯ÊıÉùÃ÷ */
void log_init  (char *file);
void log_done  (void);
int  log_printf(char *format, ...);

#endif
