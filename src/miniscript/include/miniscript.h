/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef MINISCRIPT_H
#define MINISCRIPT_H

#include <stdint.h>
#include <stdbool.h>

// The version number macros.
#define MS_VERSION_MAJOR 0
#define MS_VERSION_MINOR 1
#define MS_VERSION_PATCH 0

// String representation of the value.
#define MS_VERSION_STRING "0.1.0"

typedef struct VM VM;
typedef struct String String;

// Nan-Tagging could be disable for debugging/portability purposes.
// To disable define `VAR_NAN_TAGGING 0`, otherwise it defaults to Nan-Tagging.
#ifndef VAR_NAN_TAGGING
  #define VAR_NAN_TAGGING 1
#endif

#if VAR_NAN_TAGGING
typedef uint64_t Var;
#else
typedef struct Var Var;
#endif

#endif // MINISCRIPT_H
