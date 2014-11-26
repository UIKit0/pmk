#ifndef PLATFORM_IO
#define PLATFORM_IO

/**
 * Bit flags indicating the state of the platform's IO space.
 */
typedef enum {
	kPlatformIOSpaceExists = (1 << 0)
} kPlatformIOProperties;

/**
 * Gets information about this platform's IO space, and its properties.
 */
extern unsigned int platform_io_properties(void);

/*
 * Write a byte to system IO port
 */
extern void platform_io_outb(unsigned int port, uint8_t val);

/*
 * Read a byte from a system IO port
 */
extern uint8_t platform_io_inb(unsigned int port);

/*
 * Write a word to system IO port
 */
extern void platform_io_outw(unsigned int port, uint16_t val);

/*
 * Read a word from a system IO port
 */
extern uint16_t platform_io_inw(unsigned int port);

/*
 * Write a dword to system IO port
 */
extern void platform_io_outl(unsigned int port, uint32_t val);

/*
 * Read a dword from a system IO port
 */
extern uint32_t platform_io_inl(unsigned int port);

/*
 * Wait for a system IO operation to complete.
 */
extern void platform_io_wait(void);

#endif