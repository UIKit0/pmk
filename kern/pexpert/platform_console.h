#ifndef PLATFORM_CONSOLE_H
#define PLATFORM_CONSOLE_H

/**
 * Defines the different classes of console messages.
 */
typedef enum {
	kPlatformConsoleCritical,
	kPlatformConsoleCriticalError,
	kPlatformConsoleError,
	kPlatformConsoleWarning,
	kPlatformConsoleSuccess,
	kPlatformConsoleNormal,
	kPlatformConsoleDebug,
	kPlatformConsoleEmphasis
} platform_console_type_t;

/**
 * Clears the console.
 */
extern void platform_console_clear(void);

/**
 * Prints a character to the current console. This can print to the display,
 * a debugger, a serial interface, and more. Special characters like "\n" must
 * be handled specifically.
 */
extern void platform_console_putc(char c);

/**
 * Prints a formatted string to the current console. This works like printf on
 * most systems.
 */
extern int platform_console_printf(const char* format, va_list ap);

/**
 * Changes the type of message to be printed to the console. Consoles needn't
 * do anything with this, but a possible use may be printing coloured text.
 */
extern void platform_console_set_type(platform_console_type_t type);

#endif