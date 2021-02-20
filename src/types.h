/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef MS_TYPES_H
#define MS_TYPES_H

#include "common.h"

/*****************************************************************************
 * API                                                                       *
 *****************************************************************************/

class _ErrorInfo : public Reference {
  GODOT_CLASS(_ErrorInfo, Reference);

public:
  String message; //< The error message;
  String file;    //< The script file path.
  int line = 0;   //< 1 based error line.
  static void _register_methods() {
    register_property<_ErrorInfo, String>("message", &_ErrorInfo::message, "");
    register_property<_ErrorInfo, String>("file", &_ErrorInfo::file, "");
    register_property<_ErrorInfo, int>("line", &_ErrorInfo::line, 0);
  }

  // This method is required for all native scripts.
  void _init() {}

};

/*****************************************************************************
 * EDITOR                                                                    *
 *****************************************************************************/

 // Because godot vectors are floats.
struct Vector2i {
  Vector2i(int x = 0, int y = 0) : x(x), y(y) {}
  int x = 0, y = 0;
  static const Vector2i ZERO;

  bool operator==(const Vector2i& other) {
    return x == other.x && y == other.y;
  }
  bool operator!=(const Vector2i& other) {
    return !operator==(other);
  }

  bool operator<(const Vector2i& other) {
    if (y < other.y) return true;
    if (y > other.y) return false;
    if (x < other.x) return true;
    return false;
  }

};

struct Cursor {
  Vector2i pos;
  int intended = 0; // Intended column.
};

struct Selection {
  Vector2i start, end;
  void clear() {
    start = Vector2i::ZERO;
    end = Vector2i::ZERO;
  }

  bool is_active() {
    return start != Vector2i::ZERO || end != Vector2i::ZERO;
  }

  Vector2i get_first() {
    if (start < end) return start;
    return end;
  }

  Vector2i get_second() {
    if (start < end) return end;
    return start;
  }

};


#endif // MS_TYPES_H
