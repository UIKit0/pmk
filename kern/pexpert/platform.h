/**
 * Defines various functions for interfacing with the compiled-in platform
 * modules.
 *
 * In addition, this defines the prototypes of the platform functions for these
 * modules, if the "PEXPERT_DEFINE_PROTOTYPES" define is set.
 */
#ifndef PEXPERT_H
#define PEXPERT_H

#include <types.h>

// all platform modules define names
extern const char *platform_name;


/// Initialisation
#include "platform_init.h"

/// Boot arguments
#include "platform_bootarg.h"

/// Console
#include "platform_console.h"

/// Interrupts

/// Paging
#include "platform_paging.h"

/**
 * Performs initialisation of the platform.
 */
void pexpert_init(void);

#endif