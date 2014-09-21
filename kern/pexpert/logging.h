#ifndef PEXPERT_LOGGING_H
#define PEXPERT_LOGGING_H

typedef enum {
	kLogLevelDebug,
	kLogLevelInfo,
	kLogLevelSuccess,
	kLogLevelWarning,
	kLogLevelError,
	kLogLevelCritical
} pexpert_log_level_t;

/**
 * Logs some information. The platform expert will forward this to the platform
 * console, and also keeps track of the information.
 */
void pexpert_log(pexpert_log_level_t level, const char* format, ...) __attribute__ ((format (printf, 2, 3)));

#endif