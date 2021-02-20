/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef COMMON_H
#define COMMON_H

#include <GlobalConstants.hpp>
#include <Godot.hpp>
using namespace godot;

#ifdef DEBUG

#include <stdio.h>

#ifdef _MSC_VER
  #define DEBUG_BREAK() __debugbreak()
#else
  #define DEBUG_BREAK() __builtin_trap()
#endif

#define ASSERT(condition, message)                                   \
  do {                                                               \
    if (!(condition)) {                                              \
      fprintf(stderr, "Assertion failed: %s\n\tat %s() (%s:%i)\n",   \
        message, __func__, __FILE__, __LINE__);                      \
      DEBUG_BREAK();                                                 \
      abort();                                                       \
    }                                                                \
  } while (false)

#define UNREACHABLE()                                                \
  do {                                                               \
    fprintf(stderr, "Execution reached an unreachable path\n"        \
      "\tat %s() (%s:%i)\n", __func__, __FILE__, __LINE__);          \
    abort();                                                         \
  } while (false)

#else

#define DEBUG_BREAK()

#define ASSERT(condition, message) do { } while (false)

// Reference : https://github.com/wren-lang/
#if defined( _MSC_VER )
  #define UNREACHABLE() __assume(0)
#elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
  #define UNREACHABLE() __builtin_unreachable()
#else
  #define UNREACHABLE()
#endif

#endif // DEBUG

#define TODO ASSERT(false, "TODO")
#define OOPS "Oops a bug!! report plese."

#endif // COMMON_H
