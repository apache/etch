#include <stdlib.h>
#include <stdarg.h>
#include <apr_time.h>

#include "etchlog.h"

char * etchlog_level_str[] = 
{
    "D",
    "I",
    "W",
    "E"
};

void etchlog_report(const char *cat_ptr, etchlog_level level, const char *fmt_ptr, ...)
{
    /* for now, we will prepand time for each message. in the future, we should add formatter support. */
    va_list args;
    int len;
    char *buf_ptr;
    apr_time_exp_t now;
    size_t size;
    size_t preamble_len;

    char preamble[128];

    if (level < ETCHLOG_DEBUG)
        level = ETCHLOG_DEBUG;
    if (level > ETCHLOG_ERROR)
        level = ETCHLOG_ERROR;

    /* preamble */
    apr_time_exp_lt(&now, apr_time_now());
    apr_strftime(preamble, &size, sizeof(preamble), "%m%d%Y %H:%M:%S", &now);

    va_start(args, fmt_ptr);
    len = _vscprintf(fmt_ptr, args) + 1;
    if (len < 0)    //error
    {
        printf("etchlog_report._vscprintf failed. There are error(s) in format string and its arguement list.");
        return;
    }

    //now the preamble len
    preamble_len = strlen(preamble) + 1 /*space*/ + strlen(etchlog_level_str[level]) + 1 + strlen(cat_ptr) + 1;

    buf_ptr = malloc( (len+preamble_len) * sizeof(char) );
    if (buf_ptr == NULL)
    {
        printf("etchlog_report.malloc( %d ) failed.", (len+preamble_len) * sizeof(char));
        return;
    }

    sprintf(buf_ptr, "%s %s\t%s\t", preamble, etchlog_level_str[level], cat_ptr);
    vsprintf(buf_ptr+preamble_len, fmt_ptr, args);
    va_end(fmt_ptr);
    
    /* dump to stdout for now */
    printf("%s\r\n", buf_ptr); 

    free(buf_ptr);
}