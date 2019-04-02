/**
 * @file strings.h
 * 
 * Provide header file for string comparison. These functions are not built-in
 * functions on the XC16 compiler, and must be defined. The implementation is
 * provided in the src folder of embbedded-software.
 * 
 * Created on: Mar 4, 2019
 *    @author: Nate Hoffman
 * 
 * @version 1.0.0 Initial function definitions
 */

#ifndef STRINGS_H
#define STRINGS_H

#include "stddef.h"

int strcasecmp(const char *, const char *);
int strncasecmp(const char *, const char *, size_t);

#endif // STRINGS_H
