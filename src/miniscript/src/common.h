/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef MS_COMMON_H
#define MS_COMMON_H

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// miniscript visibility macros. define MS_DLL for using miniscript as a 
// shared library and define MS_COMPILE to export symbols.

#ifdef _MSC_VER
  #define _MS_EXPORT __declspec(dllexport)
  #define MS_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
  #define _MS_EXPORT __attribute__((visibility ("default")))
  #define _MS_IMPORT
#else
  #define _MS_EXPORT
  #define _MS_IMPORT
#endif

#ifdef MS_DLL
  #ifdef MS_COMPILE
    #define MS_PUBLIC _MS_EXPORT
  #else
    #define MS_PUBLIC _MS_IMPORT
  #endif
#else
  #define MS_PUBLIC
#endif


#endif //MS_COMMON_H
