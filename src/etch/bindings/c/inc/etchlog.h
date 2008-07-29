/**
 * etchlog.h logger for c.
 *
 */
 
 #ifndef ETCHLOG_H
 #define ETCHLOG_H
 
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

 typedef enum etchlog_level 
 {
    ETCHLOG_DEBUG = 0,
    ETCHLOG_INFO,
    ETCHLOG_WARNING,
    ETCHLOG_ERROR
 } etchlog_level;
 
 /**
  * log a message to console/log file/system log depending on current setting.
  * 
  *@param cat_ptr the category name for this message.
  *@param level logging level of the message. 
  *@param fmt_ptr the format string as in a typical printf statement.
  *@... the variable length argument list for the message.
  *
  */
 void etchlog_report(const char *cat_ptr, etchlog_level level, const char *fmt_ptr, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
 