#include <types.h>
#include "logging.h"

#include "platform.h"

/**
 * Logs some information. The platform expert will forward this to the platform
 * console, and also keeps track of the information.
 */
void pexpert_log(pexpert_log_level_t level, const char* format, ...) {
	// set the console thing based on the log level
	switch(level) {
		case kLogLevelDebug:
			platform_console_set_type(kPlatformConsoleDebug);
			break;

		case kLogLevelInfo:
			platform_console_set_type(kPlatformConsoleNormal);
			break;

		case kLogLevelSuccess:
			platform_console_set_type(kPlatformConsoleSuccess);
			break;

		case kLogLevelWarning:
			platform_console_set_type(kPlatformConsoleWarning);
			break;

		case kLogLevelError:
			platform_console_set_type(kPlatformConsoleCriticalError);
			break;

		case kLogLevelCritical:
			platform_console_set_type(kPlatformConsoleCritical);
			break;
	}

	// extract variadic arguments
	va_list ap;
	va_start(ap, format);
	platform_console_printf(format, ap);
	va_end(ap);	
}