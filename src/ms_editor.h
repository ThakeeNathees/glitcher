/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef MS_EDITOR
#define MS_EDITOR

#include "common.h"
#include "types.h"
#include <Node2D.hpp>

#include <Font.hpp>
#include <InputEvent.hpp>

#include <algorithm>
#include <vector>
#include <string>


class MSEditor : public Node2D {
  GODOT_CLASS(MSEditor, Node2D);

  Ref<Font> font; // Current font.
  std::vector<std::string> lines; // Array of lines.

  Cursor cursor;
  Selection select;
  Vector2 draw_start;
  Vector2 char_size;

  int line_height = 10;  // Number of rows at once visible.
  int first_line = 0;    // First visible line number 0 based.

public:
  static void _register_methods();

  void _init();
  void _ready();
  void _process();
  void _draw();
  void _input(Variant event);

private:
  void clear_selected();
  void handle_backspace();
  void handle_navigation(int64_t key);
  bool is_key_navigation(int64_t key);

  void ensure_cursor_visible();

};



#endif // MS_EDITOR
